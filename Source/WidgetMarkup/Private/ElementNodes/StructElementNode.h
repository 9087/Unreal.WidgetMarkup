// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "BufferedPropertyContext.h"
#include "ElementNode.h"

struct FPropertyBuffer;

class FStructElementNode : public FElementNode
{
	DECLARE_ELEMENT_NODE(FStructElementNode, FElementNode)

public:
	static TSharedRef<FElementNode> Create();

	virtual ~FStructElementNode() override;

	void* GetStructMemory() const;
	UScriptStruct* GetScriptStruct() const;
	const FBufferedPropertyContext& GetBufferedPropertyContext() const;

	//~Begin FElementNode interface
	virtual UStruct* GetPropertyOwnerStruct() const override;

protected:
	virtual FResult OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct) override;
	virtual FResult OnEnd() override;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child) override;
	virtual bool HasProperty(const FStringView& AttributeName) override;
	//~End FElementNode interface

private:
	void Cleanup();

	UScriptStruct* ScriptStruct = nullptr;
	FStructProperty* ValueProperty = nullptr;
	TSharedPtr<FPropertyBuffer> StructPropertyBuffer;
	FBufferedPropertyContext StructBufferedPropertyContext;
};
