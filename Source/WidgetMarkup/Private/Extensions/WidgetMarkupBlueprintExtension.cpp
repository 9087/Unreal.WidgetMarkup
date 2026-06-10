// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Extensions/WidgetMarkupBlueprintExtension.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Extensions/WidgetMarkupBlueprintGeneratedClassExtension.h"
#include "WidgetBlueprintCompiler.h"

void UWidgetMarkupBlueprintExtension::SetScript(const FString& InScript)
{
	Script = InScript;
}

void UWidgetMarkupBlueprintExtension::SetPropertyBindings(const TArray<FWidgetPropertyBinding>& InPropertyBindings)
{
	PropertyBindings = InPropertyBindings;
}

void UWidgetMarkupBlueprintExtension::AddDelegateBinding(const FWidgetDelegateBinding& InDelegateBinding)
{
	DelegateBindings.Add(InDelegateBinding);
}

void UWidgetMarkupBlueprintExtension::AddWidgetStyleAssignment(FName WidgetName, FName StyleName)
{
	WidgetStyleAssignments.Add(WidgetName, StyleName);
}

UWidgetStyleSheet* UWidgetMarkupBlueprintExtension::GetStyleSheet()
{
	if (!StyleSheet)
	{
		StyleSheet = NewObject<UWidgetStyleSheet>(this, NAME_None, RF_Transient | RF_Public);
	}
	return StyleSheet;
}

void UWidgetMarkupBlueprintExtension::ApplyToUserWidget(UUserWidget* UserWidget)
{
	if (!StyleSheet || !UserWidget) return;
	StyleSheet->ResolveComputedStyles();
	StyleSheet->ApplyToUserWidget(UserWidget);
}

void UWidgetMarkupBlueprintExtension::HandleBeginCompilation(FWidgetBlueprintCompilerContext& InCreationContext)
{
	Super::HandleBeginCompilation(InCreationContext);
	CurrentCompilerContext = &InCreationContext;
}

void UWidgetMarkupBlueprintExtension::HandleFinishCompilingClass(UWidgetBlueprintGeneratedClass* Class)
{
	Super::HandleFinishCompilingClass(Class);

	if (!Class || !CurrentCompilerContext)
	{
		return;
	}

	// Resolve the full Inherit chain into ComputedStyles.
	if (StyleSheet)
	{
		StyleSheet->ResolveComputedStyles();
	}

	UWidgetMarkupBlueprintGeneratedClassExtension* ClassExtension = Class->GetExtension<UWidgetMarkupBlueprintGeneratedClassExtension>(false);
	if (!ClassExtension)
	{
		ClassExtension = NewObject<UWidgetMarkupBlueprintGeneratedClassExtension>(Class);
		CurrentCompilerContext->AddExtension(Class, ClassExtension);
	}

	ClassExtension->SetStyleSheet(StyleSheet);
	ClassExtension->SetScript(Script);
	ClassExtension->SetPropertyBindings(PropertyBindings);
	ClassExtension->SetDelegateBindings(DelegateBindings);
	ClassExtension->SetWidgetStyleAssignments(WidgetStyleAssignments);
}

void UWidgetMarkupBlueprintExtension::HandleEndCompilation()
{
	Super::HandleEndCompilation();
	CurrentCompilerContext = nullptr;
}
