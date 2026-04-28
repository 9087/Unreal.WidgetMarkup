// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/FieldPath.h"
#include "UObject/UnrealType.h"

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

	template <typename TObjectType>
	bool ToArray(TArray<TObjectType*>& OutArray) const
	{
		static_assert(TIsDerivedFrom<TObjectType, UObject>::IsDerived, "FPropertyBuffer::ToArray only supports UObject types.");

		FArrayProperty* BufferedArrayProperty = CastField<FArrayProperty>(GetProperty());
		if (!BufferedArrayProperty || !GetValueData())
		{
			return false;
		}

		FObjectPropertyBase* BufferedInnerObjectProperty = CastField<FObjectPropertyBase>(BufferedArrayProperty->Inner);
		if (!BufferedInnerObjectProperty)
		{
			return false;
		}

		FScriptArrayHelper BufferedArrayHelper(BufferedArrayProperty, GetValueData());
		const int32 BufferedItemCount = BufferedArrayHelper.Num();
		OutArray.Reset();
		OutArray.Reserve(BufferedItemCount);
		for (int32 BufferedIndex = 0; BufferedIndex < BufferedItemCount; ++BufferedIndex)
		{
			void* BufferedElementPointer = BufferedArrayHelper.GetRawPtr(BufferedIndex);
			if (!BufferedElementPointer)
			{
				continue;
			}

			UObject* BufferedObject = BufferedInnerObjectProperty->GetObjectPropertyValue(BufferedElementPointer);
			if (!BufferedObject)
			{
				continue;
			}

			TObjectType* TypedBufferedObject = Cast<TObjectType>(BufferedObject);
			if (!TypedBufferedObject)
			{
				return false;
			}

			OutArray.Add(TypedBufferedObject);
		}

		return true;
	}

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
