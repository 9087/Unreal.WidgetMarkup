// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "BufferedPropertyContext.h"

#include "ElementNode.h"

class IPropertyRun;
class FPropertyChainHandle;
class FProperty;
struct FPropertyBuffer;

class FPropertyElementNode : public FElementNode, public FGCObject
{
	DECLARE_ELEMENT_NODE(FPropertyElementNode, FElementNode)

	friend class FElementTreeBuilder;
	friend class FPropertyRun;

public:
	static TSharedRef<FElementNode> Create(const FStringView& InPropertyName, const FStringView& InPropertyValue);

	/** Attribute form: (name, value). Element form: (element name, ElementData). */
	FPropertyElementNode(const FStringView& InPropertyName, const FStringView& InPropertyValue, bool bInUseBufferedWrite = false);

	virtual ~FPropertyElementNode() override;

	virtual UStruct* GetPropertyOwnerStruct() const override;

	FString GetPropertyPath() const { return PropertyPath.GetPathName().ToString(); }
	const FString& GetPropertyName() const { return PropertyName; }
	const FString& GetPropertyValue() const { return PropertyValue; }
	TSharedPtr<const FPropertyBuffer> GetPropertyBuffer() const { return BufferedPropertyContext.GetPropertyBuffer(); }
	void SetPropertyRun(TSharedPtr<IPropertyRun> InPropertyRun);

protected:
	//~Begin FGCObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;
	//~End FGCObject interface

	//~Begin FElementNode interface
	virtual FResult OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct) override;
	virtual FResult OnEnd() override;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child) override;
	virtual bool HasProperty(const FStringView& AttributeName) override;
	//~End FElementNode interface

	TSharedPtr<IPropertyRun> GetPropertyRunInternal() const { return PropertyRun; }

	/** Name/path passed at construction (segment or canonical property path). */
	FString PropertyName;
	/** Full canonical property path from nearest object; computed in Begin(). */
	FWidgetPropertyPath PropertyPath;
	/** Buffered write context shared by nested property nodes. */
	FBufferedPropertyContext BufferedPropertyContext;
	/** Value passed at construction (attribute value or ElementData). */
	FString PropertyValue;
	/** When true, writes go to temporary property memory and commit in End(). */
	bool bUseBufferedWrite = false;

	TSharedPtr<FPropertyChainHandle> PropertyChain;

	/** Child element nodes. SetValue in End() only when Num() == 0. */
	TArray<TSharedRef<FElementNode>> ElementChildren;

	TSharedPtr<IPropertyRun> PropertyRun;
};