// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/FieldPath.h"

#include "PropertyBuffer.generated.h"

class FProperty;
class FReferenceCollector;
class UStruct;

USTRUCT()
struct FPropertyBuffer
{
	GENERATED_BODY()

public:
	FPropertyBuffer() = default;
	explicit FPropertyBuffer(FProperty* InProperty);
	FPropertyBuffer(FProperty* InProperty, const FStringView& InValueString);
	~FPropertyBuffer();

	FPropertyBuffer(const FPropertyBuffer& Other);
	FPropertyBuffer& operator=(const FPropertyBuffer& Other);

	FPropertyBuffer(FPropertyBuffer&& Other) noexcept;
	FPropertyBuffer& operator=(FPropertyBuffer&& Other) noexcept;

	void SetProperty(FProperty* InProperty);

	bool HasValue() const;
	void* GetValueData() const { return ValueData; }
	FProperty* GetProperty() const;
	UStruct* GetStruct() const;

	bool Serialize(FArchive& Ar);
	void AddStructReferencedObjects(FReferenceCollector& Collector) const;
	bool ExportTextItem(FString& ValueStr, const FPropertyBuffer& DefaultValue, UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const;
	bool ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText);
	bool ExportValueText(FString& OutValueText, int32 PortFlags = 0, UObject* Parent = nullptr, UObject* ExportRootScope = nullptr) const;
	bool ImportValueText(const TCHAR* Buffer, int32 PortFlags = 0, UObject* Parent = nullptr, FOutputDevice* ErrorText = nullptr);

private:
	bool Initialize();
	void Uninitialize();

	void CopyFrom(const FPropertyBuffer& Other);
	void MoveFrom(FPropertyBuffer&& Other);

	FProperty* ResolvedProperty() const;

	UPROPERTY()
	TFieldPath<FProperty> PropertyFieldPath;

	mutable FProperty* CachedProperty = nullptr;
	void* ValueData = nullptr;
};

template<>
struct TStructOpsTypeTraits<FPropertyBuffer> : public TStructOpsTypeTraitsBase2<FPropertyBuffer>
{
	enum
	{
		WithSerializer = true,
		WithAddStructReferencedObjects = true,
		WithExportTextItem = true,
		WithImportTextItem = true,
	};
};
