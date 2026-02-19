// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "ConverterRegistry.h"

#include "Converter.h"

FConverterRegistry& FConverterRegistry::Get()
{
	static FConverterRegistry ConverterRegistry;
	return ConverterRegistry;
}

bool FConverterRegistry::Register(FName PropertyTypeName, const FOnCreateConverter& OnCreateConverter)
{
	CreateConverterDelegateMap.FindOrAdd(PropertyTypeName) = OnCreateConverter;
	return true;
}

bool FConverterRegistry::Unregister(FName PropertyTypeName)
{
	return CreateConverterDelegateMap.Remove(PropertyTypeName) != 0;
}

TSharedPtr<FConverter> FConverterRegistry::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	auto FieldClass = Property.GetClass();
	if (!FieldClass)
	{
		return nullptr;
	}
	auto OnCreateConverterPtr = CreateConverterDelegateMap.Find(Property.IsA<FStructProperty>() ? CastField<FStructProperty>(&Property)->Struct->GetFName() : FieldClass->GetFName());
	if (!OnCreateConverterPtr)
	{
		return nullptr;
	}
	auto& OnCreateConverter = *OnCreateConverterPtr;
	if (!OnCreateConverter.IsBound())
	{
		return nullptr;
	}
	auto Converter = OnCreateConverter.Execute();
	if (!Converter->Convert(Property, Data, String))
	{
		return nullptr;
	}
	return Converter;
}
