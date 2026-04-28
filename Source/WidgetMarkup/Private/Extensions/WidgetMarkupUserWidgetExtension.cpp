// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Extensions/WidgetMarkupUserWidgetExtension.h"

#include "Blueprint/UserWidget.h"
#include "Components/IWidgetMarkupComponent.h"
#include "Styles/WidgetStyleSheet.h"
#include "WidgetMarkupModule.h"

UWidgetMarkupUserWidgetExtension* UWidgetMarkupUserWidgetExtension::GetOrAddExtension(UUserWidget* UserWidget)
{
	if (!UserWidget)
	{
		return nullptr;
	}

	if (UWidgetMarkupUserWidgetExtension* ExistingExtension = UserWidget->GetExtension<UWidgetMarkupUserWidgetExtension>())
	{
		return ExistingExtension;
	}

	return UserWidget->AddExtension<UWidgetMarkupUserWidgetExtension>();
}

void UWidgetMarkupUserWidgetExtension::Initialize()
{
	Super::Initialize();
	ApplyStyleSheets();
}

void UWidgetMarkupUserWidgetExtension::SetStyleSheets(const TArray<FWidgetStyleSheetData>& InStyleSheets)
{
	StyleSheets = InStyleSheets;
}

void UWidgetMarkupUserWidgetExtension::SetWidgetMarkupComponent(TSharedPtr<IWidgetMarkupComponent> InWidgetMarkupComponent)
{
	WidgetMarkupComponent = MoveTemp(InWidgetMarkupComponent);
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
