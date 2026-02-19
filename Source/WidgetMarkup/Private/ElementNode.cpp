// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "ElementNode.h"

#include "WidgetMarkupModule.h"

FElementNode::FMessage::FMessage(const FText& InText, EMessageType InType)
	: Text(InText)
	, Type(InType)
{
}

const FText& FElementNode::FMessage::GetText() const
{
	return Text;
}

FElementNode::FResult& FElementNode::FResult::Log(const FText& MessageText)
{
	Messages.Add(MakeShared<FMessage>(MessageText, EMessageType::Log));
	return *this;
}

FElementNode::FResult& FElementNode::FResult::Error(const FText& MessageText)
{
	Messages.Add(MakeShared<FMessage>(MessageText, EMessageType::Error));
	return *this;
}

FElementNode::FResult FElementNode::FResult::Success()
{
	FResult Result;
	Result.bOK = true;
	return Result;
}

FElementNode::FResult FElementNode::FResult::Failure()
{
	FResult Result;
	Result.bOK = false;
	return Result;
}

FElementNode::FResult::operator bool() const
{
	return bOK;
}

bool FElementNode::FResult::operator!() const
{
	return !bOK;
}

FElementNode::FResult& FElementNode::FResult::PrintOnFailure()
{
	if (!bOK)
	{
		UE_LOG(LogWidgetMarkup, Error, TEXT("Failed to Compile the Widget Markup."));
		for (const auto& Message : Messages)
		{
			UE_LOG(LogWidgetMarkup, Error, TEXT("%s"), *Message.Get().GetText().ToString());
		}
	}
	return *this;
}

void FElementNode::FContext::Push(const TSharedRef<FElementNode>& ElementNode)
{
	Nodes.Push(ElementNode);
}

TSharedRef<FElementNode> FElementNode::FContext::Pop()
{
	return Nodes.Pop();
}

bool FElementNode::FContext::IsEmpty() const
{
	return Nodes.IsEmpty();
}

TSharedPtr<FElementNode> FElementNode::FContext::GetLastNode() const
{
	if (Nodes.IsEmpty())
	{
		return nullptr;
	}
	return Nodes.Last();
}

UObject* FElementNode::FContext::FindObject(UClass* Class) const
{
	for (int Index = Nodes.Num() - 1; Index >= 0; Index--)
	{
		auto Node = Nodes[Index];
		auto Object = Node->GetObject();
		if (Object->IsA(Class))
		{
			return Object;
		}
	}
	return nullptr;
}

FElementNode::FElementNode()
{
}

FElementNode::~FElementNode()
{
}