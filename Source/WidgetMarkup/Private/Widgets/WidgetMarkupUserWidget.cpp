// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Widgets/WidgetMarkupUserWidget.h"
#include "Extensions/WidgetMarkupUserWidgetExtension.h"
#include "Components/IWidgetMarkupComponent.h"

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
