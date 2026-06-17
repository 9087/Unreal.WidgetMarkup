// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Classes/PythonCore.h"
#include "Classes/PythonWidgetLibrary.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"
#include "Components/Widget.h"
#include "Misc/PackageName.h"
#include "PyConversion.h"
#include "PythonUtilities.h"
#include "PythonWidgetMarkupListItem.h"
#include "WidgetMarkupModule.h"

#if defined(WITH_PYTHON) && WITH_PYTHON

namespace
{
	// -----------------------------------------------------------------------
	// Python-callable helpers
	// -----------------------------------------------------------------------

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

	PyObject* PyGetPythonObjectFromListItem(PyObject* /*Self*/, PyObject* Args)
	{
		PyObject* PyItem = nullptr;
		if (!PyArg_ParseTuple(Args, "O:get_python_object_from_list_item", &PyItem))
		{
			return nullptr;
		}

		UObject* ItemObject = nullptr;
		if (!PyConversion::NativizeObject(PyItem, ItemObject, UPythonWidgetMarkupListItem::StaticClass()))
		{
			Py_RETURN_NONE;
		}

		UPythonWidgetMarkupListItem* ListItem = Cast<UPythonWidgetMarkupListItem>(ItemObject);
		if (ListItem)
		{
			PyObject* PythonObject = ListItem->GetPythonObject();
			if (PythonObject)
			{
				Py_INCREF(PythonObject);
				return PythonObject;
			}
		}
		Py_RETURN_NONE;
	}

	PyObject* PyAddChildWidget(PyObject* /*Self*/, PyObject* Args)
	{
		PyObject* PyUserWidget = nullptr;
		const char* ParentName = nullptr;
		PyObject* PyClass = nullptr;       // UClass object or None (if class_token is used)
		const char* ClassToken = nullptr;  // string class token (may be nullptr)
		const char* ChildName = nullptr;
		if (!PyArg_ParseTuple(Args, "OsOzz:add_child_widget",
			&PyUserWidget, &ParentName, &PyClass, &ClassToken, &ChildName))
		{
			return nullptr;
		}

		// --- Resolve owning UserWidget ---
		UObject* UserWidgetObject = nullptr;
		if (!PyConversion::NativizeObject(PyUserWidget, UserWidgetObject, UUserWidget::StaticClass()))
		{
			PyErr_SetString(PyExc_TypeError, "First argument must be a UserWidget.");
			return nullptr;
		}
		UUserWidget* UserWidget = Cast<UUserWidget>(UserWidgetObject);
		if (!UserWidget || !UserWidget->WidgetTree)
		{
			PyErr_SetString(PyExc_ValueError, "UserWidget has no WidgetTree.");
			return nullptr;
		}

		// --- Resolve widget class ---
		UClass* WidgetClass = nullptr;
		if (PyClass && PyClass != Py_None)
		{
			UClass* ClassObject = nullptr;
			if (!PyConversion::NativizeClass(PyClass, ClassObject, UWidget::StaticClass()))
			{
				PyErr_SetString(PyExc_TypeError, "Class argument must be a UClass.");
				return nullptr;
			}
			WidgetClass = ClassObject;
		}
		else if (ClassToken && ClassToken[0] != '\0')
		{
			WidgetClass = FWidgetMarkupCore::ResolveClass(FString(UTF8_TO_TCHAR(ClassToken)));
		}

		if (!WidgetClass)
		{
			PyErr_SetString(PyExc_ValueError, "Could not resolve widget class.");
			return nullptr;
		}

		if (!WidgetClass->IsChildOf(UWidget::StaticClass()))
		{
			PyErr_SetString(PyExc_TypeError, "Resolved class is not a UWidget subclass.");
			return nullptr;
		}

		const FName ChildFName(UTF8_TO_TCHAR(ChildName));

		// --- Validate parent widget ---
		const FName ParentFName(UTF8_TO_TCHAR(ParentName));
		UWidget* ParentWidget = UserWidget->WidgetTree->FindWidget(ParentFName);
		if (!ParentWidget)
		{
			PyErr_Format(PyExc_ValueError, "Parent widget '%s' not found in WidgetTree.", ParentName);
			return nullptr;
		}

		UPanelWidget* ParentPanel = Cast<UPanelWidget>(ParentWidget);
		if (!ParentPanel)
		{
			PyErr_Format(PyExc_TypeError, "Parent widget '%s' is not a panel widget and cannot hold children.", ParentName);
			return nullptr;
		}

		// --- Validate child name uniqueness ---
		if (UserWidget->WidgetTree->FindWidget(ChildFName))
		{
			PyErr_Format(PyExc_ValueError, "A widget named '%s' already exists in the WidgetTree.", ChildName);
			return nullptr;
		}

		// --- Validate slot capacity ---
		if (!ParentPanel->CanHaveMultipleChildren() && ParentPanel->GetChildrenCount() > 0)
		{
			PyErr_Format(PyExc_ValueError, "Parent widget '%s' only supports a single child and already has one.", ParentName);
			return nullptr;
		}

		if (!ParentPanel->CanAddMoreChildren())
		{
			PyErr_Format(PyExc_ValueError, "Parent widget '%s' cannot accept more children.", ParentName);
			return nullptr;
		}

		// --- Create the child widget ---
		UWidget* ChildWidget = nullptr;
		if (WidgetClass->IsChildOf(UUserWidget::StaticClass()))
		{
			ChildWidget = UUserWidget::CreateWidgetInstance(*UserWidget->WidgetTree,
				TSubclassOf<UUserWidget>(WidgetClass), ChildFName);
		}
		else
		{
			ChildWidget = UserWidget->WidgetTree->ConstructWidget<UWidget>(WidgetClass, ChildFName);
		}

		if (!ChildWidget)
		{
			PyErr_SetString(PyExc_RuntimeError, "Failed to create child widget.");
			return nullptr;
		}

		// --- Add to parent ---
		ParentPanel->AddChild(ChildWidget);

		return PyConversion::PythonizeObject(ChildWidget);
	}

