// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "ElementTreeBuilder.h"

#include "ElementNode.h"
#include "ElementNodeFactory.h"
#include "WidgetMarkupModule.h"
#include "ElementNodes/PropertyElementNode.h"

FElementTreeBuilder::FElementTreeBuilder(UObject* InOuter)
	: Outer(InOuter)
{
}

bool FElementTreeBuilder::ProcessXmlDeclaration(const TCHAR* ElementData, int32 XmlFileLineNumber)
{
	return true;
}

bool FElementTreeBuilder::ProcessElement(const TCHAR* ElementName, const TCHAR* ElementData, int32 XmlFileLineNumber)
{
	UStruct* Struct = nullptr;
	auto ElementNode = FElementNodeFactory::Get().CreateElementNode(Outer, ElementName, Struct);
	if (!ElementNode.IsValid())
	{
		return false;
	}
	if (!ElementNode->Begin(Context, Outer, Struct).PrintOnFailure())
	{
		return false;
	}
	if (auto Current = GetCurrentElementNode())
	{
		if (!Current->OnAddChild(ElementNode.ToSharedRef()).PrintOnFailure())
		{
			return false;
		}
	}
	Context.Push(ElementNode.ToSharedRef());
	return true;
}

bool FElementTreeBuilder::ProcessAttribute(const TCHAR* AttributeName, const TCHAR* AttributeValue)
{
	auto Current = GetCurrentElementNode();
	if (!Current.IsValid())
	{
		return false;
	}
	FStringView PropertyName(AttributeName);
	FStringView PropertyValue(AttributeValue);
	if (!Current->HasProperty(PropertyName))
	{
		UE_LOG(LogWidgetMarkup, Error, TEXT("Failed to recognize the property name '%s'."), PropertyName.GetData());
		return false;
	}
	TSharedRef<FElementNode> PropertyElementNode = MakeShared<FPropertyElementNode>(PropertyName, PropertyValue);
	if (!PropertyElementNode->Begin(Context, Outer, nullptr).PrintOnFailure())
	{
		return false;
	}
	if (!PropertyElementNode->End().PrintOnFailure())
	{
		return false;
	}
	return true;
}

bool FElementTreeBuilder::ProcessClose(const TCHAR* Element)
{
	auto Current = Context.Pop();
	if (!Current->End().PrintOnFailure())
	{
		return false;
	}
	if (Context.IsEmpty())
	{
		RootElementNode = Current;
	}
	return true;
}

bool FElementTreeBuilder::ProcessComment(const TCHAR* Comment)
{
	return true;
}

void FElementTreeBuilder::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (RootElementNode.IsValid())
	{
		RootElementNode->AddReferencedObjects(Collector);
	}
	Collector.AddReferencedObject(Outer);
	for (auto& ElementNode : Context.GetNodes())
	{
		ElementNode->AddReferencedObjects(Collector);
	}
}

FString FElementTreeBuilder::GetReferencerName() const
{
	return TEXT("WidgetTreeBuilder");
}

TSharedPtr<FElementNode> FElementTreeBuilder::GetRootElementNode()
{
	return RootElementNode;
}

TSharedPtr<FElementNode> FElementTreeBuilder::GetCurrentElementNode() const
{
	return !Context.IsEmpty() ? Context.GetLastNode() : TSharedPtr<FElementNode>();
}
