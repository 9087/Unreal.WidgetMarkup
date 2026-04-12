// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyPathResolver.h"

#include "Templates/SharedPointer.h"
#include "UObject/UnrealType.h"

TSharedPtr<FPropertyPathResolver::FOutput> FPropertyPathResolver::TryResolvePath(const FInitialState& InitialState, const FWidgetPropertyPath& Path)
{
	if (!InitialState.Container || Path.IsEmpty())
	{
		return nullptr;
	}

	const TArray<FWidgetPropertyPathElement>& Elements = Path.GetElements();
	if (Elements.IsEmpty())
	{
		return nullptr;
	}

	FProperty* CurrentProperty = InitialState.Property;
	void* CurrentContainer = InitialState.Container;
	UStruct* CurrentStruct = InitialState.Struct;

	for (int32 ElementIndex = 0; ElementIndex < Elements.Num(); ++ElementIndex)
	{
		const FWidgetPropertyPathElement& Element = Elements[ElementIndex];

		if (Element.bIsAny)
		{
			return nullptr;
		}

		void* ValueAddress = nullptr;
		if (Element.Type == EWidgetPropertyPathElementType::Property)
		{
			CurrentProperty = CurrentStruct ? CurrentStruct->FindPropertyByName(FName(*Element.Name)) : nullptr;
			if (!CurrentProperty)
			{
				return nullptr;
			}
		}
		else if (Element.Type == EWidgetPropertyPathElementType::ArrayIndex)
		{
			FArrayProperty* ArrayProperty = CastField<FArrayProperty>(CurrentProperty);
			if (!ArrayProperty)
			{
				return nullptr;
			}

			FScriptArrayHelper ArrayHelper(ArrayProperty, CurrentContainer);
			if (!ArrayHelper.IsValidIndex(Element.ArrayIndex))
			{
				return nullptr;
			}

			void* ElementValueAddress = ArrayHelper.GetRawPtr(Element.ArrayIndex);
			FProperty* InnerProperty = ArrayProperty->Inner;
			if (!InnerProperty)
			{
				return nullptr;
			}

			CurrentProperty = InnerProperty;
			ValueAddress = ElementValueAddress;
			CurrentContainer = ElementValueAddress;
		}
		else
		{
			return nullptr;
		}

		if (!ValueAddress)
		{
			ValueAddress = CurrentProperty->ContainerPtrToValuePtr<void>(CurrentContainer);
		}

		const bool bIsLastElement = (ElementIndex == Elements.Num() - 1);
		if (bIsLastElement)
		{
			if (!CurrentProperty || !CurrentContainer || !ValueAddress)
			{
				return nullptr;
			}

			TSharedPtr<FOutput> Output = MakeShared<FOutput>();
			Output->Property = CurrentProperty;
			Output->Container = CurrentContainer;
			Output->ValueAddress = ValueAddress;
			return Output;
		}

		if (FStructProperty* StructProperty = CastField<FStructProperty>(CurrentProperty))
		{
			CurrentContainer = ValueAddress;
			CurrentStruct = StructProperty->Struct;
		}
		else if (FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(CurrentProperty))
		{
			UObject* ObjectValue = ObjectProperty->GetObjectPropertyValue(ValueAddress);
			if (!ObjectValue)
			{
				return nullptr;
			}

			CurrentContainer = ObjectValue;
			CurrentStruct = ObjectValue->GetClass();
		}
		else
		{
			CurrentContainer = ValueAddress;
			CurrentStruct = nullptr;
		}
	}

	return nullptr;
}
