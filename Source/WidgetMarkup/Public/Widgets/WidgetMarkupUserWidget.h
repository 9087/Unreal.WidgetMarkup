// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "WidgetMarkupUserWidget.generated.h"

class UListViewBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWidgetMarkupEntryReleased);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetMarkupItemSelectionChanged, bool, bIsSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetMarkupItemExpansionChanged, bool, bIsExpanded);

/**
 * Base class for WidgetMarkup ListView entry widgets.
 * Bridges the ListView entry creation to WidgetMarkupComponent.on_data_refresh.
 */
UCLASS()
class WIDGETMARKUP_API UWidgetMarkupUserWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnEntryReleased() override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	virtual void NativeOnItemExpansionChanged(bool bIsExpanded) override;

	/** Get the raw list item object assigned to this entry. */
	UFUNCTION(BlueprintCallable, Category = "WidgetMarkup")
	UObject* GetEntryListItem() const;

	/** Check if this entry is currently selected in its owning list view. */
	UFUNCTION(BlueprintCallable, Category = "WidgetMarkup")
	bool IsEntrySelected() const;

	/** Get the owning ListView for this entry widget. */
	UFUNCTION(BlueprintCallable, Category = "WidgetMarkup")
	UListViewBase* GetOwningEntryListView() const;

	/** Broadcast when this entry is released from the owning list view. */
	UPROPERTY(BlueprintAssignable, Category = "WidgetMarkup")
	FOnWidgetMarkupEntryReleased OnEntryReleased;

	/** Broadcast when the selection state of this entry changes. */
	UPROPERTY(BlueprintAssignable, Category = "WidgetMarkup")
	FOnWidgetMarkupItemSelectionChanged OnItemSelectionChanged;

	/** Broadcast when the expansion state of this entry changes (TreeView only). */
	UPROPERTY(BlueprintAssignable, Category = "WidgetMarkup")
	FOnWidgetMarkupItemExpansionChanged OnItemExpansionChanged;
};
