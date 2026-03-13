// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"

WIDGETMARKUP_API DECLARE_LOG_CATEGORY_EXTERN(LogWidgetMarkup, Log, All);

class FWidgetMarkupModule : public IModuleInterface, public FGCObject
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	UObject* CompileFromSourceCode(FName Name, const FString& XML);
	UObject* CompileFromFile(const FString& SourceFilePath);
	UObject* GetObjectFromName(FName Name);
	UObject* GetObjectFromFile(const FString& SourceFilePath);

	template <typename T>
	T* CompileFromSourceCode(FName Name, const FString& XML)
	{
		return Cast<T>(CompileFromSourceCode(Name, XML));
	}

	template <typename T>
	T* CompileFromFile(const FString& SourceFilePath)
	{
		return Cast<T>(CompileFromFile(SourceFilePath));
	}

	template <typename T>
	T* GetObjectFromName(FName Name)
	{
		return Cast<T>(GetObjectFromName(Name));
	}

	template <typename T>
	T* GetObjectFromFile(const FString& SourceFilePath)
	{
		return Cast<T>(GetObjectFromFile(SourceFilePath));
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnObjectCompiled, FName, UObject*)
	FOnObjectCompiled& GetOnObjectCompiled();

private:
	static FString ToAbsolutePath(const FString& SourceFilePath);
	static bool ConvertFilePathToObjectPath(const FString& FilePath, FString& OutObjectPath);

	TMap<FName, TObjectPtr<UObject>> Objects;
	FOnObjectCompiled OnObjectCompiled;

public:
	void OnPostEngineInit();
	void StartSourceFileWatching(const FDirectoryPath& InDirectoryPath);
	void StopSourceFileWatching();

private:
	void HandleOnSourceFileDirectoryChanged(const TArray<struct FFileChangeData>& FileChanges);

	TMap<FString, FName> SourceFileToName;
	bool bSourceFileWatchingStarted = false;
	FString SourceFileWatchingDirectoryPath;
	FDelegateHandle SourceFileWatchingDelegateHandle;
};
