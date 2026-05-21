// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FWidgetMarkupModule;

class WIDGETMARKUP_API IWidgetMarkupScriptIntegration : public TSharedFromThis<IWidgetMarkupScriptIntegration>
{
protected:
	explicit IWidgetMarkupScriptIntegration(FWidgetMarkupModule& InWidgetMarkupModule);

public:
	void Initialize(bool bOK);

protected:
	FWidgetMarkupModule& WidgetMarkupModule;
};