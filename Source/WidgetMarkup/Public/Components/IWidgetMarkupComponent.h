// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class WIDGETMARKUP_API IWidgetMarkupComponent
{
public:
	virtual ~IWidgetMarkupComponent() = default;

	/** Refresh the component's display with the given data object. */
	virtual void OnDataRefresh(class UObject* Data) {}
};
