// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Styles/WidgetStyleSheet.h"

#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "ElementNodes/PropertyChainHandle.h"
#include "WidgetMarkupModule.h"

bool FWidgetStyleEntry::CanApply(FString* OutErrorMessage) const
{
	if (WidgetName.IsNone())
	{
		if (OutErrorMessage)
		{
			*OutErrorMessage = TEXT("WidgetName is not set.");
		}
		return false;
	}

	if (!PropertyValue.HasValue())
	{
		if (OutErrorMessage)
		{
			*OutErrorMessage = TEXT("PropertyValue is not initialized.");
		}
		return false;
	}

	if (PropertyPath.IsEmpty())
	{
		if (OutErrorMessage)
		{
			*OutErrorMessage = TEXT("PropertyPath is empty.");
		}
		return false;
	}

	return true;
}

void FWidgetStyleSheetData::AddOrReplaceStyleEntry(const FWidgetStyleEntry& Entry)
{
	for (FWidgetStyleEntry& Existing : Styles)
	{
		if (Existing.WidgetName == Entry.WidgetName && Existing.PropertyPath == Entry.PropertyPath)
		{
			Existing = Entry;
			return;
		}
	}
	Styles.Add(Entry);
}

bool FWidgetStyleSheetData::ApplyToUserWidget(UUserWidget* UserWidget) const
{
	bool bAllSucceeded = true;
	if (!UserWidget)
	{
		UE_LOG(LogWidgetMarkup, Warning, TEXT("WidgetStyleSheet: failed to apply styles because UserWidget is null."));
		return false;
	}

	for (int32 StyleIndex = 0; StyleIndex < Styles.Num(); ++StyleIndex)
	{
		const FWidgetStyleEntry& Style = Styles[StyleIndex];
		FString ErrorMessage;
		if (!Style.CanApply(&ErrorMessage))
		{
			bAllSucceeded = false;
			UE_LOG(LogWidgetMarkup, Warning, TEXT("WidgetStyleSheet: style [%d] is invalid. Reason: %s"), StyleIndex, *ErrorMessage);
			continue;
		}

		UWidget* WidgetNode = UserWidget->GetWidgetFromName(Style.WidgetName);
		if (!WidgetNode)
		{
			bAllSucceeded = false;
			UE_LOG(LogWidgetMarkup, Warning, TEXT("WidgetStyleSheet: style [%d] could not find widget '%s'."), StyleIndex, *Style.WidgetName.ToString());
			continue;
		}

		const TSharedPtr<FPropertyChainHandle> PropertyChainHandle = FPropertyChainHandle::Create(WidgetNode, Style.PropertyPath);
		if (!PropertyChainHandle.IsValid())
		{
			bAllSucceeded = false;
			UE_LOG(LogWidgetMarkup, Warning, TEXT("WidgetStyleSheet: style [%d] failed to resolve property path '%s' on widget '%s'."), StyleIndex, *Style.PropertyPath.GetPathName().ToString(), *Style.WidgetName.ToString());
			continue;
		}

		if (!PropertyChainHandle->SetValue(Style.PropertyValue))
		{
			bAllSucceeded = false;
			UE_LOG(LogWidgetMarkup, Warning, TEXT("WidgetStyleSheet: style [%d] failed to apply property buffer on widget '%s' path '%s'."), StyleIndex, *Style.WidgetName.ToString(), *Style.PropertyPath.GetPathName().ToString());
			continue;
		}
	}

	return bAllSucceeded;
}

bool UWidgetStyleSheet::ApplyToUserWidget(UUserWidget* UserWidget) const
{
	return StyleSheet.ApplyToUserWidget(UserWidget);
}
