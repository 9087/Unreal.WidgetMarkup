// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "LinearColorConverter.h"

TSharedRef<FConverter> FLinearColorConverter::Create()
{
	return MakeShared<FLinearColorConverter>();
}

bool FLinearColorConverter::Convert(const FStringView& String, FLinearColor& OutValue)
{
	TArray<FString> Channels;
	FString(String).ParseIntoArray(Channels, TEXT(","));
	auto NumChannels = Channels.Num();
	if (NumChannels != 3 && NumChannels != 4)
	{
		return false;
	}
	for (int Index = 0; Index < NumChannels; Index++)
	{
		if (!LexTryParseString(OutValue.Component(Index), *Channels[Index].TrimStartAndEnd()))
		{
			return false;
		}
	}
	return true;
}

bool FLinearColorConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	check(Property.IsA<FStructProperty>());
	auto& StructProperty = *CastField<FStructProperty>(&Property);
	check(StructProperty.Struct == TBaseStructure<FLinearColor>::Get());
	FLinearColor LinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	if (!Convert(String, LinearColor))
	{
		return false;
	}
	StructProperty.CopyCompleteValue(Data, &LinearColor);
	return true;
}
