// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyBuffer.h"

#include "ConverterRegistry.h"
#include "Misc/OutputDevice.h"
#include "Serialization/StructuredArchiveAdapters.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/UnrealType.h"

FPropertyBuffer::FPropertyBuffer(FProperty* InProperty)
{
	SetProperty(InProperty);
}

FPropertyBuffer::FPropertyBuffer(FProperty* InProperty, const FStringView& InValueString)
{
	SetProperty(InProperty);
	if (!HasValue() || !InProperty || !FConverterRegistry::Get().Convert(*InProperty, ValueData, InValueString))
	{
		SetProperty(nullptr);
	}
}

FPropertyBuffer::~FPropertyBuffer()
{
	Uninitialize();
}

FPropertyBuffer::FPropertyBuffer(const FPropertyBuffer& Other)
{
	CopyFrom(Other);
}

FPropertyBuffer& FPropertyBuffer::operator=(const FPropertyBuffer& Other)
{
	if (this != &Other)
	{
		Uninitialize();
		CopyFrom(Other);
	}

	return *this;
}

FPropertyBuffer::FPropertyBuffer(FPropertyBuffer&& Other) noexcept
{
	MoveFrom(MoveTemp(Other));
}

FPropertyBuffer& FPropertyBuffer::operator=(FPropertyBuffer&& Other) noexcept
{
	if (this != &Other)
	{
		Uninitialize();
		MoveFrom(MoveTemp(Other));
	}

	return *this;
}

void FPropertyBuffer::SetProperty(FProperty* InProperty)
{
	if (InProperty == CachedProperty)
	{
		return;
	}

	Uninitialize();
	CachedProperty = InProperty;
	PropertyFieldPath = InProperty ? TFieldPath<FProperty>(InProperty) : TFieldPath<FProperty>();
	Initialize();
}

bool FPropertyBuffer::Initialize()
{
	FProperty* Property = ResolvedProperty();
	if (!Property)
	{
		return false;
	}
	if (ValueData)
	{
		return true;
	}

	ValueData = Property->AllocateAndInitializeValue();
	if (!ValueData)
	{
		return false;
	}

	return true;
}

void FPropertyBuffer::Uninitialize()
{
	if (CachedProperty && ValueData)
	{
		CachedProperty->DestroyAndFreeValue(ValueData);
	}

	ValueData = nullptr;
}

bool FPropertyBuffer::HasValue() const
{
	return ResolvedProperty() != nullptr && ValueData != nullptr;
}

FProperty* FPropertyBuffer::GetProperty() const
{
	return ResolvedProperty();
}

UStruct* FPropertyBuffer::GetStruct() const
{
	if (const FStructProperty* StructProperty = CastField<FStructProperty>(ResolvedProperty()))
	{
		return StructProperty->Struct;
	}

	return nullptr;
}

bool FPropertyBuffer::Serialize(FArchive& Ar)
{
	Ar << PropertyFieldPath;

	if (Ar.IsLoading())
	{
		if (CachedProperty && ValueData)
		{
			CachedProperty->DestroyAndFreeValue(ValueData);
		}

		ValueData = nullptr;
		CachedProperty = nullptr;

		if (!Initialize())
		{
			return false;
		}
	}

	FProperty* Property = ResolvedProperty();
	if (!Property)
	{
		return true;
	}

	if (!ValueData && !Initialize())
	{
		return false;
	}

	FStructuredArchiveFromArchive StructuredArchive(Ar);
	Property->SerializeItem(StructuredArchive.GetSlot(), ValueData, nullptr);
	if (Ar.IsError())
	{
		return false;
	}

	return true;
}

void FPropertyBuffer::AddStructReferencedObjects(FReferenceCollector& Collector) const
{
	FProperty* Property = ResolvedProperty();
	if (!Property || !ValueData)
	{
		return;
	}

	// Risk boundary: this only reports references when a GC-aware owner forwards to this method during AddReferencedObjects.
	FReferenceCollectorArchive& Archive = Collector.GetVerySlowReferenceCollectorArchive();
	FVerySlowReferenceCollectorArchiveScope ArchiveScope(Archive, nullptr, Property, nullptr);
	FStructuredArchiveFromArchive StructuredArchive(Archive);
	Property->SerializeItem(StructuredArchive.GetSlot(), ValueData, nullptr);
}

bool FPropertyBuffer::ExportTextItem(FString& ValueStr, const FPropertyBuffer& DefaultValue, UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const
{
	const FProperty* Property = ResolvedProperty();
	if (!Property || !ValueData)
	{
		return false;
	}

	const void* DefaultValueData = nullptr;
	if (DefaultValue.HasValue() && DefaultValue.GetProperty() && Property->SameType(DefaultValue.GetProperty()))
	{
		DefaultValueData = DefaultValue.GetValueData();
	}

	Property->ExportTextItem_Direct(ValueStr, ValueData, DefaultValueData, Parent, PortFlags, ExportRootScope);
	return true;
}

bool FPropertyBuffer::ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText)
{
	if (!Buffer || !Initialize())
	{
		return false;
	}

	FProperty* Property = ResolvedProperty();
	if (!Property)
	{
		return false;
	}

	const TCHAR* ImportResult = Property->ImportText_Direct(Buffer, ValueData, Parent, PortFlags, ErrorText);
	if (!ImportResult)
	{
		return false;
	}

	Buffer = ImportResult;
	return true;
}

bool FPropertyBuffer::ExportValueText(FString& OutValueText, int32 PortFlags, UObject* Parent, UObject* ExportRootScope) const
{
	const FProperty* Property = ResolvedProperty();
	if (!Property || !ValueData)
	{
		return false;
	}

	OutValueText.Reset();
	Property->ExportTextItem_Direct(OutValueText, ValueData, nullptr, Parent, PortFlags, ExportRootScope);
	return true;
}

bool FPropertyBuffer::ImportValueText(const TCHAR* Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText)
{
	if (!Buffer)
	{
		return false;
	}

	if (!Initialize())
	{
		return false;
	}

	FProperty* Property = ResolvedProperty();
	if (!Property)
	{
		return false;
	}

	const TCHAR* ImportResult = Property->ImportText_Direct(Buffer, ValueData, Parent, PortFlags, ErrorText);
	if (!ImportResult)
	{
		return false;
	}

	return true;
}

void FPropertyBuffer::CopyFrom(const FPropertyBuffer& Other)
{
	PropertyFieldPath = Other.PropertyFieldPath;
	CachedProperty = nullptr;
	ValueData = nullptr;

	if (!Initialize())
	{
		return;
	}

	if (Other.HasValue())
	{
		if (const FProperty* Property = ResolvedProperty())
		{
			Property->CopyCompleteValue(ValueData, Other.GetValueData());
		}
	}
}

void FPropertyBuffer::MoveFrom(FPropertyBuffer&& Other)
{
	PropertyFieldPath = MoveTemp(Other.PropertyFieldPath);
	CachedProperty = Other.CachedProperty;
	ValueData = Other.ValueData;

	Other.CachedProperty = nullptr;
	Other.ValueData = nullptr;
}

FProperty* FPropertyBuffer::ResolvedProperty() const
{
	if (!CachedProperty)
	{
		CachedProperty = PropertyFieldPath.Get();
	}

	return CachedProperty;
}