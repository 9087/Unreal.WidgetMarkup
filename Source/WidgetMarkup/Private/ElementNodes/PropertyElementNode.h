// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "ElementNode.h"

class FPropertyChainHandle;

class FPropertyElementNode : public FElementNode
{
	DECLARE_ELEMENT_NODE(FPropertyElementNode, FElementNode)

public:
	/** Attribute form: (name, value). Element form: (element name, ElementData). */
	FPropertyElementNode(const FStringView& InPropertyName, const FStringView& InPropertyValue);

	const FString& GetPropertyPath() const { return PropertyPath; }

protected:
	//~Begin FElementNode interface
	virtual FResult Begin(const FContext& Context, UObject* Outer, UStruct* Struct) override;
	virtual FResult End() override;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child) override;
	virtual bool HasProperty(const FStringView& AttributeName) override;
	//~End FElementNode interface

	/** Name/path passed at construction (segment or dotted path). Caller must ensure it outlives this node. */
	FStringView PropertyName;
	/** Full dotted path from nearest object; computed in Begin(). */
	FString PropertyPath;
	/** Value passed at construction (attribute value or ElementData). Caller must ensure it outlives this node. */
	FStringView PropertyValue;

	TSharedPtr<FPropertyChainHandle> PropertyChain;

	/** Child element nodes. SetValue in End() only when Num() == 0. */
	TArray<TSharedRef<FElementNode>> ElementChildren;
};
