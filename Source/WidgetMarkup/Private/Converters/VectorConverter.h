// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "Converter.h"
#include "Templates/ChooseClass.h"

template <typename T, int D>
class TVectorConverter : public FConverter
{
public:
	using This = TVectorConverter<T, D>;

	struct FInvalid {};

	template <bool Predicate, typename TrueClass, typename FalseClass>
	using TChooseClassResult = typename TChooseClass<Predicate, TrueClass, FalseClass>::Result;
	
	using FVector =
		TChooseClassResult<D == 2, UE::Math::TVector2<T>,
		TChooseClassResult<D == 3, UE::Math::TVector<T>,
		TChooseClassResult<D == 4, UE::Math::TVector4<T>,
		FInvalid
		>>>;

	static_assert(!TIsSame<FVector, FInvalid>::Value);

	static TSharedRef<FConverter> Create()
	{
		return MakeShared<This>();
	}

	static bool Convert(const FStringView& String, FVector& OutValue)
	{
		TArray<FString> Components;
		FString(String).ParseIntoArray(Components, TEXT(","));
		auto NumComponents = Components.Num();
		if (NumComponents != D)
		{
			return false;
		}
		for (int Index = 0; Index < NumComponents; Index++)
		{
			if (!LexTryParseString(OutValue.Component(Index), *Components[Index].TrimStartAndEnd()))
			{
				return false;
			}
		}
		return true;
	}

protected:
	virtual bool Convert(const FProperty& Property, void* Data, const FStringView& String) override
	{
		check(Property.IsA<FStructProperty>());
		auto& StructProperty = *CastField<FStructProperty>(&Property);
		check(StructProperty.Struct == TBaseStructure<FVector>::Get());
		FVector Vector;
		if (!Convert(String, Vector))
		{
			return false;
		}
		Property.CopyCompleteValue(Data, &Vector);
		return true;
	}
};
