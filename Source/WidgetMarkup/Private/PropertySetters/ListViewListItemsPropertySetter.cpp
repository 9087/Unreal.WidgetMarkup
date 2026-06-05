// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertySetters/ListViewListItemsPropertySetter.h"

#include "Components/ListView.h"
#include "PropertyBuffer.h"
#include "UObject/UnrealType.h"

TSharedRef<FPropertySetter> FListViewListItemsPropertySetter::Create()
{
	return MakeShared<FListViewListItemsPropertySetter>();
}

bool FListViewListItemsPropertySetter::SetValue(
	void* InContainer,
	const FWidgetPropertyPath& InPropertyPath,
	FProperty* InTargetProperty,
	void* InTargetValueAddress,
	const FPropertyBuffer& InPropertyBuffer) const
{
	UListView* ListView = Cast<UListView>(static_cast<UObject*>(InContainer));
	if (!ListView || InPropertyPath.IsEmpty() || !InTargetProperty || !InTargetValueAddress || !InPropertyBuffer.HasValue())
	{
		return false;
	}

	TArray<UObject*> ListItems;
	if (!InPropertyBuffer.ToArray<UObject>(ListItems))
	{
		return false;
	}

	ListView->SetListItems(ListItems);
	return true;
}