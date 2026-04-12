// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ElementNodes/PropertyBuffer.h"
#include "Utilities/WidgetPropertyPath.h"

#include "WidgetStyleSheet.generated.h"

class UUserWidget;

USTRUCT()
struct FWidgetStyleEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Style")
	FName WidgetName = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Style")
	FWidgetPropertyPath PropertyPath;

	UPROPERTY(EditAnywhere, Category = "Style")
	FPropertyBuffer PropertyValue;

	bool CanApply(FString* OutErrorMessage = nullptr) const;
};

USTRUCT(BlueprintType)
struct FWidgetStyleSheetApplyResult
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Style")
	int32 AppliedCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Style")
	int32 FailedCount = 0;
};

USTRUCT()
struct FWidgetStyleSheetData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Style")
	TArray<FWidgetStyleEntry> Styles;
};

UCLASS(BlueprintType)
class WIDGETMARKUP_API UWidgetStyleSheet : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Style")
	FWidgetStyleSheetData StyleSheet;

	UFUNCTION(BlueprintCallable, Category = "Widget Markup|Style")
	FWidgetStyleSheetApplyResult ApplyToUserWidget(UUserWidget* UserWidget) const;
};
