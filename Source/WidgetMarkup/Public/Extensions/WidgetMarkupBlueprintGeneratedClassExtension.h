// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Binding/WidgetDelegateBinding.h"
#include "Binding/WidgetPropertyBinding.h"
#include "Extensions/WidgetBlueprintGeneratedClassExtension.h"
#include "Styles/WidgetStyleSheet.h"

#include "WidgetMarkupBlueprintGeneratedClassExtension.generated.h"

class UUserWidget;

UCLASS(BlueprintType)
class WIDGETMARKUP_API UWidgetMarkupBlueprintGeneratedClassExtension : public UWidgetBlueprintGeneratedClassExtension
{
	GENERATED_BODY()

public:
	virtual void Initialize(UUserWidget* UserWidget) override;

	UFUNCTION(BlueprintPure, Category = "WidgetMarkup")
	static UWidgetMarkupBlueprintGeneratedClassExtension* GetWidgetMarkupExtension(UUserWidget* UserWidget);

	UFUNCTION(BlueprintPure, Category = "WidgetMarkup")
	TArray<FWidgetPropertyBinding> GetPropertyBindings() const;

	UFUNCTION(BlueprintPure, Category = "WidgetMarkup")
	TArray<FWidgetDelegateBinding> GetDelegateBindings() const;

	void SetStyleSheets(const TArray<FWidgetStyleSheetData>& InStyleSheets);
	void SetScript(const FString& InScript);
	void SetPropertyBindings(const TArray<FWidgetPropertyBinding>& InPropertyBindings);
	void SetDelegateBindings(const TArray<FWidgetDelegateBinding>& InDelegateBindings);

	const TArray<FWidgetStyleSheetData>& GetStyleSheets() const
	{
		return StyleSheets;
	}

	const FString& GetScript() const
	{
		return Script;
	}

	const TArray<FWidgetPropertyBinding>& GetPropertyBindingsRef() const
	{
		return PropertyBindings;
	}

private:
	UPROPERTY()
	TArray<FWidgetStyleSheetData> StyleSheets;

	UPROPERTY()
	FString Script;

	UPROPERTY()
	TArray<FWidgetPropertyBinding> PropertyBindings;

	UPROPERTY()
	TArray<FWidgetDelegateBinding> DelegateBindings;
};
