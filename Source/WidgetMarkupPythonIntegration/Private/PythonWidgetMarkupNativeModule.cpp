// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PythonWidgetMarkupNativeModule.h"

#include "Binding/WidgetPropertyBinding.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ListViewBase.h"
#include "Components/Widget.h"
#include "Modules/ModuleManager.h"
#include "PythonWidgetMarkupListEntry.h"
#include "PropertyBuffer.h"
#include "ElementNodes/PropertyChainHandle.h"
#include "PyConversion.h"
#include "PythonUtilities.h"
#include "Utilities/WidgetPropertyPath.h"
#include "WidgetMarkupPythonIntegration.h"
#include "WidgetMarkupModule.h"

namespace
{
	const char* NativeModuleName = "widget_markup";

#if defined(WITH_PYTHON) && WITH_PYTHON

	/** Convert a Python value into the native data buffer for a given UE property. */
	static bool NativizePythonValueToProperty(PyObject* PyValue, FProperty* Property, void* OutData)
	{
		if (!PyValue || !Property || !OutData)
		{
			return false;
		}

		if (CastField<FIntProperty>(Property))
		{
			int32 Value = static_cast<int32>(PyLong_AsLong(PyValue));
			if (PyErr_Occurred()) { PyErr_Clear(); return false; }
			FMemory::Memcpy(OutData, &Value, sizeof(Value));
			return true;
		}
		if (CastField<FInt64Property>(Property))
		{
			int64 Value = PyLong_AsLongLong(PyValue);
			if (PyErr_Occurred()) { PyErr_Clear(); return false; }
			FMemory::Memcpy(OutData, &Value, sizeof(Value));
			return true;
		}
		if (CastField<FFloatProperty>(Property))
		{
			float Value = static_cast<float>(PyFloat_AsDouble(PyValue));
			if (PyErr_Occurred()) { PyErr_Clear(); return false; }
			FMemory::Memcpy(OutData, &Value, sizeof(Value));
			return true;
		}
		if (CastField<FDoubleProperty>(Property))
		{
			double Value = PyFloat_AsDouble(PyValue);
			if (PyErr_Occurred()) { PyErr_Clear(); return false; }
			FMemory::Memcpy(OutData, &Value, sizeof(Value));
			return true;
		}
		if (CastField<FBoolProperty>(Property))
		{
			bool Value = PyObject_IsTrue(PyValue) != 0;
			static_cast<FBoolProperty*>(Property)->SetPropertyValue(OutData, Value);
			return true;
		}
		if (CastField<FStrProperty>(Property))
		{
			if (!PyUnicode_Check(PyValue)) return false;
			const char* Utf8 = PyUnicode_AsUTF8(PyValue);
			if (!Utf8) return false;
			FString Value(UTF8_TO_TCHAR(Utf8));
			static_cast<FStrProperty*>(Property)->SetPropertyValue(OutData, Value);
			return true;
		}
		if (CastField<FTextProperty>(Property))
		{
			FString TextString;
			if (PyUnicode_Check(PyValue))
			{
				const char* Utf8 = PyUnicode_AsUTF8(PyValue);
				if (!Utf8) return false;
				TextString = UTF8_TO_TCHAR(Utf8);
			}
			else if (PyLong_Check(PyValue))
			{
				const long LongVal = PyLong_AsLong(PyValue);
				if (PyErr_Occurred()) { PyErr_Clear(); return false; }
				TextString = FString::Printf(TEXT("%ld"), LongVal);
			}
			else if (PyFloat_Check(PyValue))
			{
				const double DoubleVal = PyFloat_AsDouble(PyValue);
				if (PyErr_Occurred()) { PyErr_Clear(); return false; }
				TextString = FString::SanitizeFloat(DoubleVal);
			}
			else
			{
				return false;
			}
			FText Value = FText::FromString(TextString);
			static_cast<FTextProperty*>(Property)->SetPropertyValue(OutData, Value);
			return true;
		}
		if (CastField<FNameProperty>(Property))
		{
			if (!PyUnicode_Check(PyValue)) return false;
			const char* Utf8 = PyUnicode_AsUTF8(PyValue);
			if (!Utf8) return false;
			FName Value(UTF8_TO_TCHAR(Utf8));
			static_cast<FNameProperty*>(Property)->SetPropertyValue(OutData, Value);
			return true;
		}
		if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			return PyConversion::Internal::NativizeStructInstance(PyValue, StructProperty->Struct, OutData, PyConversion::ESetErrorState::No).Succeeded();
		}
		if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
		{
			if (!PyList_Check(PyValue)) return false;

			FProperty* InnerProperty = ArrayProperty->Inner;
			FScriptArrayHelper ArrayHelper(ArrayProperty, OutData);
			const Py_ssize_t ListLen = PyList_Size(PyValue);
			ArrayHelper.Resize(ListLen);
			for (Py_ssize_t i = 0; i < ListLen; ++i)
			{
				PyObject* PyElement = PyList_GetItem(PyValue, i);
				void* ElementPtr = ArrayHelper.GetRawPtr(i);
				if (!PyElement || !ElementPtr) return false;

				// For UObject arrays (e.g. ListView items), auto-wrap primitive Python
				// values in UPythonWidgetMarkupListEntry so the ListView can display them.
				if (FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(InnerProperty))
				{
					UObject* Object = nullptr;
					if (PyConversion::NativizeObject(PyElement, Object, UObject::StaticClass()))
					{
						ObjectProperty->SetObjectPropertyValue(ElementPtr, Object);
					}
					else
					{
						PyErr_Clear(); // NativizeObject failed for non-UObject value; clear the error.
						UPythonWidgetMarkupListEntry* Entry = UPythonWidgetMarkupListEntry::Create(GetTransientPackage(), PyElement);
						ObjectProperty->SetObjectPropertyValue(ElementPtr, Entry);
					}
				}
				else if (!NativizePythonValueToProperty(PyElement, InnerProperty, ElementPtr))
				{
					return false;
				}
			}
			return true;
		}

