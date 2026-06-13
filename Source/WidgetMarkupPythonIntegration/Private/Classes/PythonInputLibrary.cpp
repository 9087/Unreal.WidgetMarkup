// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Classes/PythonInputLibrary.h"

#include "Input/Events.h"
#include "InputCoreTypes.h"
#include "Kismet/KismetInputLibrary.h"
#include "PyConversion.h"
#include "PythonUtilities.h"

#if defined(WITH_PYTHON) && WITH_PYTHON

namespace
{
	bool TryNativizePointerEvent(PyObject* PyEvent, FPointerEvent& OutEvent)
	{
		if (!PyEvent)
		{
			return false;
		}

		return PyConversion::Internal::NativizeStructInstance(
			PyEvent,
			TBaseStructure<FPointerEvent>::Get(),
			&OutEvent,
			PyConversion::ESetErrorState::No).Succeeded();
	}

	bool TryNativizeKey(PyObject* PyKey, FKey& OutKey)
	{
		if (!PyKey)
		{
			return false;
		}

		if (PyUnicode_Check(PyKey))
		{
			const char* Utf8 = PyUnicode_AsUTF8(PyKey);
			if (!Utf8)
			{
				return false;
			}

			OutKey = FKey(FName(UTF8_TO_TCHAR(Utf8)));
			return true;
		}

		return PyConversion::Internal::NativizeStructInstance(
			PyKey,
			TBaseStructure<FKey>::Get(),
			&OutKey,
			PyConversion::ESetErrorState::No).Succeeded();
	}

	PyObject* PyPointerEventGetEffectingButton(PyObject* /*Self*/, PyObject* Args)
	{
		PyObject* PyEvent = nullptr;
		if (!PyArg_ParseTuple(Args, "O:pointer_event_get_effecting_button", &PyEvent))
		{
			return nullptr;
		}

		FPointerEvent PointerEvent;
		if (!TryNativizePointerEvent(PyEvent, PointerEvent))
		{
			Py_RETURN_NONE;
		}

		const FKey EffectingButton = UKismetInputLibrary::PointerEvent_GetEffectingButton(PointerEvent);
		PyObject* OutPyObject = nullptr;
		if (!PyConversion::Internal::PythonizeStructInstance(
			TBaseStructure<FKey>::Get(),
			&EffectingButton,
			OutPyObject,
			PyConversion::ESetErrorState::Yes).Succeeded())
		{
			return nullptr;
		}

		return OutPyObject;
	}

	PyObject* PyPointerEventIsMouseButtonDown(PyObject* /*Self*/, PyObject* Args)
	{
		PyObject* PyEvent = nullptr;
		PyObject* PyKey = nullptr;
		if (!PyArg_ParseTuple(Args, "OO:pointer_event_is_mouse_button_down", &PyEvent, &PyKey))
		{
			return nullptr;
		}

		FPointerEvent PointerEvent;
		if (!TryNativizePointerEvent(PyEvent, PointerEvent))
		{
			Py_RETURN_FALSE;
		}

		FKey MouseButton;
		if (!TryNativizeKey(PyKey, MouseButton))
		{
			Py_RETURN_FALSE;
		}

		return PyBool_FromLong(
			UKismetInputLibrary::PointerEvent_IsMouseButtonDown(PointerEvent, MouseButton) ? 1 : 0);
	}

	PyMethodDef InputLibraryMethods[] =
	{
		{ "pointer_event_get_effecting_button", PyPointerEventGetEffectingButton, METH_VARARGS | METH_STATIC, "Get the effecting mouse button from a PointerEvent (WidgetMarkupApp-safe)." },
		{ "pointer_event_is_mouse_button_down", PyPointerEventIsMouseButtonDown, METH_VARARGS | METH_STATIC, "Return whether a mouse button is down in a PointerEvent (WidgetMarkupApp-safe)." },
		{ nullptr, nullptr, 0, nullptr }
	};
}

bool RegisterPythonInputLibraryType(PyObject* Module)
{
	return FPythonUtilities::AddStaticMethodType(
		Module,
		"InputLibrary",
		"widget_markup.InputLibrary",
		InputLibraryMethods,
		"WidgetMarkupApp-safe pointer input helpers.");
}

#endif
