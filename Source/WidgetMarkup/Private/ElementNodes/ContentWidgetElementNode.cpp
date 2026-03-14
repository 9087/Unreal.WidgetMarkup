// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "ContentWidgetElementNode.h"

#include "Components/ContentWidget.h"

#define LOCTEXT_NAMESPACE "WidgetMarkup"

TSharedRef<FElementNode> FContentWidgetElementNode::Create()
{
	return MakeShared<FContentWidgetElementNode>();
}

FElementNode::FResult FContentWidgetElementNode::OnAddChild(const TSharedRef<FElementNode>& Child)
{
	auto ContentWidget = Cast<UContentWidget>(Object);
	if (!ensure(ContentWidget))
	{
		return FResult::Failure().Error(FText::FromString(TEXT("ContentWidgetElementNode: underlying object is not a valid UContentWidget instance.")));
	}
	if (ContentWidget->GetContent() != nullptr)
	{
		return FResult::Failure().Error(FText::Format(FText::FromString(TEXT("{0} instance only allows one child.")), Object->GetClass()->GetDisplayNameText()));
	}
	return FPanelWidgetElementNode::OnAddChild(Child);
}

#undef LOCTEXT_NAMESPACE