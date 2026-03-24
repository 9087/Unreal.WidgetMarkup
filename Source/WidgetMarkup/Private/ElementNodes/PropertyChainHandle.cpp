// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyChainHandle.h"

#include "PropertyBuffer.h"
#include "ConverterRegistry.h"
#include "Utilities/PropertyPathResolver.h"
#include "UObject/UnrealType.h"

TSharedPtr<FPropertyChainHandle> FPropertyChainHandle::Create(UObject* Object, const FPropertyPath& PropertyPath)
{
	return Create(Object, PropertyPath, FBufferedPropertyContext());
}

TSharedPtr<FPropertyChainHandle> FPropertyChainHandle::Create(UObject* Object, const FPropertyPath& PropertyPath,
	const FBufferedPropertyContext& InBufferedPropertyContext)
{
	if (!Object || PropertyPath.IsEmpty())
	{
		return nullptr;
	}

	TSharedPtr<FPropertyChainHandle> PropertyChainHandle = MakeShareable(new FPropertyChainHandle());
	PropertyChainHandle->Object = Object;
	PropertyChainHandle->BufferedPropertyContext = InBufferedPropertyContext;
	PropertyChainHandle->PropertyPath = PropertyPath;

	FProperty* TailProperty = nullptr;
	void* TailContainer = nullptr;
	void* TailValueAddress = nullptr;
	if (!PropertyChainHandle->Resolve(TailProperty, TailContainer, TailValueAddress))
	{
		return nullptr;
	}

	return PropertyChainHandle;
}

TSharedPtr<FPropertyChainHandle> FPropertyChainHandle::Create(UObject* Object, const FStringView& PropertyPathString)
{
	if (!Object)
	{
		return nullptr;
	}

	FPropertyPath PropertyPath;
	if (!FPropertyPath::TryParse(PropertyPathString, PropertyPath))
	{
		return nullptr;
	}

	return Create(Object, PropertyPath);
}

TSharedPtr<FPropertyChainHandle> FPropertyChainHandle::Create(UObject* Object, const FStringView& PropertyPathString,
	const FBufferedPropertyContext& InBufferedPropertyContext)
{
	if (!Object)
	{
		return nullptr;
	}

	FPropertyPath PropertyPath;
	if (!FPropertyPath::TryParse(PropertyPathString, PropertyPath))
	{
		return nullptr;
	}

	return Create(Object, PropertyPath, InBufferedPropertyContext);
}

FProperty* FPropertyChainHandle::GetTailProperty() const
{
	FProperty* TailProperty = nullptr;
	void* TailContainer = nullptr;
	void* TailValueAddress = nullptr;
	return Resolve(TailProperty, TailContainer, TailValueAddress) ? TailProperty : nullptr;
}

void* FPropertyChainHandle::GetTailContainer() const
{
	FProperty* TailProperty = nullptr;
	void* TailContainer = nullptr;
	void* TailValueAddress = nullptr;
	return Resolve(TailProperty, TailContainer, TailValueAddress) ? TailContainer : nullptr;
}

void* FPropertyChainHandle::GetTailValueAddress() const
{
	FProperty* TailProperty = nullptr;
	void* TailContainer = nullptr;
	void* TailValueAddress = nullptr;
	return Resolve(TailProperty, TailContainer, TailValueAddress) ? TailValueAddress : nullptr;
}

bool FPropertyChainHandle::IsArrayProperty() const
{
	FProperty* TailProperty = GetTailProperty();
	return TailProperty && CastField<FArrayProperty>(TailProperty) != nullptr;
}

bool FPropertyChainHandle::SetValue(const void* Data) const
{
	FProperty* TailProperty = nullptr;
	void* TailContainer = nullptr;
	void* TailValueAddress = nullptr;
	if (!Resolve(TailProperty, TailContainer, TailValueAddress))
	{
		return false;
	}

	if (TailValueAddress && TailProperty)
	{
		TailProperty->CopyCompleteValue(TailValueAddress, Data);
		return true;
	}
	return false;
}

