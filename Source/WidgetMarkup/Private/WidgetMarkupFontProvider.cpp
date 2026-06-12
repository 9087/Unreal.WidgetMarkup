// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "WidgetMarkupFontProvider.h"
#include "WidgetMarkupModule.h"
#include "Fonts/CompositeFont.h"
#include "Fonts/SlateFontInfo.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformMisc.h"
#include "Misc/Paths.h"

const FCompositeFont* UWidgetMarkupFontProvider::GetCompositeFont() const
{
	return CompositeFont.Get();
}

void UWidgetMarkupFontProvider::InitializeFromPath(const FString& InPath)
{
	CompositeFont = MakeShared<FCompositeFont>();
	CompositeFont->DefaultTypeface.AppendFont(NAME_None, InPath, EFontHinting::Default, EFontLoadingPolicy::Stream);
}

UWidgetMarkupFontProvider* UWidgetMarkupFontProvider::CreateFromFontName(const FProperty& Property, const FString& InFontName)
{
	// Only handle FSlateFontInfo::FontObject, not every ObjectProperty.
	if (Property.GetFName() != FName(TEXT("FontObject")) ||
		Property.GetOwnerStruct() != FSlateFontInfo::StaticStruct())
	{
		return nullptr;
	}
	const bool bLooksLikePath = InFontName.Contains(TEXT("/")) || InFontName.Contains(TEXT("\\")) ||
		InFontName.EndsWith(TEXT(".ttf")) || InFontName.EndsWith(TEXT(".otf"));

	FString ResolvedPath;

	if (bLooksLikePath)
	{
		if (IFileManager::Get().FileExists(*InFontName))
		{
			ResolvedPath = InFontName;
		}
		else
		{
			ResolvedPath = FPaths::Combine(FPaths::LaunchDir(), *InFontName);
			FPaths::NormalizeFilename(ResolvedPath);
			if (!IFileManager::Get().FileExists(*ResolvedPath))
			{
				return nullptr;
			}
		}
	}
	else
	{
#if PLATFORM_WINDOWS
		for (const TCHAR* Ext : { TEXT(".ttf"), TEXT(".otf") })
		{
			const FString FileName = InFontName + Ext;
			ResolvedPath = FPaths::Combine(FPlatformMisc::GetEnvironmentVariable(TEXT("SystemRoot")), TEXT("Fonts"), FileName);
			if (IFileManager::Get().FileExists(*ResolvedPath)) break;
			ResolvedPath = FPaths::Combine(FPlatformMisc::GetEnvironmentVariable(TEXT("LocalAppData")),
				TEXT("Microsoft"), TEXT("Windows"), TEXT("Fonts"), FileName);
			if (IFileManager::Get().FileExists(*ResolvedPath)) break;
			ResolvedPath.Empty();
		}
#elif PLATFORM_MAC
		for (const TCHAR* Ext : { TEXT(".ttf"), TEXT(".otf") })
		{
			const FString FileName = InFontName + Ext;
			ResolvedPath = FPaths::Combine(TEXT("/System/Library/Fonts"), FileName);
			if (IFileManager::Get().FileExists(*ResolvedPath)) break;
			ResolvedPath = FPaths::Combine(TEXT("/Library/Fonts"), FileName);
			if (IFileManager::Get().FileExists(*ResolvedPath)) break;
			ResolvedPath.Empty();
		}
#elif PLATFORM_LINUX
		for (const TCHAR* Ext : { TEXT(".ttf"), TEXT(".otf") })
		{
			const FString FileName = InFontName + Ext;
			ResolvedPath = FPaths::Combine(TEXT("/usr/share/fonts/truetype"), FileName);
			if (IFileManager::Get().FileExists(*ResolvedPath)) break;
			ResolvedPath.Empty();
		}
#endif
		if (ResolvedPath.IsEmpty())
		{
			UE_LOG(LogWidgetMarkup, Warning, TEXT("FontProvider: system font '%s' not found on disk."), *InFontName);
			return nullptr;
		}
	}

	UWidgetMarkupFontProvider* Provider = NewObject<UWidgetMarkupFontProvider>(GetTransientPackage(), NAME_None, RF_Public | RF_Standalone);
	if (Provider)
	{
		Provider->InitializeFromPath(ResolvedPath);
	}
	return Provider;
}
