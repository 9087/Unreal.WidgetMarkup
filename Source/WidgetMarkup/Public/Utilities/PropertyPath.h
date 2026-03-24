// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EPropertyPathElementType : uint8
{
	Property,
	ArrayIndex,
	MapKey,
};

struct WIDGETMARKUP_API FPropertyPathElement
{
	EPropertyPathElementType Type = EPropertyPathElementType::Property;
	FString Name;
	int32 ArrayIndex = INDEX_NONE;
	bool bIsAny = false;

	static FPropertyPathElement MakeProperty(const FStringView& PropertyName);
	static FPropertyPathElement MakeAnyProperty();
	static FPropertyPathElement MakeArrayIndex(int32 InArrayIndex);
	static FPropertyPathElement MakeAnyArrayIndex();
	static FPropertyPathElement MakeMapKey(const FStringView& MapKey);
	static FPropertyPathElement MakeAnyMapKey();

	bool Matches(const FPropertyPathElement& Candidate) const;
	FString ToString() const;
};

class WIDGETMARKUP_API FPropertyPath
{
public:
	static bool TryParse(const FStringView& InText, FPropertyPath& OutPath, FString* OutError = nullptr);

	void Reset();
	bool IsEmpty() const;
	bool HasAny() const;
	FString ToString() const;
	const FName& GetCanonicalName() const;
	bool operator==(const FPropertyPath& Other) const;
	bool Matches(const FPropertyPath& Candidate) const;
	bool TryMakeRelativeTo(const FPropertyPath& BasePath, FPropertyPath& OutRelativePath) const;

	FPropertyPath WithAppendedProperty(const FStringView& PropertyPathString) const;
	FPropertyPath WithAppendedAnyProperty() const;
	FPropertyPath WithAppendedArrayIndex(int32 ArrayIndex) const;
	FPropertyPath WithAppendedAnyArrayIndex() const;
	FPropertyPath WithAppendedMapKey(const FStringView& MapKey) const;
	FPropertyPath WithAppendedAnyMapKey() const;

	const TArray<FPropertyPathElement>& GetElements() const;

	friend uint32 GetTypeHash(const FPropertyPath& PropertyPath)
	{
		return GetTypeHash(PropertyPath.CanonicalName);
	}

private:
	void AppendProperty(const FStringView& PropertyName);
	void AppendAnyProperty();
	void AppendArrayIndex(int32 ArrayIndex);
	void AppendAnyArrayIndex();
	void AppendMapKey(const FStringView& MapKey);
	void AppendAnyMapKey();
	void RefreshCanonicalPropertyPath();

	TArray<FPropertyPathElement> Elements;
	FString CanonicalString;
	FName CanonicalName;
};