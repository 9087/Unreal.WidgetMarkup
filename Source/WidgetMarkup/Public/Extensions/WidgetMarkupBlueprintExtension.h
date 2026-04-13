// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styles/WidgetStyleSheet.h"
#include "WidgetBlueprintExtension.h"

#include "WidgetMarkupBlueprintExtension.generated.h"

class FWidgetBlueprintCompilerContext;
class UWidgetBlueprintGeneratedClass;

UCLASS()
class WIDGETMARKUP_API UWidgetMarkupBlueprintExtension : public UWidgetBlueprintExtension
{
	GENERATED_BODY()

public:
	void SetStyleSheets(const TArray<FWidgetStyleSheetData>& InStyleSheets);

	const TArray<FWidgetStyleSheetData>& GetStyleSheets() const
	{
		return StyleSheets;
	}

protected:
	virtual void HandleBeginCompilation(FWidgetBlueprintCompilerContext& InCreationContext) override;
	virtual void HandleFinishCompilingClass(UWidgetBlueprintGeneratedClass* Class) override;
	virtual void HandleEndCompilation() override;

private:
	UPROPERTY(EditAnywhere, Category = "Widget Markup")
	TArray<FWidgetStyleSheetData> StyleSheets;

	FWidgetBlueprintCompilerContext* CurrentCompilerContext = nullptr;
};
