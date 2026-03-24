// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "TypeResolver.h"

#include <type_traits>

#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "UObject/UObjectIterator.h"
#include "WidgetMarkupModule.h"

namespace
{
	/**
	 * Convert dot-notation package path to BlueprintClass path.
	 * "Game.WidgetMarkup.MyWidget" → "/Game/WidgetMarkup/MyWidget.MyWidget_C"
	 */
	static FString ToBlueprintClassPath(const FString& Token)
	{
		const FString AssetPath = TEXT("/") + Token.Replace(TEXT("."), TEXT("/"));
		const FString AssetName = FPackageName::GetShortName(AssetPath);
		if (AssetName.IsEmpty())
		{
			return FString();
		}
		return FString::Printf(TEXT("%s.%s_C"), *AssetPath, *AssetName);
	}

	static FString ToAssetObjectPath(const FString& Token)
	{
		const FString AssetPath = TEXT("/") + Token.Replace(TEXT("."), TEXT("/"));
		const FString AssetName = FPackageName::GetShortName(AssetPath);
		if (AssetName.IsEmpty())
		{
			return FString();
		}
		return FString::Printf(TEXT("%s.%s"), *AssetPath, *AssetName);
	}

	/**
	 * Check if token is long-form (dot notation, not a path)
	 */
	static bool IsLongBlueprintToken(const FString& Token)
	{
		return Token.Contains(TEXT(".")) && !Token.Contains(TEXT("/"));
	}

	/**
	 * Common resolve logic for long-form tokens.
	 * Returns result from WidgetMarkup compilation or standard loading if successful, nullptr otherwise.
	 */
	template<typename T>
	static T* ResolveLongToken(const FString& Token)
	{
		const FString PackagePath = TEXT("/") + Token.Replace(TEXT("."), TEXT("/"));

		// Try WidgetMarkup first
		if (auto WidgetMarkupModule = FModuleManager::GetModulePtr<FWidgetMarkupModule>("WidgetMarkup"))
		{
			if (UObject* CompiledObject = WidgetMarkupModule->GetObjectOrCompileFromPackage(PackagePath))
			{
				if (T* CompiledType = Cast<T>(CompiledObject))
				{
					return CompiledType;
				}
			}
		}

		if constexpr (std::is_same_v<T, UClass>)
		{
			const FString ClassPath = ToBlueprintClassPath(Token);
			if (ClassPath.IsEmpty())
			{
				return nullptr;
			}
			return LoadObject<UClass>(nullptr, *ClassPath);
		}

		if constexpr (std::is_same_v<T, UStruct>)
		{
			const FString StructPath = ToAssetObjectPath(Token);
			if (StructPath.IsEmpty())
			{
				return nullptr;
			}
			if (UStruct* Struct = LoadObject<UStruct>(nullptr, *StructPath))
			{
				return Struct->IsA<UClass>() ? nullptr : Struct;
			}
			return nullptr;
		}

		// Unsupported resolver type for long-token loading.
		return nullptr;
	}
}

// Generic ResolveShortName implementation for all types
template<typename T>
T* TTypeResolver<T>::ResolveShortName(const FString& Token)
{
	if (Token.IsEmpty())
	{
		return nullptr;
	}

	// First try TryFindTypeSlow
	if (T* Type = UClass::TryFindTypeSlow<T>(Token, EFindFirstObjectOptions::None))
	{
		if constexpr (std::is_same_v<T, UStruct>)
		{
			if (Type->IsA<UClass>())
			{
				return nullptr;
			}
		}
		return Type;
	}

	// Fall back to iterator with case-sensitive exact match
	for (TObjectIterator<T> It; It; ++It)
	{
		T* Type = *It;
		if constexpr (std::is_same_v<T, UStruct>)
		{
			if (Type && Type->IsA<UClass>())
			{
				continue;
			}
		}
		if (Type && Type->GetName().Equals(Token, ESearchCase::CaseSensitive))
		{
			return Type;
		}
	}
	return nullptr;
}

// Generic Resolve implementation for all types
template<typename T>
T* TTypeResolver<T>::Resolve(const FStringView& TokenView)
{
	const FString Token = FString(TokenView).TrimStartAndEnd();
	if (Token.IsEmpty())
	{
		return nullptr;
	}

	// Try long-form token first
	if (IsLongBlueprintToken(Token))
	{
		if (T* Result = ResolveLongToken<T>(Token))
		{
			return Result;
		}
	}

	// Fall back to short-name resolution
	return ResolveShortName(Token);
}

// Explicit instantiation for UClass and UStruct
template class TTypeResolver<UClass>;
template class TTypeResolver<UStruct>;

