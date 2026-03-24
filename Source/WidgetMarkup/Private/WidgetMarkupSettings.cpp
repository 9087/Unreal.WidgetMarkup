// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "WidgetMarkupSettings.h"

#include "WidgetMarkupModule.h"

const UWidgetMarkupSettings& UWidgetMarkupSettings::Get()
{
	return *GetDefault<UWidgetMarkupSettings>();
}

#if WITH_EDITOR

void UWidgetMarkupSettings::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	auto Property = PropertyChangedEvent.PropertyChain.GetHead();
	if (Property && Property->GetValue() && Property->GetValue()->GetName() == GET_MEMBER_NAME_CHECKED(UWidgetMarkupSettings, SourceFileDirectoryPaths))
	{
		if (auto WidgetMarkupModule = FModuleManager::Get().GetModulePtr<FWidgetMarkupModule>("WidgetMarkup"))
		{
			WidgetMarkupModule->StopSourceFileWatching();
			for (const FDirectoryPath& DirectoryPath : this->SourceFileDirectoryPaths)
			{
				WidgetMarkupModule->StartSourceFileWatching(DirectoryPath);
			}
		}
	}
}

#endif
