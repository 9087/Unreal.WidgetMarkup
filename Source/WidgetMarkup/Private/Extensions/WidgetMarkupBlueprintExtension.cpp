// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Extensions/WidgetMarkupBlueprintExtension.h"

#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Extensions/WidgetMarkupBlueprintGeneratedClassExtension.h"
#include "WidgetBlueprintCompiler.h"

void UWidgetMarkupBlueprintExtension::SetStyleSheets(const TArray<FWidgetStyleSheetData>& InStyleSheets)
{
	StyleSheets = InStyleSheets;
}

void UWidgetMarkupBlueprintExtension::SetScript(const FString& InScript)
{
	Script = InScript;
}

FWidgetStyleSheetData& UWidgetMarkupBlueprintExtension::GetOrAddDefaultStyleSheet()
{
	if (StyleSheets.Num() == 0)
	{
		StyleSheets.AddDefaulted(1);
	}
	return StyleSheets[0];
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

	if (UWidgetMarkupBlueprintGeneratedClassExtension* ExistingExtension = Class->GetExtension<UWidgetMarkupBlueprintGeneratedClassExtension>(false))
	{
		ExistingExtension->SetStyleSheets(StyleSheets);
		ExistingExtension->SetScript(Script);
		return;
	}

	UWidgetMarkupBlueprintGeneratedClassExtension* ClassExtension = NewObject<UWidgetMarkupBlueprintGeneratedClassExtension>(Class);
	ClassExtension->SetStyleSheets(StyleSheets);
	ClassExtension->SetScript(Script);
	CurrentCompilerContext->AddExtension(Class, ClassExtension);
}

void UWidgetMarkupBlueprintExtension::HandleEndCompilation()
{
	Super::HandleEndCompilation();
	CurrentCompilerContext = nullptr;
}
