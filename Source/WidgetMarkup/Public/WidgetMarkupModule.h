// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "ElementNode.h"
#include "PropertyRun.h"
#include "PropertySetter.h"
#include "Utilities/WidgetPropertyPath.h"

WIDGETMARKUP_API DECLARE_LOG_CATEGORY_EXTERN(LogWidgetMarkup, Log, All);

class UUserWidget;
class UWidgetMarkupBlueprintGeneratedClassExtension;

class FWidgetMarkupModule : public IModuleInterface, public FGCObject
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	UObject* CompileFromPackagePath(const FString& PackagePath);
	UObject* GetObjectFromPackagePath(const FString& PackagePath);
	UObject* GetObjectOrCompileFromPackage(const FString& PackagePath);

	template <typename T>
	T* CompileFromPackagePath(const FString& PackagePath)
	{
		return Cast<T>(CompileFromPackagePath(PackagePath));
	}

	template <typename T>
	T* GetObjectFromPackagePath(const FString& PackagePath)
	{
		return Cast<T>(GetObjectFromPackagePath(PackagePath));
	}

	template <typename T>
	T* GetObjectOrCompileFromPackage(const FString& PackagePath)
	{
		return Cast<T>(GetObjectOrCompileFromPackage(PackagePath));
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnObjectCompiled, FName, UObject*)
	FOnObjectCompiled& GetOnObjectCompiled();

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWidgetMarkupUserWidgetInitialized, UUserWidget*, UWidgetMarkupBlueprintGeneratedClassExtension*)
	FOnWidgetMarkupUserWidgetInitialized& GetOnWidgetMarkupUserWidgetInitialized();

	DECLARE_DELEGATE_RetVal(TSharedRef<IPropertyRun>, FOnCreatePropertyRun);
	bool RegisterCustomPropertyRun(UStruct* InStruct, FName InPropertyPath, FOnCreatePropertyRun InOnCreatePropertyRun);
	void UnregisterCustomPropertyRun(UStruct* InStruct, FName InPropertyPath);

	template <typename T>
	bool RegisterCustomPropertyRun(FName InPropertyPath, FOnCreatePropertyRun InOnCreatePropertyRun)
	{
		if constexpr (TIsDerivedFrom<T, UObject>::Value)
		{
			return RegisterCustomPropertyRun(T::StaticClass(), InPropertyPath, InOnCreatePropertyRun);
		}
		else
		{
			return RegisterCustomPropertyRun(T::StaticStruct(), InPropertyPath, InOnCreatePropertyRun);
		}
	}

	template <typename T>
	void UnregisterCustomPropertyRun(FName InPropertyPath)
	{
		if constexpr (TIsDerivedFrom<T, UObject>::Value)
		{
			UnregisterCustomPropertyRun(T::StaticClass(), InPropertyPath);
		}
		else
		{
			UnregisterCustomPropertyRun(T::StaticStruct(), InPropertyPath);
		}
	}

	TSharedPtr<IPropertyRun> CreateCustomPropertyRun(UStruct* InStruct, FName InPropertyPath) const;
	TSharedRef<IPropertyRun> CreatePropertyRun(UStruct* InStruct, FName InPropertyPath) const;

	DECLARE_DELEGATE_RetVal(TSharedRef<FPropertySetter>, FOnCreatePropertySetter);
	bool RegisterCustomPropertySetter(UStruct* InStruct, FName InPropertyPath, FOnCreatePropertySetter InOnCreatePropertySetter);
	void UnregisterCustomPropertySetter(UStruct* InStruct, FName InPropertyPath);

	template <typename T>
	bool RegisterCustomPropertySetter(FName InPropertyPath, FOnCreatePropertySetter InOnCreatePropertySetter)
	{
		if constexpr (TIsDerivedFrom<T, UObject>::Value)
		{
			return RegisterCustomPropertySetter(T::StaticClass(), InPropertyPath, InOnCreatePropertySetter);
		}
		else
		{
			return RegisterCustomPropertySetter(T::StaticStruct(), InPropertyPath, InOnCreatePropertySetter);
		}
	}

	template <typename T>
	void UnregisterCustomPropertySetter(FName InPropertyPath)
	{
		if constexpr (TIsDerivedFrom<T, UObject>::Value)
		{
			UnregisterCustomPropertySetter(T::StaticClass(), InPropertyPath);
		}
		else
		{
			UnregisterCustomPropertySetter(T::StaticStruct(), InPropertyPath);
		}
	}

	TSharedPtr<FPropertySetter> CreateCustomPropertySetter(UStruct* InStruct, FName InPropertyPath) const;

private:
	UObject* CompileFromSourceCode(FName PackagePath, const FString& XML);

	TMap<FName, TObjectPtr<UObject>> Objects;
	FOnObjectCompiled OnObjectCompiled;
	FOnWidgetMarkupUserWidgetInitialized OnWidgetMarkupUserWidgetInitialized;

	/** Custom properties: keyed by UStruct* (element type), then exact canonical property path to descriptor. */
	TMap<TWeakObjectPtr<UStruct>, TMap<FWidgetPropertyPath, FOnCreatePropertyRun>> PropertyRunCreateDelegates;
	/** Custom property setters: keyed by UStruct* (element type), then exact canonical property path to setter factory. */
	TMap<TWeakObjectPtr<UStruct>, TMap<FWidgetPropertyPath, FOnCreatePropertySetter>> PropertySetterCreateDelegates;

public:
	void OnPostEngineInit();
	void StartSourceFileWatching(const FDirectoryPath& InDirectoryPath);
	void StopSourceFileWatching();

private:
	void HandleOnSourceFileDirectoryChanged(const TArray<struct FFileChangeData>& FileChanges, const FString& WatchedDirectory);

	TMap<FString, FName> SourceFileToName;
	/** Maps absolute watched directory path -> delegate handle. Supports multiple watched directories. */
	TMap<FString, FDelegateHandle> WatchedDirectories;
};
