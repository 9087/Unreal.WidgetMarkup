// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "PropertyBuffer.h"
#include "Utilities/WidgetPropertyPath.h"

struct FBufferedPropertyContext
{
	FBufferedPropertyContext()
	{
		check(IsStateConsistent());
	}

	FBufferedPropertyContext(const FWidgetPropertyPath& InRootPropertyPath, const TSharedPtr<FPropertyBuffer>& InPropertyBuffer)
	{
		if (!InPropertyBuffer.IsValid())
		{
			Reset();
			return;
		}

		RootPropertyPath = InRootPropertyPath;
		PropertyBuffer = InPropertyBuffer;
		check(IsStateConsistent());
	}

	void Reset()
	{
		RootPropertyPath.Reset();
		PropertyBuffer = nullptr;
		check(IsStateConsistent());
	}

	bool InValid() const
	{
		return !PropertyBuffer.IsValid();
	}

	TSharedPtr<FPropertyBuffer> GetPropertyBuffer()
	{
		return PropertyBuffer;
	}

	TSharedPtr<const FPropertyBuffer> GetPropertyBuffer() const
	{
		return PropertyBuffer;
	}

	const FWidgetPropertyPath& GetRootPropertyPath() const
	{
		return RootPropertyPath;
	}

	bool MatchesPath(const FWidgetPropertyPath& PropertyPath) const
	{
		return PropertyBuffer.IsValid() && !RootPropertyPath.IsEmpty() && RootPropertyPath == PropertyPath;
	}

private:
	bool IsStateConsistent() const
	{
		const bool bHasPropertyBuffer = PropertyBuffer.IsValid();
		const bool bHasRootPropertyPath = !RootPropertyPath.IsEmpty();
		return bHasPropertyBuffer == bHasRootPropertyPath;
	}

	FWidgetPropertyPath RootPropertyPath;
	TSharedPtr<FPropertyBuffer> PropertyBuffer;
};