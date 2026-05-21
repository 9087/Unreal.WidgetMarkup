// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"

#include "WidgetMarkupLoopCommandlet.generated.h"

UCLASS()
class WIDGETMARKUP_API UWidgetMarkupLoopCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UWidgetMarkupLoopCommandlet();

	virtual int32 Main(const FString& Params) override;
};
