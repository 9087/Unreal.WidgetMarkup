// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "WidgetMarkupPythonIntegration.h"

#include "Modules/ModuleManager.h"
#include "WidgetMarkupModule.h"

DEFINE_LOG_CATEGORY(LogWidgetMarkupPythonIntegration);

class FWidgetMarkupPythonIntegrationModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		FWidgetMarkupModule::Get().StartUp<FWidgetMarkupPythonIntegration>();
	}

	virtual void ShutdownModule() override
	{
		FWidgetMarkupModule::Get().Shutdown();
	}
};

IMPLEMENT_MODULE(FWidgetMarkupPythonIntegrationModule, WidgetMarkupPythonIntegration)