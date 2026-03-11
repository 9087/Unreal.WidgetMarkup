// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "SlateChildSizeConverter.h"

#include "Components/SlateWrapperTypes.h"
#include "UObject/UnrealType.h"

TSharedRef<FConverter> FSlateChildSizeConverter::Create()
{
	return MakeShared<FSlateChildSizeConverter>();
}

bool FSlateChildSizeConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	check(Property.IsA<FStructProperty>());
	auto& StructProperty = *CastField<FStructProperty>(&Property);
	check(StructProperty.Struct == TBaseStructure<FSlateChildSize>::Get());

	FString View(String);
	View.TrimStartAndEndInline();

	FSlateChildSize Out;
	Out.Value = 1.0f;
	Out.SizeRule = ESlateSizeRule::Fill;

	if (View.Equals(TEXT("Automatic"), ESearchCase::CaseSensitive))
	{
		Out.SizeRule = ESlateSizeRule::Automatic;
		Property.CopyCompleteValue(Data, &Out);
		return true;
	}

	float ParsedValue;
	if (LexTryParseString(ParsedValue, *View))
	{
		Out.SizeRule = ESlateSizeRule::Fill;
		Out.Value = ParsedValue;
		Property.CopyCompleteValue(Data, &Out);
		return true;
	}

	return false;
}
