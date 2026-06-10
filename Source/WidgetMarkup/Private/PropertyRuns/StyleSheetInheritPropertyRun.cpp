// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyRuns/StyleSheetInheritPropertyRun.h"

#include "ElementNodes/StyleSheetElementNode.h"

TSharedRef<IPropertyRun> FStyleSheetInheritPropertyRun::Create()
{
	return MakeShared<FStyleSheetInheritPropertyRun>();
}

FElementNode::FResult FStyleSheetInheritPropertyRun::OnBegin(FElementNode::FContext& Context, UObject* Object, const FStringView& PropertyName, const FStringView& PropertyValue)
{
	if (auto* Node = static_cast<FStyleSheetElementNode*>(Context.GetLastNode().Get()))
	{
		Node->ResolveInheritFromPath(FString(PropertyValue));
	}
	return FElementNode::FResult::Success();
}

FElementNode::FResult FStyleSheetInheritPropertyRun::OnEnd(FElementNode::FContext& Context)
{
	return FElementNode::FResult::Success();
}
