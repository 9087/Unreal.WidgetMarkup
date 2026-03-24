// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyBuffer.h"

#include "UObject/UnrealType.h"

FPropertyBuffer::FPropertyBuffer(FProperty* InRootProperty)
	: RootProperty(InRootProperty)
{
}

bool FPropertyBuffer::Initialize()
{
	if (!RootProperty)
	{
		return false;
	}
	if (RootValueData)
	{
		return true;
	}

	RootValueData = RootProperty->AllocateAndInitializeValue();
	if (!RootValueData)
	{
		RootStruct = nullptr;
		return false;
	}

	if (const FStructProperty* StructProperty = CastField<FStructProperty>(RootProperty))
	{
		RootStruct = StructProperty->Struct;
	}
	else
	{
		RootStruct = nullptr;
	}

	return true;
}

void FPropertyBuffer::Uninitialize()
{
	if (RootProperty && RootValueData)
	{
		RootProperty->DestroyAndFreeValue(RootValueData);
	}

	RootValueData = nullptr;
	RootStruct = nullptr;
	ArrayElementTargetProperty = nullptr;
	ArrayElementTargetAddress = nullptr;
}

bool FPropertyBuffer::HasRootValue() const
{
	return RootProperty != nullptr && RootValueData != nullptr;
}