// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Extensions/WidgetMarkupUserWidgetExtension.h"

#include "Blueprint/UserWidget.h"
#include "Components/IWidgetMarkupComponent.h"
#include "Extensions/WidgetMarkupBlueprintGeneratedClassExtension.h"
#include "Modules/ModuleManager.h"
#include "Styles/WidgetStyleSheet.h"
#include "WidgetMarkupModule.h"

UWidgetMarkupUserWidgetExtension* UWidgetMarkupUserWidgetExtension::GetOrAddExtension(UUserWidget* UserWidget)
{
	if (!UserWidget)
	{
		return nullptr;
	}

	if (UWidgetMarkupUserWidgetExtension* ExistingExtension = UserWidget->GetExtension<UWidgetMarkupUserWidgetExtension>())
	{
		return ExistingExtension;
	}

	UWidgetMarkupUserWidgetExtension* Extension = UserWidget->AddExtension<UWidgetMarkupUserWidgetExtension>();
	return Extension;
}

void UWidgetMarkupUserWidgetExtension::Initialize()
{
	Super::Initialize();
	ApplyStyleSheet();

	UUserWidget* UserWidget = GetUserWidget();
	UWidgetMarkupBlueprintGeneratedClassExtension* ClassExtension =
		UWidgetMarkupBlueprintGeneratedClassExtension::GetWidgetMarkupExtension(UserWidget);
	if (UserWidget)
	{
		FWidgetMarkupModule& WidgetMarkupModule = FModuleManager::GetModuleChecked<FWidgetMarkupModule>(TEXT("WidgetMarkup"));
		WidgetMarkupModule.GetOnWidgetMarkupUserWidgetInitialized().Broadcast(UserWidget, ClassExtension);
	}
}

void UWidgetMarkupUserWidgetExtension::SetWidgetMarkupComponent(TSharedPtr<IWidgetMarkupComponent> InWidgetMarkupComponent)
{
	WidgetMarkupComponent = MoveTemp(InWidgetMarkupComponent);
}

void UWidgetMarkupUserWidgetExtension::ApplyStyleSheet()
{
	UUserWidget* UserWidget = GetUserWidget();
	if (!UserWidget) return;

	UWidgetMarkupBlueprintGeneratedClassExtension* ClassExtension =
		UWidgetMarkupBlueprintGeneratedClassExtension::GetWidgetMarkupExtension(UserWidget);
	if (!ClassExtension) return;

	UWidgetStyleSheet* Sheet = ClassExtension->GetStyleSheet();
	if (Sheet)
	{
		Sheet->ApplyToUserWidget(UserWidget);
	}
}