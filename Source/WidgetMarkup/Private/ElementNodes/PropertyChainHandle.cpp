// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyChainHandle.h"

#include "ConverterRegistry.h"
#include "UObject/UnrealType.h"

TSharedPtr<FPropertyChainHandle> FPropertyChainHandle::Create(UObject* Object, const TArray<FString>& PropertyNames)
{
	if (!Object || PropertyNames.IsEmpty())
	{
		return nullptr;
	}
	TSharedPtr<FPropertyChainHandle> PropertyChainHandle = MakeShareable(new FPropertyChainHandle());
	PropertyChainHandle->Object = Object;
	auto PropertyChain = MakeShared<FEditPropertyChain>();
	PropertyChainHandle->PropertyChain = PropertyChain;
	UStruct* Struct = Object->GetClass();
	void* Ptr = Object;
	for (int Index = 0; Index < PropertyNames.Num(); Index++)
	{
		auto Property = Struct->FindPropertyByName(FName(*PropertyNames[Index]));
		if (!Property)
		{
			return nullptr;
		}
		if (PropertyChain->IsEmpty())
		{
			PropertyChain->AddHead(Property);
		}
		else
		{
			PropertyChain->AddTail(Property);
		}
		if (auto StructProperty = CastField<FStructProperty>(Property))
		{
			Struct = StructProperty->Struct;
			Ptr = StructProperty->ContainerPtrToValuePtr<void>(Ptr);
		}
		else if (auto ObjectPropertyBase = CastField<FObjectPropertyBase>(Property))
		{
			UObject* ObjectValue = ObjectPropertyBase->GetObjectPropertyValue_InContainer(Ptr);
			Ptr = ObjectValue;
			Struct = ObjectValue ? ObjectValue->GetClass() : ObjectPropertyBase->PropertyClass.Get();
		}
		else
		{
			if (Index != PropertyNames.Num() - 1)
			{
				return nullptr;
			}
		}
	}
	return PropertyChainHandle;
}

TSharedPtr<FPropertyChainHandle> FPropertyChainHandle::Create(UObject* Object, const FStringView& PropertyPath)
{
	TArray<FString> PropertyNames;
	FString(PropertyPath).ParseIntoArray(PropertyNames, TEXT("."));
	return Create(Object, PropertyNames);
}

FProperty* FPropertyChainHandle::GetTailProperty() const
{
	auto Tail = PropertyChain->GetTail();
	if (!Tail)
	{
		return nullptr;
	}
	return Tail->GetValue();
}

void* FPropertyChainHandle::GetTailContainer() const
{
	void* Container = Object.Get();
	if (!Container)
	{
		return nullptr;
	}
	auto Node = PropertyChain->GetHead();
	if (!Node)
	{
		return nullptr;
	}
	for (; Node->GetNextNode(); Node = Node->GetNextNode())
	{
		if (auto StructProperty = CastField<FStructProperty>(Node->GetValue()))
		{
			Container = StructProperty->ContainerPtrToValuePtr<void>(Container);
		}
		else if (auto ObjectPropertyBase = CastField<FObjectPropertyBase>(Node->GetValue()))
		{
			Container = ObjectPropertyBase->GetObjectPropertyValue_InContainer(Container);
		}
		else
		{
			ensure(false);
		}
	}
	return Container;
}

bool FPropertyChainHandle::SetValue(const void* Data) const
{
	auto TailContainer = GetTailContainer();
	auto TailProperty = GetTailProperty();
	if (TailContainer && TailProperty)
	{
		TailProperty->SetValue_InContainer(TailContainer, Data);
		return true;
	}
	return false;
}

bool FPropertyChainHandle::SetValue(const FStringView& ValueString) const
{
	auto TailProperty = GetTailProperty();
	void* Data = TailProperty->AllocateAndInitializeValue();
	if (!FConverterRegistry::Get().Convert(*TailProperty, Data, ValueString))
	{
		TailProperty->DestroyAndFreeValue(Data);
		return false;
	}
	bool bOK = SetValue(Data);
	TailProperty->DestroyAndFreeValue(Data);
	Data = nullptr;
	return bOK;
}

TSharedPtr<FPropertyChainHandle> FPropertyChainHandle::GetChildHandle(const FStringView& ChildName) const
{
	TSharedPtr<FPropertyChainHandle> PropertyChainHandle = MakeShareable(new FPropertyChainHandle());
	auto NewPropertyChain = MakeShared<FEditPropertyChain>();
	PropertyChainHandle->Object = Object;
	PropertyChainHandle->PropertyChain = NewPropertyChain;
	for (auto Node = PropertyChain->GetHead(); Node; Node = Node->GetNextNode())
	{
		if (NewPropertyChain->IsEmpty())
		{
			NewPropertyChain->AddHead(Node->GetValue());
		}
		else
		{
			NewPropertyChain->AddTail(Node->GetValue());
		}
	}
	auto TailProperty = GetTailProperty();
	if (auto StructProperty = CastField<FStructProperty>(TailProperty))
	{
		if (auto Struct = StructProperty->Struct)
		{
			NewPropertyChain->AddTail(Struct->FindPropertyByName(FName(ChildName)));
			return PropertyChainHandle;
		}
	}
	else if (auto ObjectPropertyBase = CastField<FObjectPropertyBase>(TailProperty))
	{
		if (auto TailContainer = GetTailContainer())
		{
			auto Class = static_cast<UObject*>(TailContainer)->GetClass();
			NewPropertyChain->AddTail(Class->FindPropertyByName(FName(ChildName)));
			return PropertyChainHandle;
		}
	}
	return nullptr;
}
