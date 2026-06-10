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

	void SetStyleSheet(UWidgetStyleSheet* InStyleSheet);
	void SetScript(const FString& InScript);
	void SetPropertyBindings(const TArray<FWidgetPropertyBinding>& InPropertyBindings);
	void SetDelegateBindings(const TArray<FWidgetDelegateBinding>& InDelegateBindings);
	void SetWidgetStyleAssignments(const TMap<FName, FName>& InAssignments);
	const TMap<FName, FName>& GetWidgetStyleAssignments() const { return WidgetStyleAssignments; }

	const TObjectPtr<UWidgetStyleSheet>& GetStyleSheet() const
	{
		return StyleSheet;
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
	TObjectPtr<UWidgetStyleSheet> StyleSheet;

	UPROPERTY()
	FString Script;

	UPROPERTY()
	TArray<FWidgetPropertyBinding> PropertyBindings;

	UPROPERTY()
	TArray<FWidgetDelegateBinding> DelegateBindings;

	UPROPERTY()
	TMap<FName, FName> WidgetStyleAssignments;
};
