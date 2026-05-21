#pragma once

#include "CoreMinimal.h"

struct WIDGETMARKUP_API FWidgetPropertyBindingToken
{
	FString SourceExpression;
};

WIDGETMARKUP_API bool TryParseWidgetPropertyBindingToken(
	const FStringView& PropertyValue,
	FWidgetPropertyBindingToken& OutToken);

WIDGETMARKUP_API FString UnescapeWidgetPropertyBindingLiteral(const FStringView& PropertyValue);