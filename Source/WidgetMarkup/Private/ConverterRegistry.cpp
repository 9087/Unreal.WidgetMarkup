// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "ConverterRegistry.h"

#include "Converter.h"
#include "UObject/UnrealType.h"

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
	FName ConverterName;
	if (Property.IsA<FStructProperty>())
	{
		ConverterName = CastField<FStructProperty>(&Property)->Struct->GetFName();
	}
	else if (Property.IsA<FEnumProperty>())
	{
		ConverterName = NAME_EnumProperty;
	}
	else if (const FByteProperty* ByteProperty = CastField<FByteProperty>(&Property))
	{
		ConverterName = ByteProperty->GetIntPropertyEnum() ? NAME_EnumProperty : FieldClass->GetFName();
	}
	else
	{
		ConverterName = FieldClass->GetFName();
	}
	auto OnCreateConverterPtr = CreateConverterDelegateMap.Find(ConverterName);
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
