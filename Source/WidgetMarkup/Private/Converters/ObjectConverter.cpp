// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "ObjectConverter.h"
#include "WidgetMarkupFontProvider.h"

#include "UObject/UnrealType.h"
#include "WidgetMarkupModule.h"

TSharedRef<FConverter> FObjectConverter::Create()
{
	return MakeShared<FObjectConverter>();
}

bool FObjectConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	const FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(&Property);
	if (!ObjectProperty)
	{
		return false;
	}

	const FString SourceToken(String);

	// Step 1: standard UE asset path resolution (Texture, Material, etc.)
	UObject* Loaded = StaticLoadObject(ObjectProperty->PropertyClass, nullptr, *SourceToken);
	if (Loaded)
	{
		ObjectProperty->SetObjectPropertyValue(Data, Loaded);
		return true;
	}

	// Step 2: try font provider (internally validates FSlateFontInfo::FontObject)
	UWidgetMarkupFontProvider* Provider = UWidgetMarkupFontProvider::CreateFromFontName(Property, SourceToken);
	if (Provider)
	{
		ObjectProperty->SetObjectPropertyValue(Data, Provider);
		UE_LOG(LogWidgetMarkup, Display,
			TEXT("ObjectConverter: resolved '%s' (IFontProviderInterface)."),
			*SourceToken);
		return true;
	}

	UE_LOG(LogWidgetMarkup, Warning,
		TEXT("ObjectConverter: could not resolve '%s' as an asset or font name."),
		*SourceToken);
	return false;
}
