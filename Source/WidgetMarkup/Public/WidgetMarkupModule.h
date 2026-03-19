// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "ElementNode.h"

WIDGETMARKUP_API DECLARE_LOG_CATEGORY_EXTERN(LogWidgetMarkup, Log, All);

/** Delegate for applying a custom attribute: (Context, TargetObject, TypeName, AttributeValue) -> FResult. */
DECLARE_DELEGATE_RetVal_FourParams(FElementNode::FResult, FOnApplyCustomAttribute, FElementNode::FContext& /* Context */, UObject* /* TargetObject */, FName /* TypeName */, const FStringView& /* AttributeValue */);

/** Descriptor for a custom attribute: type name (FConverterRegistry key) and apply delegate. */
struct FCustomAttributeDescriptor
{
	FName TypeName;
	FOnApplyCustomAttribute ApplyDelegate;
};

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

	/** Register a custom attribute for the given Struct (UClass* or UStruct*). Overwrites and logs warning if (Struct, AttributeName) already exists. */
	bool RegisterCustomAttribute(UStruct* Struct, FName AttributeName, FName TypeName, FOnApplyCustomAttribute InOnApplyCustomAttribute);
	/** Unregister a custom attribute. */
	void UnregisterCustomAttribute(UStruct* Struct, FName AttributeName);

	template <typename T>
	bool RegisterCustomAttribute(FName AttributeName, FName TypeName, FOnApplyCustomAttribute InOnApplyCustomAttribute)
	{
		if constexpr (TIsDerivedFrom<T, UObject>::Value)
		{
			return RegisterCustomAttribute(T::StaticClass(), AttributeName, TypeName, InOnApplyCustomAttribute);
		}
		else
		{
			return RegisterCustomAttribute(T::StaticStruct(), AttributeName, TypeName, InOnApplyCustomAttribute);
		}
	}

	template <typename T>
	void UnregisterCustomAttribute(FName AttributeName)
	{
		if constexpr (TIsDerivedFrom<T, UObject>::Value)
		{
			UnregisterCustomAttribute(T::StaticClass(), AttributeName);
		}
		else
		{
			UnregisterCustomAttribute(T::StaticStruct(), AttributeName);
		}
	}

	/** Find custom attribute descriptor for the given Struct and attribute name (best-match by Struct). Returns true if found. */
	bool FindCustomAttributeDescriptor(UStruct* Struct, FName AttributeName, FCustomAttributeDescriptor& OutDescriptor) const;

private:
	static FString ToAbsolutePath(const FString& SourceFilePath);
	static bool ConvertFilePathToObjectPath(const FString& FilePath, FString& OutObjectPath);

	TMap<FName, TObjectPtr<UObject>> Objects;
	FOnObjectCompiled OnObjectCompiled;

	/** Custom attributes: keyed by UStruct* (element type), then FName (attribute name). */
	TMap<TWeakObjectPtr<UStruct>, TMap<FName, FCustomAttributeDescriptor>> CustomAttributes;

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
