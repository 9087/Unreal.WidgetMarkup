// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyRuns/WidgetDelegatePropertyRun.h"

#include "Binding/WidgetDelegateBinding.h"
#include "Binding/WidgetPropertyBindingUtility.h"
#include "Components/Widget.h"
#include "Extensions/WidgetMarkupBlueprintExtension.h"
#include "WidgetBlueprint.h"

TSharedRef<IPropertyRun> FWidgetDelegatePropertyRun::Create()
{
	return MakeShared<FWidgetDelegatePropertyRun>();
}

FElementNode::FResult FWidgetDelegatePropertyRun::OnBegin(FElementNode::FContext& /*Context*/, UObject* Object, const FStringView& PropertyName, const FStringView& PropertyValue)
{
	UWidget* Widget = Cast<UWidget>(Object);
	if (!Widget)
	{
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Delegate property target is not a Widget.")));
	}

	// Verify the property is actually a delegate on this widget type.
	const FName PropName(PropertyName);
	FProperty* Property = Widget->GetClass()->FindPropertyByName(PropName);
	if (!Property || !Property->IsA<FMulticastDelegateProperty>())
	{
		return FElementNode::FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("'{0}' is not a valid delegate property on '{1}'.")),
			FText::FromName(PropName),
			FText::FromString(Widget->GetClass()->GetName())));
	}

	// Reject binding expression syntax.
	FWidgetPropertyBindingToken BindingToken;
	if (TryParseWidgetPropertyBindingToken(PropertyValue, BindingToken))
	{
		return FElementNode::FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("Delegate binding does not support binding expression '{{{0}}}'.")),
			FText::FromString(BindingToken.SourceExpression)));
	}

	const FString Unescaped = UnescapeWidgetPropertyBindingLiteral(PropertyValue);
	if (Unescaped.IsEmpty())
	{
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Delegate binding requires a non-empty function name.")));
	}

	DelegatePropertyName = PropName;
	FunctionName = Unescaped;
	return FElementNode::FResult::Success();
}

FElementNode::FResult FWidgetDelegatePropertyRun::OnEnd(FElementNode::FContext& Context)
{
	if (!DelegatePropertyName.IsSet() || !FunctionName.IsSet())
	{
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Delegate property run has no pending binding to apply.")));
	}

	UWidgetBlueprint* WidgetBlueprint = Context.FindObject<UWidgetBlueprint>();
	if (!WidgetBlueprint)
	{
		DelegatePropertyName.Reset();
		FunctionName.Reset();
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Failed to finalize delegate binding: no UWidgetBlueprint in context.")));
	}

	// Resolve the target widget name from the current element node (top of stack).
	FName TargetWidgetName;
	if (TSharedPtr<FElementNode> LastObjectNode = Context.GetLastObjectNode())
	{
		if (UObject* NodeObject = LastObjectNode->GetObject())
		{
			TargetWidgetName = NodeObject->GetFName();
		}
	}

	if (TargetWidgetName.IsNone())
	{
		DelegatePropertyName.Reset();
		FunctionName.Reset();
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Failed to finalize delegate binding: could not resolve target widget name from context.")));
	}

	UWidgetMarkupBlueprintExtension* Extension = UWidgetMarkupBlueprintExtension::RequestExtension<UWidgetMarkupBlueprintExtension>(WidgetBlueprint);
	if (!Extension)
	{
		DelegatePropertyName.Reset();
		FunctionName.Reset();
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Failed to finalize delegate binding: could not create WidgetMarkupBlueprintExtension.")));
	}

	Extension->AddDelegateBinding(FWidgetDelegateBinding::Create(
		TargetWidgetName,
		DelegatePropertyName.GetValue(),
		FunctionName.GetValue()));

	DelegatePropertyName.Reset();
	FunctionName.Reset();
	return FElementNode::FResult::Success();
}
