// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyRuns/WidgetBlueprintScriptPropertyRun.h"

#include "Binding/WidgetPropertyBindingUtility.h"
#include "WidgetBlueprint.h"
#include "Extensions/WidgetMarkupBlueprintExtension.h"

TSharedRef<IPropertyRun> FWidgetBlueprintScriptPropertyRun::Create()
{
	return MakeShared<FWidgetBlueprintScriptPropertyRun>();
}

FElementNode::FResult FWidgetBlueprintScriptPropertyRun::OnBegin(FElementNode::FContext& /*Context*/, UObject* Object, const FStringView& /*PropertyName*/, const FStringView& PropertyValue)
{
	FWidgetPropertyBindingToken BindingToken;
	if (TryParseWidgetPropertyBindingToken(PropertyValue, BindingToken))
	{
		return FElementNode::FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("Script does not support binding expression '{{{0}}}'.")),
			FText::FromString(BindingToken.SourceExpression)));
	}

	UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(Object);
	if (!WidgetBlueprint)
	{
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Script property target is not a WidgetBlueprint.")));
	}

	Script = UnescapeWidgetPropertyBindingLiteral(PropertyValue);
	return FElementNode::FResult::Success();
}

FElementNode::FResult FWidgetBlueprintScriptPropertyRun::OnEnd(FElementNode::FContext& Context)
{
	if (!Script.IsSet())
	{
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Script property run has no pending script value to apply.")));
	}

	UWidgetBlueprint* WidgetBlueprint = Context.FindObject<UWidgetBlueprint>();
	if (!WidgetBlueprint)
	{
		Script.Reset();
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Failed to finalize Script property: no UWidgetBlueprint was found in context.")));
	}

	UWidgetMarkupBlueprintExtension* Extension = UWidgetMarkupBlueprintExtension::RequestExtension<UWidgetMarkupBlueprintExtension>(WidgetBlueprint);
	if (!Extension)
	{
		Script.Reset();
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Failed to finalize Script property: could not create WidgetMarkupBlueprintExtension.")));
	}

	Extension->SetScript(Script.GetValue());
	Script.Reset();
	return FElementNode::FResult::Success();
}
