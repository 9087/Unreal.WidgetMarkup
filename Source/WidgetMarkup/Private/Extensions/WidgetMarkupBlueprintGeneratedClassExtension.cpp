// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Extensions/WidgetMarkupBlueprintGeneratedClassExtension.h"

#include "Blueprint/WidgetBlueprintGeneratedClass.h"
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

UWidgetMarkupBlueprintGeneratedClassExtension* UWidgetMarkupBlueprintGeneratedClassExtension::GetWidgetMarkupExtension(UUserWidget* UserWidget)
{
	if (!UserWidget)
	{
		return nullptr;
	}

	UWidgetBlueprintGeneratedClass* WidgetClass = Cast<UWidgetBlueprintGeneratedClass>(UserWidget->GetClass());
	return WidgetClass ? WidgetClass->GetExtension<UWidgetMarkupBlueprintGeneratedClassExtension>() : nullptr;
}

TArray<FWidgetPropertyBinding> UWidgetMarkupBlueprintGeneratedClassExtension::GetPropertyBindings() const
{
	return PropertyBindings;
}

void UWidgetMarkupBlueprintGeneratedClassExtension::SetStyleSheets(const TArray<FWidgetStyleSheetData>& InStyleSheets)
{
	StyleSheets = InStyleSheets;
}

void UWidgetMarkupBlueprintGeneratedClassExtension::SetScript(const FString& InScript)
{
	Script = InScript;
}

void UWidgetMarkupBlueprintGeneratedClassExtension::SetPropertyBindings(const TArray<FWidgetPropertyBinding>& InPropertyBindings)
{
	PropertyBindings = InPropertyBindings;
}

TArray<FWidgetDelegateBinding> UWidgetMarkupBlueprintGeneratedClassExtension::GetDelegateBindings() const
{
	return DelegateBindings;
}

void UWidgetMarkupBlueprintGeneratedClassExtension::SetDelegateBindings(const TArray<FWidgetDelegateBinding>& InDelegateBindings)
{
	DelegateBindings = InDelegateBindings;
}
