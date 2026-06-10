// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PropertyBuffer.h"
#include "Utilities/WidgetPropertyPath.h"

#include "WidgetStyleSheet.generated.h"

class UUserWidget;
class UWidget;

USTRUCT()
struct FWidgetStyleSetter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Style")
	FWidgetPropertyPath Property;

	UPROPERTY(EditAnywhere, Category = "Style")
	FPropertyBuffer Value;

	/** Raw value string for deferred property resolution. */
	UPROPERTY(EditAnywhere, Category = "Style")
	FString RawValue;

	bool ApplyToWidget(UWidget* Widget) const;
};

USTRUCT()
struct FWidgetStyleEntry
{
	GENERATED_BODY()

	/** Target widget class name. "*" matches all widgets of TargetType (implicit style). */
	UPROPERTY(EditAnywhere, Category = "Style")
	FName TargetType = NAME_None;

	/** Explicit style name (matched via Style="Name" attribute). NAME_None for implicit. */
	UPROPERTY(EditAnywhere, Category = "Style")
	FName Name = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Style")
	TArray<FWidgetStyleSetter> Setters;
};

UCLASS(BlueprintType)
class WIDGETMARKUP_API UWidgetStyleSheet : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Base stylesheet this one inherits from (resolved at compile time). */
	UPROPERTY(EditAnywhere, Instanced, Category = "Style")
	TObjectPtr<UWidgetStyleSheet> Inherit;

	UPROPERTY(EditAnywhere, Category = "Style")
	TArray<FWidgetStyleEntry> Styles;

	/** Final merged styles (base + overrides, computed by ResolveComputedStyles). */
	UPROPERTY(EditAnywhere, Transient, Category = "Style")
	TArray<FWidgetStyleEntry> ComputedStyles;

	void AddOrReplaceStyleEntry(const FWidgetStyleEntry& Entry);

	/** Walk Inherit chain and compute ComputedStyles (base Inherits first, then local Styles override). */
	void ResolveComputedStyles();

	/** Apply ComputedStyles to the given UserWidget using the provided style assignments. */
	void ApplyToUserWidget(UUserWidget* UserWidget) const;
};
