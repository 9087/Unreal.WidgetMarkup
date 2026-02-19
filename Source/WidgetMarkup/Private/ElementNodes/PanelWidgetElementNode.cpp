// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PanelWidgetElementNode.h"

#include "Components/PanelWidget.h"

TSharedRef<FElementNode> FPanelWidgetElementNode::Create()
{
	return MakeShared<FPanelWidgetElementNode>();
}

FElementNode::FResult FPanelWidgetElementNode::OnAddChild(const TSharedRef<FElementNode>& Child)
{
	auto Widget = Cast<UWidget>(Child->GetObject());
	if (!Widget)
	{
		return FResult::Failure().Error(FText::Format(FText::FromString(TEXT("Any child of {0} must be of type Widget.")), Object->GetClass()->GetDisplayNameText()));
	}
	auto PanelWidget = Cast<UPanelWidget>(Object);
	PanelWidget->AddChild(Widget);
	return FResult::Success();
}
