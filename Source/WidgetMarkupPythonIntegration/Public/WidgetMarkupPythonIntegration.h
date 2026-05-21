// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetMarkupScriptIntegration.h"

WIDGETMARKUPPYTHONINTEGRATION_API DECLARE_LOG_CATEGORY_EXTERN(LogWidgetMarkupPythonIntegration, Log, All);

class IPythonScriptPlugin;
class UUserWidget;
class UWidgetMarkupBlueprintGeneratedClassExtension;
class FWidgetMarkupPythonIntegrationModule;

class WIDGETMARKUPPYTHONINTEGRATION_API FWidgetMarkupPythonIntegration : public IWidgetMarkupScriptIntegration
{
public:
	explicit FWidgetMarkupPythonIntegration(FWidgetMarkupModule& InWidgetMarkupModule);

protected:
	void HandlePythonInitialized();
	void HandleWidgetMarkupUserWidgetInitialized(UUserWidget* UserWidget, UWidgetMarkupBlueprintGeneratedClassExtension* GeneratedClassExtension);

	IPythonScriptPlugin* PythonScriptPlugin = nullptr;
};
