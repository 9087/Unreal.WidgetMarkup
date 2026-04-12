// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "ElementNode.h"
#include "PropertyRun.h"
#include "Utilities/WidgetPropertyPath.h"

WIDGETMARKUP_API DECLARE_LOG_CATEGORY_EXTERN(LogWidgetMarkup, Log, All);

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

	DECLARE_DELEGATE_RetVal(TSharedRef<IPropertyRun>, FOnCreatePropertyRun);
	bool RegisterCustomPropertyRun(UStruct* InStruct, FName InPropertyPath, FOnCreatePropertyRun InOnCreatePropertyRun);
	void UnregisterCustomPropertyRun(UStruct* InStruct, FName InPropertyPath);

	template <typename T>
	bool RegisterCustomPropertyRun(FName InPropertyPath, FOnCreatePropertyRun InOnCreatePropertyRun)
	{
		if constexpr (TIsDerivedFrom<T, UObject>::Value)
		{
			return RegisterCustomProperty(T::StaticClass(), InPropertyPath, InOnCreatePropertyRun);
		}
		else
		{
			return RegisterCustomProperty(T::StaticStruct(), InPropertyPath, InOnCreatePropertyRun);
		}
	}

	template <typename T>
	void UnregisterCustomPropertyRun(FName InPropertyPath)
	{
		if constexpr (TIsDerivedFrom<T, UObject>::Value)
		{
			UnregisterCustomProperty(T::StaticClass(), InPropertyPath);
		}
		else
		{
			UnregisterCustomProperty(T::StaticStruct(), InPropertyPath);
		}
	}

	TSharedPtr<IPropertyRun> CreateCustomPropertyRun(UStruct* InStruct, FName InPropertyPath) const;

private:
	UObject* CompileFromSourceCode(FName PackagePath, const FString& XML);

	TMap<FName, TObjectPtr<UObject>> Objects;
	FOnObjectCompiled OnObjectCompiled;

	/** Custom properties: keyed by UStruct* (element type), then exact canonical property path to descriptor. */
	TMap<TWeakObjectPtr<UStruct>, TMap<FWidgetPropertyPath, FOnCreatePropertyRun>> PropertyRunCreateDelegates;

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
