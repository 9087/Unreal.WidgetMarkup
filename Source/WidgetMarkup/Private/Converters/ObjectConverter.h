// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "Converter.h"

/**
 * Generic ObjectProperty converter.
 *
 * Step 1: StaticLoadObject (handles Texture, Material, Blueprint, etc.)
 * Step 2: UWidgetMarkupFontProvider::CreateFromFontName (handles system fonts,
 *          internally validates FSlateFontInfo::FontObject).
 */
class FObjectConverter : public FConverter
{
public:
	static TSharedRef<FConverter> Create();

protected:
	virtual bool Convert(const FProperty& Property, void* Data, const FStringView& String) override;
};
