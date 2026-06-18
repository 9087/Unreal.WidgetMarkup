// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "WidgetMarkupFontProvider.h"
#include "WidgetMarkupModule.h"
#include "Fonts/CompositeFont.h"
#include "Fonts/SlateFontInfo.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformMisc.h"
#include "Misc/Paths.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"

/** Look up a font display name in the Windows Registry to find its filename. */
static FString LookupFontFileNameFromRegistry(const FString& InFontName)
{
	static const LPCTSTR kFontsRegKey = TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");

	HKEY hKey = nullptr;
	if (::RegOpenKeyExW(HKEY_LOCAL_MACHINE, kFontsRegKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return FString();
	}

	FString Result;
	DWORD dwIndex = 0;
	WCHAR szValueName[256];
	WCHAR szValueData[MAX_PATH];
	DWORD cchValueName;
	DWORD cbValueData;
	DWORD dwType;

	while (true)
	{
		cchValueName = UE_ARRAY_COUNT(szValueName);
		cbValueData   = sizeof(szValueData);
		const LONG lRet = ::RegEnumValueW(hKey, dwIndex, szValueName, &cchValueName,
			nullptr, &dwType, reinterpret_cast<LPBYTE>(szValueData), &cbValueData);
		if (lRet == ERROR_NO_MORE_ITEMS) break;
		if (lRet != ERROR_SUCCESS) { ++dwIndex; continue; }

		FString DisplayName(szValueName);

		// Strip common suffixes: " (TrueType)", " (OpenType)"
		static const TCHAR* kSuffixes[] = {
			TEXT(" (TrueType)"), TEXT(" (OpenType)"),
			TEXT(" (TrueType Collection)"), TEXT(" (OpenType Collection)"),
		};
		for (const TCHAR* Suffix : kSuffixes)
		{
			const int32 Len = FCString::Strlen(Suffix);
			if (DisplayName.EndsWith(Suffix))
			{
				DisplayName.LeftChopInline(Len);
				break;
			}
		}

		if (DisplayName.Equals(InFontName, ESearchCase::IgnoreCase))
		{
			Result = FString(szValueData);
			break;
		}

		++dwIndex;
	}

	::RegCloseKey(hKey);
	return Result;
}
#endif // PLATFORM_WINDOWS

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
		// Try Windows Registry to map display name → filename (e.g. "Segoe UI" → "segoeui.ttf")
		const FString ResolvedFileName = LookupFontFileNameFromRegistry(InFontName);
		const FString& SearchName = ResolvedFileName.IsEmpty() ? InFontName : ResolvedFileName;
		// If Registry returned a filename that already has an extension, use it directly
		const bool bSearchNameHasExt = SearchName.EndsWith(TEXT(".ttf")) || SearchName.EndsWith(TEXT(".otf"));
#else
		const FString& SearchName = InFontName;
		const bool bSearchNameHasExt = false;
#endif

		// If the name already has an extension (e.g. from Registry), use it directly;
		// otherwise try .ttf and .otf extensions.
		{
			// Build font directory list (use FString to avoid dangling pointer from temporary)
			TArray<FString> FontDirs;
			FontDirs.Add(FPaths::Combine(FPlatformMisc::GetEnvironmentVariable(TEXT("SystemRoot")), TEXT("Fonts")));
#if PLATFORM_WINDOWS
			FontDirs.Add(FPaths::Combine(FPlatformMisc::GetEnvironmentVariable(TEXT("LocalAppData")),
				TEXT("Microsoft"), TEXT("Windows"), TEXT("Fonts")));
#elif PLATFORM_MAC
			FontDirs.Add(TEXT("/System/Library/Fonts"));
			FontDirs.Add(TEXT("/Library/Fonts"));
#elif PLATFORM_LINUX
			FontDirs.Add(TEXT("/usr/share/fonts/truetype"));
#endif

			if (bSearchNameHasExt)
			{
				for (const FString& FontDir : FontDirs)
				{
					ResolvedPath = FPaths::Combine(FontDir, SearchName);
					if (IFileManager::Get().FileExists(*ResolvedPath)) break;
					ResolvedPath.Empty();
				}
			}
			else
			{
				const TCHAR* Extensions[] = { TEXT(".ttf"), TEXT(".otf") };
				bool bFound = false;
				for (const TCHAR* Ext : Extensions)
				{
					const FString FileName = SearchName + Ext;
					for (const FString& FontDir : FontDirs)
					{
						ResolvedPath = FPaths::Combine(FontDir, FileName);
						if (IFileManager::Get().FileExists(*ResolvedPath)) { bFound = true; break; }
						ResolvedPath.Empty();
					}
					if (bFound) break;
				}
			}
		}
		if (ResolvedPath.IsEmpty())
		{
			UE_LOG(LogWidgetMarkup, Warning, TEXT("FontProvider: system font '%s' not found on disk."), *SearchName);
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
