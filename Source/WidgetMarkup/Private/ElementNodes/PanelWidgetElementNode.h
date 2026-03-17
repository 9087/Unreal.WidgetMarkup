// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "WidgetElementNode.h"

class FPanelWidgetElementNode : public FWidgetElementNode
{
	DECLARE_ELEMENT_NODE(FPanelWidgetElementNode, FWidgetElementNode)

public:
	static TSharedRef<FElementNode> Create();

protected:
	//~Begin FElementNode interface
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child) override;
	//~End FElementNode interface
};