		return false;
	}

	PyObject* PyApplyPropertyBinding(PyObject* /*Self*/, PyObject* Args)
	{
		PyObject* PyUserWidget = nullptr;
		PyObject* PyBinding = nullptr;
		PyObject* PyValue = nullptr;
		if (!PyArg_ParseTuple(Args, "OOO:apply_property_binding", &PyUserWidget, &PyBinding, &PyValue))
		{
			return nullptr;
		}

		UObject* UserWidgetObject = nullptr;
		if (!PyConversion::NativizeObject(PyUserWidget, UserWidgetObject, UUserWidget::StaticClass()))
		{
			return nullptr;
		}

		FWidgetPropertyBinding Binding;
		if (!PyConversion::Internal::NativizeStructInstance(PyBinding, FWidgetPropertyBinding::StaticStruct(), &Binding, PyConversion::ESetErrorState::Yes))
		{
			return nullptr;
		}

		UUserWidget* UserWidget = Cast<UUserWidget>(UserWidgetObject);
		if (!UserWidget || !UserWidget->WidgetTree || !PyValue)
		{
			Py_RETURN_NONE;
		}

		FWidgetMarkupModule* WidgetMarkupModule = FModuleManager::GetModulePtr<FWidgetMarkupModule>(TEXT("WidgetMarkup"));
		if (!WidgetMarkupModule)
		{
			Py_RETURN_NONE;
		}

		if (Binding.SourceExpression.IsEmpty() || Binding.TargetObjectName.IsNone() || Binding.TargetPropertyPath.IsEmpty())
		{
			Py_RETURN_NONE;
		}

		UWidget* TargetWidget = UserWidget->WidgetTree->FindWidget(Binding.TargetObjectName);
		if (!TargetWidget)
		{
			UE_LOG(LogWidgetMarkupPythonIntegration, Warning, TEXT("Binding skipped because target widget '%s' was not found."), *Binding.TargetObjectName.ToString());
			Py_RETURN_NONE;
		}

		FText Error;
		FWidgetPropertyPath TargetPropertyPath;
		FString ParseError;
		if (!FWidgetPropertyPath::TryParse(FStringView(Binding.TargetPropertyPath), TargetPropertyPath, &ParseError))
		{
			UE_LOG(LogWidgetMarkupPythonIntegration, Warning, TEXT("Binding failed: SourceExpression='%s', Target='%s.%s', Reason='Invalid property path: %s'."), *Binding.SourceExpression, *Binding.TargetObjectName.ToString(), *Binding.TargetPropertyPath, *ParseError);
			Py_RETURN_NONE;
		}

		// Resolve the target property to get its type, so we can convert the Python
		// value directly into an FPropertyBuffer rather than round-tripping through a string.
		TSharedPtr<FPropertyChainHandle> PropertyChain = FPropertyChainHandle::Create(TargetWidget, TargetPropertyPath);
		if (!PropertyChain.IsValid())
		{
			UE_LOG(LogWidgetMarkupPythonIntegration, Warning, TEXT("Binding failed: SourceExpression='%s', Target='%s.%s', Reason='Could not resolve property chain on '%s'.'"),
				*Binding.SourceExpression, *Binding.TargetObjectName.ToString(), *Binding.TargetPropertyPath, *TargetWidget->GetClass()->GetName());
			Py_RETURN_NONE;
		}

		FProperty* TailProperty = PropertyChain->GetTailProperty();
		if (!TailProperty)
		{
			UE_LOG(LogWidgetMarkupPythonIntegration, Warning, TEXT("Binding failed: SourceExpression='%s', Target='%s.%s', Reason='Could not resolve terminal property on '%s'.'"),
				*Binding.SourceExpression, *Binding.TargetObjectName.ToString(), *Binding.TargetPropertyPath, *TargetWidget->GetClass()->GetName());
			Py_RETURN_NONE;
		}

		FPropertyBuffer PropertyBuffer(TailProperty);
		if (!PropertyBuffer.HasValue())
		{
			UE_LOG(LogWidgetMarkupPythonIntegration, Warning, TEXT("Binding failed: SourceExpression='%s', Target='%s.%s', Reason='Could not allocate buffer for property type '%s'.'"),
				*Binding.SourceExpression, *Binding.TargetObjectName.ToString(), *Binding.TargetPropertyPath, *TailProperty->GetClass()->GetName());
			Py_RETURN_NONE;
		}

		if (!NativizePythonValueToProperty(PyValue, TailProperty, PropertyBuffer.GetValueData()))
		{
			FString ExpectedType = TailProperty->GetClass()->GetName();
			if (const FStructProperty* StructProp = CastField<FStructProperty>(TailProperty))
			{
				ExpectedType += FString::Printf(TEXT(" (%s)"), *StructProp->Struct->GetName());
			}
			const FString FallbackString = FPythonUtilities::PythonObjectToString(PyValue);
			UE_LOG(LogWidgetMarkupPythonIntegration, Warning, TEXT("Binding: SourceExpression='%s', Target='%s.%s', Python value could not be converted to '%s', falling back to string."),
				*Binding.SourceExpression, *Binding.TargetObjectName.ToString(), *Binding.TargetPropertyPath, *ExpectedType);

			FPropertyBuffer FallbackBuffer(TailProperty, FStringView(FallbackString));
			if (FallbackBuffer.HasValue())
			{
				PropertyBuffer = MoveTemp(FallbackBuffer);
			}
			else
			{
				UE_LOG(LogWidgetMarkupPythonIntegration, Warning, TEXT("Binding failed: SourceExpression='%s', Target='%s.%s', Reason='String fallback also failed.'"),
					*Binding.SourceExpression, *Binding.TargetObjectName.ToString(), *Binding.TargetPropertyPath);
				Py_RETURN_NONE;
			}
		}

		if (!WidgetMarkupModule->ApplyPropertyValue(TargetWidget, TargetPropertyPath, PropertyBuffer, &Error))
		{
			UE_LOG(LogWidgetMarkupPythonIntegration, Warning, TEXT("Binding failed: SourceExpression='%s', Target='%s.%s', Reason='%s'."), *Binding.SourceExpression, *Binding.TargetObjectName.ToString(), *Binding.TargetPropertyPath, *Error.ToString());
		}
		Py_RETURN_NONE;
	}

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

	PyObject* PyGetPythonObject(PyObject* /*Self*/, PyObject* Args)
	{
		PyObject* PyEntry = nullptr;
		if (!PyArg_ParseTuple(Args, "O:get_python_object", &PyEntry))
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

	PyObject* PyRequestShutdown(PyObject* /*Self*/, PyObject* /*Args*/)
	{
		FPlatformMisc::RequestExit(true);
		Py_RETURN_NONE;
	}

	PyObject* PyGetExtraArguments(PyObject* /*Self*/, PyObject* /*Args*/)
	{
		return PyUnicode_FromString(TCHAR_TO_UTF8(*FWidgetMarkupModule::Get().GetExtraArguments()));
	}

	PyMethodDef NativeModuleMethods[] =
	{
		{ "apply_property_binding", PyApplyPropertyBinding, METH_VARARGS, "Apply one WidgetMarkup property binding with a Python value." },
		{ "find_widget_in_user_widget", PyFindWidgetInUserWidget, METH_VARARGS, "Find a widget by name in a UserWidget's WidgetTree." },
		{ "get_python_object", PyGetPythonObject, METH_VARARGS, "Get the raw Python value from a PythonWidgetMarkupListEntry." },
		{ "get_extra_arguments", PyGetExtraArguments, METH_NOARGS, "Get the current WidgetMarkupApp extra arguments string." },
		{ "request_shutdown", PyRequestShutdown, METH_NOARGS, "Request engine exit (for standalone programs)." },
		{ nullptr, nullptr, 0, nullptr }
	};

	PyModuleDef NativeModuleDefinition =
	{
		PyModuleDef_HEAD_INIT,
		NativeModuleName,
		"Native helpers for WidgetMarkup Python integration.",
		-1,
		NativeModuleMethods,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	};
#endif
}

