#include "PropertyRun.h"

#include "Binding/WidgetPropertyBindingCollection.h"
#include "Binding/WidgetPropertyBindingUtility.h"
#include "WidgetMarkupModule.h"
#include "ElementNodes/PropertyElementNode.h"
#include "ElementNodes/StructElementNode.h"
#include "PropertyRuns/ObjectNamePropertyRun.h"
#include "Components/Widget.h"

namespace
{
bool TryCreatePropertyBinding(
	FElementNode::FContext& Context,
	UObject* Object,
	const FStringView& PropertyName,
	const FString& SourceExpression,
	FWidgetPropertyBinding& OutBinding,
	FText& OutError)
{
	TSharedPtr<FElementNode> ObjectNode = Context.GetLastObjectNode();
	UObject* TargetObject = Object ? Object : (ObjectNode.IsValid() ? ObjectNode->GetObject() : nullptr);
	if (!TargetObject)
	{
		OutError = FText::FromString(TEXT("Binding target object is null."));
		return false;
	}

	FWidgetPropertyPath TargetPropertyPath;
	FBufferedPropertyContext BufferedPropertyContext;
	bool bUseBufferedWrite = false;
	if (!FPropertyElementNode::TryResolvePropertyPathFromContext(Context, PropertyName, bUseBufferedWrite, TargetPropertyPath, BufferedPropertyContext, &OutError))
	{
		return false;
	}

	if (UWidget* Widget = Cast<UWidget>(TargetObject))
	{
		if (!FObjectNamePropertyMetaData::IsWidgetMarkupObjectNameRegistered(Context, Widget)
			&& !FObjectNamePropertyMetaData::TryApplyGeneratedWidgetMarkupObjectName(Context, Widget, OutError))
		{
			return false;
		}
	}

	OutBinding = FWidgetPropertyBinding::Create(SourceExpression, TargetObject->GetFName(), TargetPropertyPath);
	return true;
}
}

FElementNode::FResult FPropertyRun::OnBegin(FElementNode::FContext& Context, UObject* Outer, const FStringView& PropertyName, const FStringView& PropertyValue)
{
	if (!Context.GetLastNode()->HasProperty(PropertyName))
	{
		return FElementNode::FResult::Failure();
	}

	FWidgetPropertyBindingToken BindingToken;
	if (Context.HasMetaData<FWidgetPropertyAttributeValueScope>()
		&& TryParseWidgetPropertyBindingToken(PropertyValue, BindingToken))
	{
		FWidgetPropertyBinding Binding;
		FText Error;
		if (!TryCreatePropertyBinding(Context, Outer, PropertyName, BindingToken.SourceExpression, Binding, Error))
		{
			return FElementNode::FResult::Failure().Error(Error);
		}

		Context.GetOrAddMetaData<FWidgetPropertyBindingCollection>()->Bindings.Add(Binding);
		ElementNode.Reset();
		return FElementNode::FResult::Success();
	}

	const FString LiteralPropertyValue = UnescapeWidgetPropertyBindingLiteral(PropertyValue);
	auto NewElementNode = FPropertyElementNode::Create(PropertyName, LiteralPropertyValue);
	auto PropertyElementNode = StaticCastSharedRef<FPropertyElementNode>(NewElementNode);
	auto Result = NewElementNode->Begin(Context, Outer, nullptr);
	if (Result)
	{
		PropertyElementNode->SetPropertyRun(this->AsShared().ToSharedPtr());
		ElementNode = NewElementNode;
		Context.Push(PropertyElementNode);
	}
	return Result;
}

FElementNode::FResult FPropertyRun::OnEnd(FElementNode::FContext& Context)
{
	if (!ElementNode.IsValid())
	{
		return FElementNode::FResult::Success();
	}
	auto Result = ElementNode.Pin()->End();
	Context.Pop();
	return Result;
}
