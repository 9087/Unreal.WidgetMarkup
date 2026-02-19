// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "WidgetMarkupSettings.generated.h"

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Widget Markup"))
class WIDGETMARKUP_API UWidgetMarkupSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static const UWidgetMarkupSettings& Get();

	UPROPERTY(Config, EditAnywhere, Category = "Common", meta = (RelativeToGameContentDir, LongPackageName))
	FDirectoryPath SourceFileDirectoryPath;

#if WITH_EDITOR
protected:
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
};