// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "SetterElementNode.h"

#include "BasicTypeElementNode.h"
#include "ObjectElementNode.h"

IMPLEMENT_ELEMENT_NODE(FSetterElementNode, FStructElementNode)

TSharedRef<FElementNode> FSetterElementNode::Create()
{
	return MakeShared<FSetterElementNode>();
}

FElementNode::FResult FSetterElementNode::OnEnd()
{
	return FStructElementNode::OnEnd();
}

FElementNode::FResult FSetterElementNode::OnAddChild(const TSharedRef<FElementNode>& Child)
{
	// Accept inline values: BasicType, Struct, or Object children.
	if (CastElementNode<FBasicTypeElementNode>(TSharedPtr<FElementNode>(Child))
		|| CastElementNode<FStructElementNode>(TSharedPtr<FElementNode>(Child))
		|| CastElementNode<FObjectElementNode>(TSharedPtr<FElementNode>(Child)))
	{
		return FResult::Success();
	}
	return FStructElementNode::OnAddChild(Child);
}

FWidgetStyleSetter FSetterElementNode::MakeSetter() const
{
	FWidgetStyleSetter Setter;
	if (const auto* Memory = static_cast<const FWidgetStyleSetter*>(GetStructMemory()))
	{
		Setter = *Memory;
	}
	return Setter;
}
