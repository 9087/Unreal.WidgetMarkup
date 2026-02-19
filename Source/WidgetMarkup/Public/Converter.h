// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

class FConverter
{
public:
	virtual ~FConverter() = default;
	virtual bool Convert(const FProperty& Property, void* Data, const FStringView& String) = 0;
};