	PyObject* PyRemoveChildWidget(PyObject* /*Self*/, PyObject* Args)
	{
		PyObject* PyUserWidget = nullptr;
		PyObject* PyChild = nullptr;  // string (name) or UWidget
		if (!PyArg_ParseTuple(Args, "OO:remove_child_widget", &PyUserWidget, &PyChild))
		{
			return nullptr;
		}

		UObject* UserWidgetObject = nullptr;
		if (!PyConversion::NativizeObject(PyUserWidget, UserWidgetObject, UUserWidget::StaticClass()))
		{
			PyErr_SetString(PyExc_TypeError, "First argument must be a UserWidget.");
			return nullptr;
		}
		UUserWidget* UserWidget = Cast<UUserWidget>(UserWidgetObject);
		if (!UserWidget || !UserWidget->WidgetTree)
		{
			Py_RETURN_FALSE;
		}

		UWidget* ChildWidget = nullptr;

		// Try string (name) first
		if (PyUnicode_Check(PyChild))
		{
			const char* ChildName = PyUnicode_AsUTF8(PyChild);
			ChildWidget = UserWidget->WidgetTree->FindWidget(FName(UTF8_TO_TCHAR(ChildName)));
		}
		else
		{
			UObject* ChildObject = nullptr;
			if (PyConversion::NativizeObject(PyChild, ChildObject, UWidget::StaticClass()))
			{
				ChildWidget = Cast<UWidget>(ChildObject);
			}
		}

		if (!ChildWidget)
		{
			Py_RETURN_FALSE;
		}

		ChildWidget->RemoveFromParent();
		Py_RETURN_TRUE;
	}

	PyMethodDef WidgetLibraryMethods[] =
	{
		{ "find_widget_in_user_widget", PyFindWidgetInUserWidget, METH_VARARGS | METH_STATIC, "Find a widget by name in a UserWidget's WidgetTree." },
		{ "get_python_object_from_list_item", PyGetPythonObjectFromListItem, METH_VARARGS | METH_STATIC, "Get the raw Python value from a PythonWidgetMarkupListItem." },
		{ "add_child_widget", PyAddChildWidget, METH_VARARGS | METH_STATIC, "Create a child widget and add it to a parent panel in the WidgetTree." },
		{ "remove_child_widget", PyRemoveChildWidget, METH_VARARGS | METH_STATIC, "Remove a child widget by name or reference." },
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
