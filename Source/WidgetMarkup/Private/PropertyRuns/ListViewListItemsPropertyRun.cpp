// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyRuns/ListViewListItemsPropertyRun.h"

#include "Components/ListView.h"
#include "ElementNodes/PropertyBuffer.h"
#include "ElementNodes/PropertyElementNode.h"
#include "UObject/UnrealType.h"

TSharedRef<IPropertyRun> FListViewListItemsPropertyRun::Create()
{
	return MakeShared<FListViewListItemsPropertyRun>();
}

FElementNode::FResult FListViewListItemsPropertyRun::OnBegin(FElementNode::FContext& Context, UObject* Object, const FStringView& PropertyName, const FStringView& PropertyValue)
{
	if (!PropertyName.Equals(TEXT("ListItems"), ESearchCase::IgnoreCase))
	{
		return FElementNode::FResult::Failure();
	}

	UListView* ListView = Cast<UListView>(Object);
	if (!ListView)
	{
		const FString ObjectTypeName = Object ? Object->GetClass()->GetName() : TEXT("null");
		return FElementNode::FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("ListItems property target must be a UListView (or subclass), but got '{0}'.")),
			FText::FromString(ObjectTypeName)));
	}

	TSharedPtr<FPropertyElementNode> NewPropertyElementNode = MakeShared<FPropertyElementNode>(PropertyName, PropertyValue, true);
	FElementNode::FResult Result = NewPropertyElementNode->Begin(Context, ListView, nullptr);
	if (!Result)
	{
		return Result;
	}

	NewPropertyElementNode->SetPropertyRun(this->AsShared().ToSharedPtr());
	PropertyElementNode = NewPropertyElementNode;
	Context.Push(NewPropertyElementNode.ToSharedRef());
	return FElementNode::FResult::Success();
}

FElementNode::FResult FListViewListItemsPropertyRun::OnEnd(FElementNode::FContext& Context)
{
	if (!PropertyElementNode.IsValid())
	{
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("ListItems property run has no cached property node to finalize.")));
	}

	const TSharedPtr<const FPropertyBuffer> CachedPropertyBuffer = PropertyElementNode->GetPropertyBuffer();
	if (!CachedPropertyBuffer.IsValid() || !CachedPropertyBuffer->GetRootValueData())
	{
		Context.Pop();
		PropertyElementNode.Reset();
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Failed to finalize ListItems property: cached buffered data is invalid or uninitialized.")));
	}

	FArrayProperty* BufferedArrayProperty = CastField<FArrayProperty>(CachedPropertyBuffer->GetRootProperty());
	if (!BufferedArrayProperty)
	{
		Context.Pop();
		PropertyElementNode.Reset();
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Failed to finalize ListItems property: cached buffered root property is not an array.")));
	}

	FObjectPropertyBase* BufferedInnerObjectProperty = CastField<FObjectPropertyBase>(BufferedArrayProperty->Inner);
	if (!BufferedInnerObjectProperty)
	{
		Context.Pop();
		PropertyElementNode.Reset();
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Failed to finalize ListItems property: cached buffered array inner type is not an object property.")));
	}

	TArray<UObject*> ListItemsSnapshot;
	{
		FScriptArrayHelper BufferedArrayHelper(BufferedArrayProperty, CachedPropertyBuffer->GetRootValueData());
		const int32 BufferedNum = BufferedArrayHelper.Num();
		ListItemsSnapshot.Reserve(BufferedNum);
		for (int32 BufferedIndex = 0; BufferedIndex < BufferedNum; ++BufferedIndex)
		{
			void* BufferedElementPointer = BufferedArrayHelper.GetRawPtr(BufferedIndex);
			if (!BufferedElementPointer)
			{
				continue;
			}

			UObject* BufferedListItem = BufferedInnerObjectProperty->GetObjectPropertyValue(BufferedElementPointer);
			if (BufferedListItem)
			{
				ListItemsSnapshot.Add(BufferedListItem);
			}
		}
	}

	UListView* ListView = Context.FindObject<UListView>();
	if (!ListView)
	{
		Context.Pop();
		PropertyElementNode.Reset();
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Failed to finalize ListItems property: no UListView object was found in context.")));
	}

	FElementNode::FResult Result = PropertyElementNode->End();
	Context.Pop();
	PropertyElementNode.Reset();
	if (!Result)
	{
		return Result;
	}

	ListView->SetListItems(ListItemsSnapshot);
	return FElementNode::FResult::Success();
}
