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

	UWidgetMarkupUserWidgetExtension::GetOrAddExtension(UserWidget)->SetStyleSheets(StyleSheets);

	FWidgetMarkupModule& WidgetMarkupModule = FModuleManager::GetModuleChecked<FWidgetMarkupModule>(TEXT("WidgetMarkup"));
	WidgetMarkupModule.GetOnWidgetMarkupUserWidgetInitialized().Broadcast(UserWidget, this);
}

void UWidgetMarkupBlueprintGeneratedClassExtension::SetStyleSheets(const TArray<FWidgetStyleSheetData>& InStyleSheets)
{
	StyleSheets = InStyleSheets;
}

void UWidgetMarkupBlueprintGeneratedClassExtension::SetScript(const FString& InScript)
{
	Script = InScript;
}
