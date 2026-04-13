// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Extensions/WidgetMarkupUserWidgetExtension.h"

#include "Blueprint/UserWidget.h"
#include "Styles/WidgetStyleSheet.h"
#include "WidgetMarkupModule.h"

void UWidgetMarkupUserWidgetExtension::Initialize()
{
	Super::Initialize();
	ApplyStyleSheets();
}

void UWidgetMarkupUserWidgetExtension::SetStyleSheets(const TArray<FWidgetStyleSheetData>& InStyleSheets)
{
	StyleSheets = InStyleSheets;
}

void UWidgetMarkupUserWidgetExtension::ApplyStyleSheets()
{
	UUserWidget* UserWidget = GetUserWidget();
	if (!UserWidget)
	{
		UE_LOG(LogWidgetMarkup, Warning, TEXT("WidgetMarkup Style Extension: failed to apply style sheets because UserWidget is null."));
		return;
	}

	for (int32 StyleSheetIndex = 0; StyleSheetIndex < StyleSheets.Num(); ++StyleSheetIndex)
	{
		if (!StyleSheets[StyleSheetIndex].ApplyToUserWidget(UserWidget))
		{
			UE_LOG(LogWidgetMarkup, Warning, TEXT("WidgetMarkup Style Extension: style sheet [%d] contains one or more failed style applications."), StyleSheetIndex);
		}
	}
}
