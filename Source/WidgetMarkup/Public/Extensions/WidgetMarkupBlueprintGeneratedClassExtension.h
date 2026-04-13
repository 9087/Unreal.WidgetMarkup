// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Extensions/WidgetBlueprintGeneratedClassExtension.h"
#include "Styles/WidgetStyleSheet.h"

#include "WidgetMarkupBlueprintGeneratedClassExtension.generated.h"

class UUserWidget;

UCLASS()
class WIDGETMARKUP_API UWidgetMarkupBlueprintGeneratedClassExtension : public UWidgetBlueprintGeneratedClassExtension
{
	GENERATED_BODY()

public:
	virtual void Initialize(UUserWidget* UserWidget) override;

	void SetStyleSheets(const TArray<FWidgetStyleSheetData>& InStyleSheets);

	const TArray<FWidgetStyleSheetData>& GetStyleSheets() const
	{
		return StyleSheets;
	}

private:
	UPROPERTY()
	TArray<FWidgetStyleSheetData> StyleSheets;
};
