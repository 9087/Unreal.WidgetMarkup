// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "Utilities/PropertyPath.h"
#include "PropertyBuffer.h"

struct FBufferedPropertyContext
{
	FBufferedPropertyContext()
	{
		check(IsStateConsistent());
	}

	FBufferedPropertyContext(const FPropertyPath& InRootPropertyPath, const TSharedPtr<FPropertyBuffer>& InPropertyBuffer)
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

	const FPropertyPath& GetRootPropertyPath() const
	{
		return RootPropertyPath;
	}

	bool MatchesPath(const FPropertyPath& PropertyPath) const
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

	FPropertyPath RootPropertyPath;
	TSharedPtr<FPropertyBuffer> PropertyBuffer;
};