// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "Converter.h"

class FEnumConverter : public FConverter
{
public:
	static TSharedRef<FConverter> Create();
	static bool Convert(const FStringView& String, UEnum* Enum, int64& OutValue);

	template <typename T>
	static bool Convert(const FStringView& String, T& OutValue)
	{
		auto Enum = StaticEnum<T>();
		if (!Enum)
		{
			return false;
		}
		int64 IntegerValue;
		if (!Convert(String, Enum, IntegerValue))
		{
			return false;
		}
		OutValue = static_cast<T>(IntegerValue);
		return true;
	}

protected:
	virtual bool Convert(const FProperty& Property, void* Data, const FStringView& String) override;
};