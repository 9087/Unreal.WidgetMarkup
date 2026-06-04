// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Widgets/WidgetMarkupUserWidget.h"
#include "Extensions/WidgetMarkupUserWidgetExtension.h"
#include "Components/IWidgetMarkupComponent.h"
#include "Components/ListViewBase.h"

void UWidgetMarkupUserWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	// Find THIS entry widget's own WidgetMarkupComponent and notify it.
	if (UWidgetMarkupUserWidgetExtension* Extension = UWidgetMarkupUserWidgetExtension::GetOrAddExtension(this))
	{
		if (const TSharedPtr<IWidgetMarkupComponent>& Component = Extension->GetWidgetMarkupComponent())
		{
			Component->OnDataRefresh(ListItemObject);
		}
	}
}

void UWidgetMarkupUserWidget::NativeOnEntryReleased()
{
	IUserListEntry::NativeOnEntryReleased();
	OnEntryReleased.Broadcast();
}

void UWidgetMarkupUserWidget::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserListEntry::NativeOnItemSelectionChanged(bIsSelected);
	OnItemSelectionChanged.Broadcast(bIsSelected);
}

void UWidgetMarkupUserWidget::NativeOnItemExpansionChanged(bool bIsExpanded)
{
	IUserListEntry::NativeOnItemExpansionChanged(bIsExpanded);
	OnItemExpansionChanged.Broadcast(bIsExpanded);
}

UObject* UWidgetMarkupUserWidget::GetEntryListItem() const
{
	return GetListItem<UObject>();
}

bool UWidgetMarkupUserWidget::IsEntrySelected() const
{
	return IsListItemSelected();
}

UListViewBase* UWidgetMarkupUserWidget::GetOwningEntryListView() const
{
	return GetOwningListView();
}
