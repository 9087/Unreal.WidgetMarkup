// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Classes/PythonCore.h"

#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "Engine/Blueprint.h"
#include "Extensions/WidgetMarkupUserWidgetExtension.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "PyConversion.h"
#include "PythonUtilities.h"
#include "PythonWidgetMarkupComponent.h"
#include "UObject/UObjectIterator.h"
#include "WidgetMarkupModule.h"

#if defined(WITH_PYTHON) && WITH_PYTHON

// -----------------------------------------------------------------------
// Shared implementation (also used by PythonWidgetLibrary)
// -----------------------------------------------------------------------

UClass* FWidgetMarkupCore::ResolveClass(const FString& Token)
{
	if (Token.IsEmpty())
	{
		return nullptr;
	}

	// 1. TryFindTypeSlow for short names and path-like tokens.
	if (UClass* Class = UClass::TryFindTypeSlow<UClass>(Token, EFindFirstObjectOptions::None))
	{
		return Class;
	}

	// 2. "/Game/..." – WidgetMarkup package path.
	if (Token.StartsWith(TEXT("/")))
	{
		if (FWidgetMarkupModule* Module = FModuleManager::GetModulePtr<FWidgetMarkupModule>(TEXT("WidgetMarkup")))
		{
			if (UObject* Compiled = Module->GetObjectOrCompileFromPackage(Token))
			{
				if (UBlueprint* BP = Cast<UBlueprint>(Compiled))
				{
					return BP->GeneratedClass;
				}
			}
		}
		return nullptr;
	}

	// 3. Dot-notation ("Game.WidgetMarkup.MyWidget").
	if (Token.Contains(TEXT(".")) && !Token.Contains(TEXT("/")))
	{
		const FString PackagePath = TEXT("/") + Token.Replace(TEXT("."), TEXT("/"));

		if (FWidgetMarkupModule* Module = FModuleManager::GetModulePtr<FWidgetMarkupModule>(TEXT("WidgetMarkup")))
		{
			if (UObject* Compiled = Module->GetObjectOrCompileFromPackage(PackagePath))
			{
				if (UBlueprint* BP = Cast<UBlueprint>(Compiled))
				{
					return BP->GeneratedClass;
				}
			}
		}

		const FString AssetName = FPackageName::GetShortName(PackagePath);
		const FString ClassPath = FString::Printf(TEXT("%s.%s_C"), *PackagePath, *AssetName);
		if (UClass* Class = LoadObject<UClass>(nullptr, *ClassPath))
		{
			return Class;
		}

		return nullptr;
	}

	// 4. Case-sensitive exact match via object iterator.
	for (TObjectIterator<UClass> It; It; ++It)
	{
		if (It->GetName().Equals(Token, ESearchCase::CaseSensitive))
		{
			return *It;
		}
	}

	return nullptr;
}

namespace
{

	PyObject* PyResolveClass(PyObject* /*Self*/, PyObject* Args)
	{
		const char* ClassToken = nullptr;
		if (!PyArg_ParseTuple(Args, "s:resolve_class", &ClassToken))
		{
			return nullptr;
		}

		UClass* Resolved = FWidgetMarkupCore::ResolveClass(FString(UTF8_TO_TCHAR(ClassToken)));
		if (!Resolved)
		{
			Py_RETURN_NONE;
		}

		return PyConversion::PythonizeObject(Resolved);
	}

	PyObject* PyGetComponentByWidget(PyObject* /*Self*/, PyObject* Args)
	{
		PyObject* PyWidget = nullptr;
		if (!PyArg_ParseTuple(Args, "O:get_component_by_widget", &PyWidget))
		{
			return nullptr;
		}

		UObject* WidgetObject = nullptr;
		if (!PyConversion::NativizeObject(PyWidget, WidgetObject, UWidget::StaticClass()))
		{
			PyErr_SetString(PyExc_TypeError, "Argument must be a UWidget.");
			return nullptr;
		}

		UWidget* Widget = Cast<UWidget>(WidgetObject);
		if (!Widget)
		{
			Py_RETURN_NONE;
		}

		if (UUserWidget* UserWidget = Cast<UUserWidget>(Widget))
		{
			if (UWidgetMarkupUserWidgetExtension* Extension =
				UWidgetMarkupUserWidgetExtension::GetOrAddExtension(UserWidget))
			{
				if (const TSharedPtr<IWidgetMarkupComponent>& Component = Extension->GetWidgetMarkupComponent())
				{
					if (FPythonWidgetMarkupComponent* PythonComponent =
						static_cast<FPythonWidgetMarkupComponent*>(Component.Get()))
					{
						if (void* PyInstance = PythonComponent->GetPythonInstance())
						{
							PyObject* PyObj = static_cast<PyObject*>(PyInstance);
							Py_INCREF(PyObj);
							return PyObj;
						}
					}
				}
			}
		}

		Py_RETURN_NONE;
	}

	PyMethodDef CoreMethods[] =
	{
		{ "resolve_class", PyResolveClass, METH_VARARGS | METH_STATIC, "Resolve a class token to a UClass (short name, dot-notation, or /Game/ path)." },
		{ "get_component_by_widget", PyGetComponentByWidget, METH_VARARGS | METH_STATIC, "Get the Python WidgetMarkupComponent for a UWidget, or None." },
		{ nullptr, nullptr, 0, nullptr }
	};
}

bool RegisterPythonCoreType(PyObject* Module)
{
	return FPythonUtilities::AddStaticMethodType(
		Module,
		"Core",
		"widget_markup.Core",
		CoreMethods,
		"WidgetMarkup core utilities (class resolution, component lookup).");
}

#endif
