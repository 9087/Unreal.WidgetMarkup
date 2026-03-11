// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "EnumConverter.h"

#include "UObject/UnrealType.h"

TSharedRef<FConverter> FEnumConverter::Create()
{
	return MakeShared<FEnumConverter>();
}

bool FEnumConverter::Convert(const FStringView& String, UEnum* Enum, int64& OutValue)
{
	if (!Enum)
	{
		return false;
	}
	OutValue = Enum->GetValueByNameString(FString(String), EGetByNameFlags::CaseSensitive);
	return OutValue != INDEX_NONE;
}

bool FEnumConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	if (auto* EnumProperty = CastField<FEnumProperty>(&Property))
	{
		UEnum* Enum = EnumProperty->GetEnum();
		int64 EnumValue = INDEX_NONE;
		if (!Convert(String, Enum, EnumValue))
		{
			return false;
		}
		EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(Data, EnumValue);
		return true;
	}
	// FByteProperty may also be TEnumAsByte (with Enum)
	if (auto* ByteProperty = CastField<FByteProperty>(&Property))
	{
		if (UEnum* Enum = ByteProperty->GetIntPropertyEnum())
		{
			int64 EnumValue = INDEX_NONE;
			if (!Convert(String, Enum, EnumValue))
			{
				return false;
			}
			ByteProperty->SetIntPropertyValue(Data, static_cast<int64>(EnumValue));
			return true;
		}
	}
	return false;
}
