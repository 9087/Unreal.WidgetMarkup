// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Extensions/WidgetMarkupBlueprintGeneratedClassExtension.h"

#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/UserWidget.h"
#include "Extensions/WidgetMarkupUserWidgetExtension.h"

void UWidgetMarkupBlueprintGeneratedClassExtension::Initialize(UUserWidget* UserWidget)
{
	if (!UserWidget)
	{
		return;
	}

	UWidgetMarkupUserWidgetExtension::GetOrAddExtension(UserWidget);
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

void UWidgetMarkupBlueprintGeneratedClassExtension::SetStyleSheet(UWidgetStyleSheet* InStyleSheet)
{
	StyleSheet = InStyleSheet;
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

void UWidgetMarkupBlueprintGeneratedClassExtension::SetWidgetStyleAssignments(const TMap<FName, FName>& InAssignments)
{
	WidgetStyleAssignments = InAssignments;
}
