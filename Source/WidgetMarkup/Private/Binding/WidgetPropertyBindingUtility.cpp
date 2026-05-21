#include "Binding/WidgetPropertyBindingUtility.h"

bool TryParseWidgetPropertyBindingToken(
	const FStringView& PropertyValue,
	FWidgetPropertyBindingToken& OutToken)
{
	OutToken.SourceExpression.Reset();

	if (PropertyValue.IsEmpty())
	{
		return false;
	}

	if (PropertyValue.StartsWith(TEXT("{}")))
	{
		return false;
	}

	if (PropertyValue.Len() < 3 || PropertyValue[0] != TEXT('{') || PropertyValue[PropertyValue.Len() - 1] != TEXT('}'))
	{
		return false;
	}

	OutToken.SourceExpression = FString(PropertyValue.Mid(1, PropertyValue.Len() - 2));
	return !OutToken.SourceExpression.IsEmpty();
}

FString UnescapeWidgetPropertyBindingLiteral(const FStringView& PropertyValue)
{
	if (PropertyValue.StartsWith(TEXT("{}")))
	{
		return FString(PropertyValue.Mid(2));
	}
	return FString(PropertyValue);
}