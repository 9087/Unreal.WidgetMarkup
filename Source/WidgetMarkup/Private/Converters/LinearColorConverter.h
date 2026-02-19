// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "Converter.h"

class FLinearColorConverter : public FConverter
{
public:
	static TSharedRef<FConverter> Create();
	static bool Convert(const FStringView& String, FLinearColor& OutValue);

protected:
	virtual bool Convert(const FProperty& Property, void* Data, const FStringView& String) override;
};