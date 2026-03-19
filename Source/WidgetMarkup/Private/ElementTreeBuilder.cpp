// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "ElementTreeBuilder.h"

#include "ElementNode.h"
#include "ElementNodeFactory.h"
#include "WidgetMarkupModule.h"
#include "ElementNodes/PropertyElementNode.h"
#include "Modules/ModuleManager.h"

FElementTreeBuilder::FElementTreeBuilder(UObject* InOuter)
	: Outer(InOuter)
	, WidgetMarkupModule(&FModuleManager::GetModuleChecked<FWidgetMarkupModule>("WidgetMarkup"))
{
}

bool FElementTreeBuilder::ProcessXmlDeclaration(const TCHAR* ElementData, int32 XmlFileLineNumber)
{
	return true;
}

bool FElementTreeBuilder::ProcessElement(const TCHAR* ElementName, const TCHAR* ElementData, int32 XmlFileLineNumber)
{
	UStruct* Struct = nullptr;
	TSharedPtr<FElementNode> ElementNode = FElementNodeFactory::Get().CreateElementNode(Outer, ElementName, Struct);

	if (!ElementNode.IsValid())
	{
		// Fallback: try to create as PropertyElementNode (element form). Full path is computed in Begin() from context.
		TSharedPtr<FElementNode> ObjectNode = Context.GetLastObjectNode();
		if (!ObjectNode.IsValid())
		{
			UE_LOG(LogWidgetMarkup, Warning, TEXT("Unknown element '%s' at line %d."), ElementName, XmlFileLineNumber);
			return false;
		}
		ElementNode = MakeShared<FPropertyElementNode>(FStringView(ElementName), FStringView(ElementData ? ElementData : TEXT("")));
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
		UE_LOG(LogWidgetMarkup, Warning, TEXT("Attribute '%s' has no open element."), AttributeName);
		return false;
	}
	FStringView PropertyName(AttributeName);
	FStringView PropertyValue(AttributeValue);
	UObject* Object = Current->GetObject();
	if (Object && WidgetMarkupModule)
	{
		FCustomAttributeDescriptor Descriptor;
		if (WidgetMarkupModule->FindCustomAttributeDescriptor(Object->GetClass(), FName(PropertyName), Descriptor))
		{
			if (!Descriptor.ApplyDelegate.Execute(Context, Object, Descriptor.TypeName, PropertyValue).PrintOnFailure())
			{
				return false;
			}
			return true;
		}
	}
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
