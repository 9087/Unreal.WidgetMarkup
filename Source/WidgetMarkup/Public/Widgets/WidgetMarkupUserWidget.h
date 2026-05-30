// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "WidgetMarkupUserWidget.generated.h"

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
};
