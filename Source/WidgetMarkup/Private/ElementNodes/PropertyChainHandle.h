// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "BufferedPropertyContext.h"

class FProperty;
class FPropertyBuffer;

class FPropertyChainHandle
{
public:
	static TSharedPtr<FPropertyChainHandle> Create(UObject* Object, const FPropertyPath& PropertyPath);
	static TSharedPtr<FPropertyChainHandle> Create(UObject* Object, const FPropertyPath& PropertyPath, const FBufferedPropertyContext& InBufferedPropertyContext);
	static TSharedPtr<FPropertyChainHandle> Create(UObject* Object, const FStringView& PropertyPathString);
	static TSharedPtr<FPropertyChainHandle> Create(UObject* Object, const FStringView& PropertyPathString, const FBufferedPropertyContext& InBufferedPropertyContext);

	FProperty* GetTailProperty() const;
	void* GetTailContainer() const;
	void* GetTailValueAddress() const;

	bool IsArrayProperty() const;

	bool SetValue(const void* Data) const;
	bool SetValue(const FStringView& ValueString) const;

	TSharedPtr<FPropertyChainHandle> GetChildHandle(const FStringView& ChildName) const;
	TSharedPtr<FPropertyChainHandle> GetDirectHandle() const;

private:
	bool Resolve(FProperty*& OutTailProperty, void*& OutTailContainer, void*& OutTailValueAddress) const;
	bool ResolveAgainstObjectRoot(FProperty*& OutTailProperty, void*& OutTailContainer, void*& OutTailValueAddress) const;
	bool ResolveAgainstBufferedRoot(FProperty*& OutTailProperty, void*& OutTailContainer, void*& OutTailValueAddress) const;

	TWeakObjectPtr<UObject> Object;
	FBufferedPropertyContext BufferedPropertyContext;
	FPropertyPath PropertyPath;
};