// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Extensions/UserWidgetExtension.h"
#include "Styles/WidgetStyleSheet.h"

#include "WidgetMarkupUserWidgetExtension.generated.h"

class IWidgetMarkupComponent;
class UUserWidget;

UCLASS(Transient)
class WIDGETMARKUP_API UWidgetMarkupUserWidgetExtension : public UUserWidgetExtension
{
	GENERATED_BODY()

public:
	static UWidgetMarkupUserWidgetExtension* GetOrAddExtension(UUserWidget* UserWidget);

	virtual void Initialize() override;

	void SetStyleSheets(const TArray<FWidgetStyleSheetData>& InStyleSheets);
	void SetWidgetMarkupComponent(TSharedPtr<IWidgetMarkupComponent> InWidgetMarkupComponent);

	const TSharedPtr<IWidgetMarkupComponent>& GetWidgetMarkupComponent() const
	{
		return WidgetMarkupComponent;
	}

private:
	void ApplyStyleSheets();

	UPROPERTY(Transient)
	TArray<FWidgetStyleSheetData> StyleSheets;

	TSharedPtr<IWidgetMarkupComponent> WidgetMarkupComponent;
};
