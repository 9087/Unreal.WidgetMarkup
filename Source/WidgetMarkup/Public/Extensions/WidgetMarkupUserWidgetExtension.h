// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Extensions/UserWidgetExtension.h"
#include "Styles/WidgetStyleSheet.h"

#include "WidgetMarkupUserWidgetExtension.generated.h"

UCLASS(Transient)
class WIDGETMARKUP_API UWidgetMarkupUserWidgetExtension : public UUserWidgetExtension
{
	GENERATED_BODY()

public:
	virtual void Initialize() override;

	void SetStyleSheets(const TArray<FWidgetStyleSheetData>& InStyleSheets);

private:
	void ApplyStyleSheets();

	UPROPERTY(Transient)
	TArray<FWidgetStyleSheetData> StyleSheets;
};
