// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "WidgetPropertyPathConverter.h"

#include "Utilities/WidgetPropertyPath.h"

TSharedRef<FConverter> FWidgetPropertyPathConverter::Create()
{
	return MakeShared<FWidgetPropertyPathConverter>();
}

bool FWidgetPropertyPathConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	check(Property.IsA<FStructProperty>());
	auto& StructProperty = *CastField<FStructProperty>(&Property);
	check(StructProperty.Struct == FWidgetPropertyPath::StaticStruct());

	FWidgetPropertyPath Path(String);
	StructProperty.CopyCompleteValue(Data, &Path);
	return true;
}