bool FPropertyChainHandle::SetValue(const FStringView& ValueString) const
{
	auto TailProperty = GetTailProperty();
	if (!TailProperty)
	{
		return false;
	}

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
	if (ChildName.IsEmpty() || !Object.IsValid())
	{
		return nullptr;
	}
	const FPropertyPath ChildPath = PropertyPath.WithAppendedProperty(ChildName);
	return Create(Object.Get(), ChildPath, BufferedPropertyContext);
}

TSharedPtr<FPropertyChainHandle> FPropertyChainHandle::GetDirectHandle() const
{
	if (!Object.IsValid())
	{
		return nullptr;
	}

	return Create(Object.Get(), PropertyPath);
}

bool FPropertyChainHandle::Resolve(FProperty*& OutTailProperty, void*& OutTailContainer, void*& OutTailValueAddress) const
{
	OutTailProperty = nullptr;
	OutTailContainer = nullptr;
	OutTailValueAddress = nullptr;

	if (ResolveAgainstBufferedRoot(OutTailProperty, OutTailContainer, OutTailValueAddress))
	{
		return true;
	}
	if (ResolveAgainstObjectRoot(OutTailProperty, OutTailContainer, OutTailValueAddress))
	{
		return true;
	}
	return false;
}

bool FPropertyChainHandle::ResolveAgainstObjectRoot(FProperty*& OutTailProperty, void*& OutTailContainer, void*& OutTailValueAddress) const
{
	UObject* RootObject = Object.Get();
	if (!RootObject)
	{
		return false;
	}

	const auto InitialState = FPropertyPathResolver::FInitialState(RootObject, RootObject->GetClass());
	const auto ResolutionResult = FPropertyPathResolver::TryResolvePath(
		InitialState,
		PropertyPath);
	if (!ResolutionResult.IsValid())
	{
		return false;
	}

	OutTailProperty = ResolutionResult->Property;
	OutTailContainer = ResolutionResult->Container;
	OutTailValueAddress = ResolutionResult->ValueAddress;
	return true;
}

bool FPropertyChainHandle::ResolveAgainstBufferedRoot(FProperty*& OutTailProperty, void*& OutTailContainer, void*& OutTailValueAddress) const
{
	if (BufferedPropertyContext.InValid())
	{
		return false;
	}

	const TSharedPtr<const FPropertyBuffer> PropertyBuffer = BufferedPropertyContext.GetPropertyBuffer();
	if (!ensureMsgf(PropertyBuffer.IsValid(), TEXT("BufferedPropertyContext is valid but PropertyBuffer is invalid.")))
	{
		return false;
	}

	if (!ensureMsgf(!BufferedPropertyContext.GetRootPropertyPath().IsEmpty(), TEXT("BufferedPropertyContext is valid but RootPropertyPath is empty.")))
	{
		return false;
	}

	if (!ensureMsgf(PropertyBuffer->HasRootValue(), TEXT("BufferedPropertyContext is valid but PropertyBuffer has no root value.")))
	{
		return false;
	}

	FPropertyPath RelativePath;
	if (!PropertyPath.TryMakeRelativeTo(BufferedPropertyContext.GetRootPropertyPath(), RelativePath))
	{
		return false;
	}

	if (RelativePath.IsEmpty())
	{
		OutTailProperty = PropertyBuffer->GetRootProperty();
		OutTailContainer = PropertyBuffer->GetRootValueData();
		OutTailValueAddress = PropertyBuffer->GetRootValueData();
		return OutTailProperty != nullptr && OutTailValueAddress != nullptr;
	}

	const auto InitialState = FPropertyPathResolver::FInitialState(
		PropertyBuffer->GetRootValueData(),
		PropertyBuffer->GetRootStruct(),
		PropertyBuffer->GetRootProperty());
	const auto ResolutionResult = FPropertyPathResolver::TryResolvePath(
		InitialState,
		RelativePath);
	if (!ResolutionResult.IsValid())
	{
		return false;
	}

	OutTailProperty = ResolutionResult->Property;
	OutTailContainer = ResolutionResult->Container;
	OutTailValueAddress = ResolutionResult->ValueAddress;
	return true;
}
