// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "WidgetMarkupPythonIntegration.h"

#include "Blueprint/UserWidget.h"
#include "Extensions/WidgetMarkupBlueprintGeneratedClassExtension.h"
#include "Extensions/WidgetMarkupUserWidgetExtension.h"
#include "IPythonScriptPlugin.h"
#include "PythonWidgetMarkupComponent.h"
#include "PythonWidgetMarkupNativeModule.h"
#include "WidgetMarkupModule.h"

FWidgetMarkupPythonIntegration::FWidgetMarkupPythonIntegration(FWidgetMarkupModule& InWidgetMarkupModule)
	: IWidgetMarkupScriptIntegration(InWidgetMarkupModule)
{
	PythonScriptPlugin = IPythonScriptPlugin::Get();
	if (!PythonScriptPlugin)
	{
		Initialize(false);
		return;
	}

	WidgetMarkupModule.GetOnWidgetMarkupUserWidgetInitialized().AddRaw(this, &FWidgetMarkupPythonIntegration::HandleWidgetMarkupUserWidgetInitialized);

	if (PythonScriptPlugin->IsPythonAvailable())
	{
		HandlePythonInitialized();
	}
  else
  {
    PythonScriptPlugin->OnPythonInitialized().AddRaw(this, &FWidgetMarkupPythonIntegration::HandlePythonInitialized);
  }
}

void FWidgetMarkupPythonIntegration::HandlePythonInitialized()
{
	Initialize(true);
}

void FWidgetMarkupPythonIntegration::HandleWidgetMarkupUserWidgetInitialized(UUserWidget* UserWidget, UWidgetMarkupBlueprintGeneratedClassExtension* GeneratedClassExtension)
{
	if (!UserWidget || !GeneratedClassExtension)
	{
		UE_LOG(LogWidgetMarkupPythonIntegration, Warning, TEXT("WidgetMarkupPythonIntegration: HandleWidgetMarkupUserWidgetInitialized called with null UserWidget or Extension."));
		return;
	}

	const FString Script = GeneratedClassExtension->GetScript().TrimStartAndEnd();
	if (Script.IsEmpty())
	{
		UE_LOG(LogWidgetMarkupPythonIntegration, Display, TEXT("WidgetMarkupPythonIntegration: skipped component creation because Script is empty for UserWidget '%s'."), *GetNameSafe(UserWidget));
		return;
	}

	UE_LOG(LogWidgetMarkupPythonIntegration, Display, TEXT("WidgetMarkupPythonIntegration: creating Python component for Script '%s' on UserWidget '%s'."), *Script, *GetNameSafe(UserWidget));

	RegisterWidgetMarkupPythonIntegrationNativeModule();
	UWidgetMarkupUserWidgetExtension::GetOrAddExtension(UserWidget)->SetWidgetMarkupComponent(FPythonWidgetMarkupComponent::Create(UserWidget, Script));
}
