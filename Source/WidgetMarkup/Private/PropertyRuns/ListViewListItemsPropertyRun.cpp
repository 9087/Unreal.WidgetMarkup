// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyRuns/ListViewListItemsPropertyRun.h"

#include "WidgetBlueprint.h"
#include "WidgetBlueprintExtension.h"
#include "Components/ListView.h"
#include "PropertyBuffer.h"
#include "ElementNodes/PropertyElementNode.h"
#include "Extensions/WidgetMarkupBlueprintExtension.h"
#include "Styles/WidgetStyleSheet.h"
#include "UObject/UnrealType.h"

TSharedRef<IPropertyRun> FListViewListItemsPropertyRun::Create()
{
	return MakeShared<FListViewListItemsPropertyRun>();
}

FElementNode::FResult FListViewListItemsPropertyRun::OnBegin(FElementNode::FContext& Context, UObject* Object, const FStringView& /*PropertyName*/, const FStringView& PropertyValue)
{
	UListView* ListView = Cast<UListView>(Object);
	if (!ListView)
	{
		const FString ObjectTypeName = Object ? Object->GetClass()->GetName() : TEXT("null");
		return FElementNode::FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("ListItems property target must be a UListView (or subclass), but got '{0}'.")),
			FText::FromString(ObjectTypeName)));
	}

	TSharedPtr<FPropertyElementNode> NewPropertyElementNode = MakeShared<FPropertyElementNode>(TEXT("ListItems"), PropertyValue, true);
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
	if (!CachedPropertyBuffer.IsValid() || !CachedPropertyBuffer->GetValueData())
	{
		Context.Pop();
		PropertyElementNode.Reset();
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Failed to finalize ListItems property: cached buffered data is invalid or uninitialized.")));
	}

	FArrayProperty* BufferedArrayProperty = CastField<FArrayProperty>(CachedPropertyBuffer->GetProperty());
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
		FScriptArrayHelper BufferedArrayHelper(BufferedArrayProperty, CachedPropertyBuffer->GetValueData());
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

	// Store the entire ListItemsSnapshot as a single FWidgetStyleEntry in the default StyleSheet
	UWidgetBlueprint* WidgetBlueprint = Context.FindObject<UWidgetBlueprint>();
	if (WidgetBlueprint)
	{
		UWidgetMarkupBlueprintExtension* WidgetMarkupBlueprintExtension = UWidgetBlueprintExtension::GetExtension<UWidgetMarkupBlueprintExtension>(WidgetBlueprint);
		if (WidgetMarkupBlueprintExtension)
		{
			FWidgetStyleEntry Entry;
			Entry.WidgetName = ListView->GetFName();
			Entry.PropertyPath = FWidgetPropertyPath(TEXT("ListItems"));
			Entry.PropertyValue = *CachedPropertyBuffer;
			WidgetMarkupBlueprintExtension->GetOrAddDefaultStyleSheet().AddOrReplaceStyleEntry(Entry);
		}
	}
	return FElementNode::FResult::Success();
}
