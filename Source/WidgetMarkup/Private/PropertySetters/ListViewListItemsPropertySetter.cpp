// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertySetters/ListViewListItemsPropertySetter.h"

#include "Components/ListView.h"
#include "PropertyBuffer.h"

TSharedRef<FPropertySetter> FListViewListItemsPropertySetter::Create()
{
	return MakeShared<FListViewListItemsPropertySetter>();
}

bool FListViewListItemsPropertySetter::SetValue(
	UObject* InObject,
	const FWidgetPropertyPath& InPropertyPath,
	FProperty* InTargetProperty,
	void* InTargetValueAddress,
	const FPropertyBuffer& InPropertyBuffer) const
{
	UListView* ListView = Cast<UListView>(InObject);
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