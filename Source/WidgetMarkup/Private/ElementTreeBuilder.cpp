// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "ElementTreeBuilder.h"

#include "ElementNode.h"
#include "ElementNodeFactory.h"
#include "WidgetMarkupModule.h"
#include "ElementNodes/BlueprintVariableElementNode.h"
#include "ElementNodes/ObjectElementNode.h"
#include "ElementNodes/PropertyElementNode.h"
#include "Utilities/TypeResolver.h"
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
	TSharedPtr<FElementNode> ElementNode;

	const FString ElementNameString(ElementName);
	if (ElementNameString.Equals(TEXT("Variable"), ESearchCase::IgnoreCase))
	{
		ElementNode = FBlueprintVariableElementNode::Create();
	}
	else
	{
		ElementNode = FElementNodeFactory::Get().CreateElementNode(Outer, ElementName, Struct);
	}

	if (!ElementNode.IsValid())
	{
		TSharedPtr<FElementNode> ObjectNode = Context.GetLastObjectNode();
		if (!ObjectNode.IsValid())
		{
			UE_LOG(LogWidgetMarkup, Warning, TEXT("Unknown element '%s' at line %d."), ElementName, XmlFileLineNumber);
			return false;
		}

		UObject* Object = ObjectNode->GetObject();
		TSharedPtr<FElementNode> ParentNode = Context.GetLastNode();
		UStruct* OwnerStruct = ParentNode.IsValid() ? ParentNode->GetPropertyOwnerStruct() : nullptr;
		TSharedRef<IPropertyRun> PropertyRun = WidgetMarkupModule->CreatePropertyRun(OwnerStruct, FName(ElementName));
		FStringView PropertyName(ElementName);
		FStringView PropertyValue(ElementData ? ElementData : TEXT(""));
		if (PropertyRun->OnBegin(Context, Object, PropertyName, PropertyValue))
		{
			return true;
		}
	}

	if (ElementNode)
	{
		FElementNode::FResult Result = ElementNode->Begin(Context, Outer, Struct);
		if (!Result)
		{
			ElementNode = nullptr;
		}
	}
	if (!ElementNode)
	{
		auto FallbackClass = TTypeResolver<UClass>::Resolve(FStringView(ElementName));
		if (!FallbackClass)
		{
			return false;
		}
		TSharedPtr<FElementNode> FallbackObjectNode = MakeShared<FObjectElementNode>();
		auto FallbackResult = FallbackObjectNode->Begin(Context, Outer, FallbackClass);
		if (!FallbackResult)
		{
			FallbackResult.PrintOnFailure();
			return false;
		}
		ElementNode = FallbackObjectNode;
		Struct = FallbackClass;
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

	if (FBlueprintVariableElementNode* VariableElementNode = CastElementNode<FBlueprintVariableElementNode>(Current.Get()))
	{
		if (!VariableElementNode->ApplyVariableAttribute(PropertyName, PropertyValue).PrintOnFailure())
		{
			return false;
		}
		return true;
	}

	UObject* Object = Current->GetObject();
	if (!Object)
	{
		TSharedPtr<FElementNode> ObjectNode = Context.GetLastObjectNode();
		Object = ObjectNode.IsValid() ? ObjectNode->GetObject() : nullptr;
	}
	TSharedRef<IPropertyRun> PropertyRun = WidgetMarkupModule->CreatePropertyRun(Current->GetPropertyOwnerStruct(), FName(PropertyName));
	auto Result = PropertyRun->OnBegin(Context, Object, PropertyName, PropertyValue);
	if (Result)
	{
		Result = PropertyRun->OnEnd(Context);
	}
	return !!Result;
}

bool FElementTreeBuilder::ProcessClose(const TCHAR* Element)
{
	auto Current = GetCurrentElementNode();
	if (!Current.IsValid())
	{
		UE_LOG(LogWidgetMarkup, Warning, TEXT("Close element '%s' has no open element."), Element);
		return false;
	}

	if (FPropertyElementNode* PropertyElementNode = CastElementNode<FPropertyElementNode>(Current.Get()))
	{
		return PropertyElementNode->GetPropertyRunInternal()->OnEnd(Context) ? true : false;
	}

	Current = Context.Pop();
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
	Collector.AddReferencedObject(Outer);
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
