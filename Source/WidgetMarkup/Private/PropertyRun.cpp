#include "PropertyRun.h"

#include "WidgetMarkupModule.h"
#include "ElementNodes/PropertyElementNode.h"

FElementNode::FResult FPropertyRun::OnBegin(FElementNode::FContext& Context, UObject* Outer, const FStringView& PropertyName, const FStringView& PropertyValue)
{
	if (!Context.GetLastNode()->HasProperty(PropertyName))
	{
		return FElementNode::FResult::Failure();
	}
	auto NewElementNode = FPropertyElementNode::Create(PropertyName, PropertyValue);
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
		return FElementNode::FResult::Failure();
	}
	auto Result = ElementNode.Pin()->End();
	Context.Pop();
	return Result;
}
