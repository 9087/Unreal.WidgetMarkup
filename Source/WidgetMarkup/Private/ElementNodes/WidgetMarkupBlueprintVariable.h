// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "WidgetMarkupBlueprintVariable.generated.h"

USTRUCT()
struct FWidgetMarkupBlueprintVariable
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString Type;

	UPROPERTY()
	FString Default;
};
