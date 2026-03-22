// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "EdGraph/EdGraphPin.h"

class FTypeParser
{
public:
	static bool ParseType(const FString& InTypeText, FEdGraphPinType& OutPinType, FString& OutError);
	static UClass* ResolveClass(const FString& InClassText);
	static UScriptStruct* ResolveStruct(const FString& InStructText);
	static UEnum* ResolveEnum(const FString& InEnumText);

private:
	static bool ParseTypeInternal(const FString& InTypeText, FEdGraphPinType& OutPinType, FString& OutError, bool bAllowContainer);
	static bool ParseContainer(const FString& InTypeText, FString& OutContainerName, FString& OutInnerText, FString& OutError);
	static bool ParseMapContainer(const FString& InInnerText, FString& OutKeyText, FString& OutValueText, FString& OutError);
	static FString NormalizeToken(const FString& InText);
};
