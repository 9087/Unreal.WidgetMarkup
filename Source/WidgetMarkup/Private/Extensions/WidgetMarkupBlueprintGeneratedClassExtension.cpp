// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Extensions/WidgetMarkupBlueprintGeneratedClassExtension.h"

#include "Blueprint/UserWidget.h"
#include "Extensions/WidgetMarkupUserWidgetExtension.h"
#include "WidgetMarkupModule.h"
#include "Modules/ModuleManager.h"

void UWidgetMarkupBlueprintGeneratedClassExtension::Initialize(UUserWidget* UserWidget)
{
	if (!UserWidget)
	{
		return;
	}

	UWidgetMarkupUserWidgetExtension* UserWidgetExtension = UserWidget->GetExtension<UWidgetMarkupUserWidgetExtension>();
	if (!UserWidgetExtension)
	{
		UserWidgetExtension = UserWidget->AddExtension<UWidgetMarkupUserWidgetExtension>();
	}

	if (UserWidgetExtension)
	{
		UserWidgetExtension->SetStyleSheets(StyleSheets);
	}

	if (FWidgetMarkupModule* WidgetMarkupModule = FModuleManager::GetModulePtr<FWidgetMarkupModule>(TEXT("WidgetMarkup")))
	{
		WidgetMarkupModule->GetOnWidgetMarkupUserWidgetInitialized().Broadcast(UserWidget, this);
	}
}

void UWidgetMarkupBlueprintGeneratedClassExtension::SetStyleSheets(const TArray<FWidgetStyleSheetData>& InStyleSheets)
{
	StyleSheets = InStyleSheets;
}

void UWidgetMarkupBlueprintGeneratedClassExtension::SetScript(const FString& InScript)
{
	Script = InScript;
}
