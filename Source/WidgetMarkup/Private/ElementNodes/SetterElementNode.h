// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "StructElementNode.h"
#include "Styles/WidgetStyleSheet.h"

class FSetterElementNode : public FStructElementNode
{
	DECLARE_ELEMENT_NODE(FSetterElementNode, FStructElementNode)

public:
	static TSharedRef<FElementNode> Create();

	virtual FResult OnEnd() override;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child) override;

	FWidgetStyleSetter MakeSetter() const;
};
