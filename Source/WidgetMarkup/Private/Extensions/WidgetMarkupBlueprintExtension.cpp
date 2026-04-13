// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Extensions/WidgetMarkupBlueprintExtension.h"

#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Extensions/WidgetMarkupBlueprintGeneratedClassExtension.h"
#include "WidgetBlueprintCompiler.h"

void UWidgetMarkupBlueprintExtension::SetStyleSheets(const TArray<FWidgetStyleSheetData>& InStyleSheets)
{
	StyleSheets = InStyleSheets;
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
		return;
	}

	UWidgetMarkupBlueprintGeneratedClassExtension* ClassExtension = NewObject<UWidgetMarkupBlueprintGeneratedClassExtension>(Class);
	ClassExtension->SetStyleSheets(StyleSheets);
	CurrentCompilerContext->AddExtension(Class, ClassExtension);
}

void UWidgetMarkupBlueprintExtension::HandleEndCompilation()
{
	Super::HandleEndCompilation();
	CurrentCompilerContext = nullptr;
}
