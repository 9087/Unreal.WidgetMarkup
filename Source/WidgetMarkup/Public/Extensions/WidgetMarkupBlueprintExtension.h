// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Binding/WidgetDelegateBinding.h"
#include "Binding/WidgetPropertyBinding.h"
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
	/** Get the StyleSheet (created on first access). Used for ListItems etc. */
	UWidgetStyleSheet* GetStyleSheet();

	/** Apply the resolved ComputedStyles to the given UserWidget. */
	void ApplyToUserWidget(UUserWidget* UserWidget);

	void SetScript(const FString& InScript);
	void SetPropertyBindings(const TArray<FWidgetPropertyBinding>& InPropertyBindings);
	void AddDelegateBinding(const FWidgetDelegateBinding& InDelegateBinding);

	const FString& GetScript() const
	{
		return Script;
	}

	const TArray<FWidgetPropertyBinding>& GetPropertyBindings() const
	{
		return PropertyBindings;
	}

	const TArray<FWidgetDelegateBinding>& GetDelegateBindings() const
	{
		return DelegateBindings;
	}

	/** Record a Style="Name" assignment from widget XML (WidgetName → StyleName). */
	void AddWidgetStyleAssignment(FName WidgetName, FName StyleName);
	const TMap<FName, FName>& GetWidgetStyleAssignments() const { return WidgetStyleAssignments; }

protected:
	virtual void HandleBeginCompilation(FWidgetBlueprintCompilerContext& InCreationContext) override;
	virtual void HandleFinishCompilingClass(UWidgetBlueprintGeneratedClass* Class) override;
	virtual void HandleEndCompilation() override;

private:
	/** Stylesheet: holds ListItems etc. Its Inherit is set from WidgetBlueprint's <StyleSheet>. */
	UPROPERTY(EditAnywhere, Instanced, Category = "Widget Markup")
	TObjectPtr<UWidgetStyleSheet> StyleSheet;

	UPROPERTY(EditAnywhere, Category = "Widget Markup")
	FString Script;

	UPROPERTY()
	TArray<FWidgetPropertyBinding> PropertyBindings;

	UPROPERTY()
	TArray<FWidgetDelegateBinding> DelegateBindings;

	/** Widget Style assignments: WidgetName -> StyleName (from Style="..." attribute). */
	UPROPERTY()
	TMap<FName, FName> WidgetStyleAssignments;

	FWidgetBlueprintCompilerContext* CurrentCompilerContext = nullptr;
};
