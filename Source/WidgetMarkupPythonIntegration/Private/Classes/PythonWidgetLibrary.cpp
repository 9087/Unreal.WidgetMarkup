// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Classes/PythonWidgetLibrary.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"
#include "PyConversion.h"
#include "PythonUtilities.h"
#include "PythonWidgetMarkupListEntry.h"

#if defined(WITH_PYTHON) && WITH_PYTHON

namespace
{
	PyObject* PyFindWidgetInUserWidget(PyObject* /*Self*/, PyObject* Args)
	{
		PyObject* PyUserWidget = nullptr;
		const char* WidgetName = nullptr;
		if (!PyArg_ParseTuple(Args, "Os:find_widget_in_user_widget", &PyUserWidget, &WidgetName))
		{
			return nullptr;
		}

		UObject* UserWidgetObject = nullptr;
		if (!PyConversion::NativizeObject(PyUserWidget, UserWidgetObject, UUserWidget::StaticClass()))
		{
			return nullptr;
		}

		UUserWidget* UserWidget = Cast<UUserWidget>(UserWidgetObject);
		if (!UserWidget || !UserWidget->WidgetTree || !WidgetName)
		{
			Py_RETURN_NONE;
		}

		UWidget* FoundWidget = UserWidget->WidgetTree->FindWidget(FName(UTF8_TO_TCHAR(WidgetName)));
		if (!FoundWidget)
		{
			Py_RETURN_NONE;
		}

		return PyConversion::PythonizeObject(FoundWidget);
	}

	PyObject* PyGetPythonObjectFromListEntry(PyObject* /*Self*/, PyObject* Args)
	{
		PyObject* PyEntry = nullptr;
		if (!PyArg_ParseTuple(Args, "O:get_python_object_from_list_entry", &PyEntry))
		{
			return nullptr;
		}

		UObject* EntryObject = nullptr;
		if (!PyConversion::NativizeObject(PyEntry, EntryObject, UPythonWidgetMarkupListEntry::StaticClass()))
		{
			Py_RETURN_NONE;
		}

		UPythonWidgetMarkupListEntry* ListEntry = Cast<UPythonWidgetMarkupListEntry>(EntryObject);
		if (ListEntry)
		{
			PyObject* PythonObject = ListEntry->GetPythonObject();
			if (PythonObject)
			{
				Py_INCREF(PythonObject);
				return PythonObject;
			}
		}
		Py_RETURN_NONE;
	}

	PyMethodDef WidgetLibraryMethods[] =
	{
		{ "find_widget_in_user_widget", PyFindWidgetInUserWidget, METH_VARARGS | METH_STATIC, "Find a widget by name in a UserWidget's WidgetTree." },
		{ "get_python_object_from_list_entry", PyGetPythonObjectFromListEntry, METH_VARARGS | METH_STATIC, "Get the raw Python value from a PythonWidgetMarkupListEntry." },
		{ nullptr, nullptr, 0, nullptr }
	};
}

bool RegisterPythonWidgetLibraryType(PyObject* Module)
{
	return FPythonUtilities::AddStaticMethodType(
		Module,
		"WidgetLibrary",
		"widget_markup.WidgetLibrary",
		WidgetLibraryMethods,
		"WidgetMarkup widget lookup helpers.");
}

#endif
