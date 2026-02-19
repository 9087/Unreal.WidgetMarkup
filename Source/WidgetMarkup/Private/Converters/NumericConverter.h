// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "Converter.h"

template <typename T>
class TNumericConverter : public FConverter
{
public:
	using This = TNumericConverter<T>;

	static TSharedRef<FConverter> Create()
	{
		return MakeShared<This>();
	}

protected:
	virtual bool Convert(const FProperty& Property, void* Data, const FStringView& String) override
	{
		check(Property.IsA<TProperty_Numeric<T>>());
		T Value;
		if (!LexTryParseString(Value, String.GetData()))
		{
			return false;
		}
		auto& NumericProperty = *CastField<TProperty_Numeric<T>>(&Property);
		if constexpr (TIsIntegral<T>::Value)
		{
			if constexpr (TIsSigned<T>::Value)
			{
				NumericProperty.SetIntPropertyValue(Data, static_cast<int64>(Value));
			}
			else
			{
				NumericProperty.SetIntPropertyValue(Data, static_cast<uint64>(Value));
			}
		}
		else
		{
			NumericProperty.SetFloatingPointPropertyValue(Data, static_cast<double>(Value));
		}
		return true;
	}
};