// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PythonWidgetMarkupListEntry.h"

#include "PythonUtilities.h"

UPythonWidgetMarkupListEntry::~UPythonWidgetMarkupListEntry()
{
	if (PyValue)
	{
		Py_DECREF(PyValue);
		PyValue = nullptr;
	}
}

UPythonWidgetMarkupListEntry* UPythonWidgetMarkupListEntry::Create(UObject* Outer, PyObject* InPyValue)
{
	UPythonWidgetMarkupListEntry* Entry = NewObject<UPythonWidgetMarkupListEntry>(Outer);
	if (InPyValue)
	{
		Py_INCREF(InPyValue);
		Entry->PyValue = InPyValue;
	}
	return Entry;
}

FString UPythonWidgetMarkupListEntry::GetDisplayText() const
{
	if (PyValue)
	{
		return FPythonUtilities::PythonObjectToString(PyValue);
	}
	return TEXT("");
}