bool RegisterWidgetMarkupPythonIntegrationNativeModule()
{
#if defined(WITH_PYTHON) && WITH_PYTHON
	if (!Py_IsInitialized())
	{
		UE_LOG(LogWidgetMarkupPythonIntegration, Display, TEXT("WidgetMarkup native Python module registration skipped because Python is not available yet."));
		return false;
	}

	FPythonGILScope GILScope;
	PyObject* PyModules = PyImport_GetModuleDict();
	if (!PyModules)
	{
		return false;
	}

	if (PyDict_GetItemString(PyModules, NativeModuleName))
	{
		return true;
	}

	FPythonAutoRelease PyModule(PyModule_Create(&NativeModuleDefinition));
	if (!PyModule)
	{
		const FString ErrorMessage = FPythonUtilities::ConsumePythonErrorMessage();
		UE_LOG(LogWidgetMarkupPythonIntegration, Warning, TEXT("Failed to register native Python module '%hs': %s"), NativeModuleName, *ErrorMessage);
		return false;
	}

	if (PyDict_SetItemString(PyModules, NativeModuleName, PyModule.Get()) != 0)
	{
		const FString ErrorMessage = FPythonUtilities::ConsumePythonErrorMessage();
		UE_LOG(LogWidgetMarkupPythonIntegration, Warning, TEXT("Failed to expose native Python module '%hs': %s"), NativeModuleName, *ErrorMessage);
		return false;
	}

	return true;
#else
	return false;
#endif
}
