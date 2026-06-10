// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "ElementNode.h"
#include "Styles/WidgetStyleSheet.h"

class FStyleElementNode;

class FStyleSheetElementNode : public FElementNode
{
	DECLARE_ELEMENT_NODE(FStyleSheetElementNode, FElementNode)

public:
	static TSharedRef<FElementNode> Create();

	virtual FResult OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct) override;
	virtual FResult OnEnd() override;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child) override;
	virtual UObject* GetObject() const override { return StyleSheetObject; }
	virtual UStruct* GetPropertyOwnerStruct() const override { return UWidgetStyleSheet::StaticClass(); }
	virtual bool HasProperty(const FStringView& AttributeName) override;
	virtual void SetElementData(const TCHAR* InElementData) override;

	UWidgetStyleSheet* GetStyleSheet() const { return StyleSheetObject; }

	/** Called by StyleSheetInheritPropertyRun during attribute processing. */
	void ResolveInheritFromPath(const FString& Path);

private:
	TObjectPtr<UWidgetStyleSheet> StyleSheetObject;
	TArray<TSharedPtr<FStyleElementNode>> StyleNodes;
	FString RawElementData;
};
