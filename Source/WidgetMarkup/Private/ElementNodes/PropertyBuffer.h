// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

class FProperty;
class UStruct;

class FPropertyBuffer
{
public:
	FPropertyBuffer(FProperty* InRootProperty);

	bool Initialize();
	void Uninitialize();

	bool HasRootValue() const;
	void* GetRootValueData() const { return RootValueData; }
	FProperty* GetRootProperty() const { return RootProperty; }
	UStruct* GetRootStruct() const { return RootStruct; }

private:
	FProperty* RootProperty = nullptr;
	void* RootValueData = nullptr;
	UStruct* RootStruct = nullptr;
	FProperty* ArrayElementTargetProperty = nullptr;
	void* ArrayElementTargetAddress = nullptr;
};
