// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "Utilities/WidgetPropertyPath.h"

class FProperty;
class UStruct;

struct FPropertyPathResolver
{
public:
	struct FInitialState
	{
		FInitialState(void* InContainer, UStruct* InStruct, FProperty* InProperty = nullptr)
			: Property(InProperty)
			, Container(InContainer)
			, Struct(InStruct)
		{
		}

		FProperty* Property;
		void* Container;
		UStruct* Struct;
	};

	struct FOutput
	{
		FProperty* Property = nullptr;
		void* Container = nullptr;
		void* ValueAddress = nullptr;
	};

	static TSharedPtr<FOutput> TryResolvePath(const FInitialState& InitialState, const FWidgetPropertyPath& Path);
};
