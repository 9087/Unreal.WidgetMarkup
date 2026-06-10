// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyRuns/WidgetStylePropertyRun.h"

#include "Extensions/WidgetMarkupBlueprintExtension.h"
#include "WidgetBlueprint.h"

TSharedRef<IPropertyRun> FWidgetStylePropertyRun::Create()
{
	return MakeShared<FWidgetStylePropertyRun>();
}

FElementNode::FResult FWidgetStylePropertyRun::OnBegin(FElementNode::FContext& Context, UObject* Object, const FStringView& PropertyName, const FStringView& PropertyValue)
{
	// Defer to OnEnd — widget name may not be set yet when Style attribute is processed.
	StyleName = FName(PropertyValue);
	return FElementNode::FResult::Success();
}

FElementNode::FResult FWidgetStylePropertyRun::OnEnd(FElementNode::FContext& Context)
{
	if (!StyleName.IsSet())
	{
		return FElementNode::FResult::Success();
	}

	// At this point the widget's Name attribute has been processed, so we can read it.
	if (UWidget* Widget = Context.FindObject<UWidget>())
	{
		UWidgetBlueprint* WidgetBlueprint = Context.FindObject<UWidgetBlueprint>();
		if (!WidgetBlueprint)
		{
			StyleName.Reset();
			return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Style property: no UWidgetBlueprint in context.")));
		}

		UWidgetMarkupBlueprintExtension* Extension = UWidgetMarkupBlueprintExtension::RequestExtension<UWidgetMarkupBlueprintExtension>(WidgetBlueprint);
		if (!Extension)
		{
			StyleName.Reset();
			return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Style property: could not get WidgetMarkupBlueprintExtension.")));
		}

		Extension->AddWidgetStyleAssignment(Widget->GetFName(), StyleName.GetValue());
	}

	StyleName.Reset();
	return FElementNode::FResult::Success();
}
