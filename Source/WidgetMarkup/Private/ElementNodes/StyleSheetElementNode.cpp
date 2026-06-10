// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "StyleSheetElementNode.h"

#include "StyleElementNode.h"
#include "WidgetMarkupModule.h"

IMPLEMENT_ELEMENT_NODE(FStyleSheetElementNode, FElementNode)

TSharedRef<FElementNode> FStyleSheetElementNode::Create()
{
	return MakeShared<FStyleSheetElementNode>();
}

FElementNode::FResult FStyleSheetElementNode::OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct)
{
	StyleSheetObject = NewObject<UWidgetStyleSheet>(Outer, NAME_None, RF_Transient | RF_Public);
	if (!StyleSheetObject) return FResult::Failure().Error(FText::FromString(TEXT("StyleSheet: failed to create UWidgetStyleSheet.")));

	// Text-content Inherit path (resolved immediately).
	if (!RawElementData.IsEmpty())
	{
		ResolveInheritFromPath(RawElementData);
	}

	return FResult::Success();
}

void FStyleSheetElementNode::ResolveInheritFromPath(const FString& Path)
{
	FWidgetMarkupModule& Module = FModuleManager::GetModuleChecked<FWidgetMarkupModule>(TEXT("WidgetMarkup"));
	StyleSheetObject->Inherit = Module.GetObjectOrCompileFromPackage<UWidgetStyleSheet>(Path);
}

void FStyleSheetElementNode::SetElementData(const TCHAR* InElementData)
{
	if (InElementData)
	{
		RawElementData = FString(InElementData).TrimStartAndEnd();
	}
}

bool FStyleSheetElementNode::HasProperty(const FStringView& AttributeName)
{
	// Only expose Inherit for attribute processing (handled by StyleSheetInheritPropertyRun).
	return AttributeName == TEXT("Inherit");
}

FElementNode::FResult FStyleSheetElementNode::OnEnd()
{
	check(StyleSheetObject);

	for (const TSharedPtr<FStyleElementNode>& Node : StyleNodes)
	{
		if (Node.IsValid())
		{
			StyleSheetObject->AddOrReplaceStyleEntry(Node->MakeStyle());
		}
	}

	return FResult::Success();
}

FElementNode::FResult FStyleSheetElementNode::OnAddChild(const TSharedRef<FElementNode>& Child)
{
	if (auto StyleNode = CastElementNode<FStyleElementNode>(TSharedPtr<FElementNode>(Child)))
	{
		StyleNodes.Add(StaticCastSharedPtr<FStyleElementNode>(TSharedPtr<FElementNode>(Child)));
		return FResult::Success();
	}
	return FResult::Failure().Error(FText::FromString(TEXT("StyleSheet element only accepts Style child elements.")));
}
