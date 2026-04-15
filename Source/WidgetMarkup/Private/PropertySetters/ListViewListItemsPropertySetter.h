// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "PropertySetter.h"

class FListViewListItemsPropertySetter : public FPropertySetter
{
public:
	static TSharedRef<FPropertySetter> Create();

	virtual bool SetValue(
		UObject* InObject,
		const FWidgetPropertyPath& InPropertyPath,
		FProperty* InTargetProperty,
		void* InTargetValueAddress,
		const FPropertyBuffer& InPropertyBuffer) const override;
};