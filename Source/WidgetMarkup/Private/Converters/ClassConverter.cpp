// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "ClassConverter.h"

#include "WidgetMarkupModule.h"
#include "Misc/PackageName.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UnrealType.h"

namespace
{
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

	static bool IsLongBlueprintToken(const FString& Token)
	{
		return Token.Contains(TEXT(".")) && !Token.Contains(TEXT("/"));
	}

	static UClass* ResolveClassByShortName(const FString& Token)
	{
		if (Token.IsEmpty())
		{
			return nullptr;
		}
		if (UClass* Class = UClass::TryFindTypeSlow<UClass>(Token, EFindFirstObjectOptions::None))
		{
			return Class;
		}
		for (TObjectIterator<UClass> It; It; ++It)
		{
			UClass* Class = *It;
			if (Class && Class->GetName().Equals(Token, ESearchCase::CaseSensitive))
			{
				return Class;
			}
		}
		return nullptr;
	}

	static UClass* ResolveClass(const FStringView& TokenView)
	{
		const FString Token = FString(TokenView).TrimStartAndEnd();
		if (Token.IsEmpty())
		{
			return nullptr;
		}
		if (IsLongBlueprintToken(Token))
		{
			const FString ClassPath = ToBlueprintClassPath(Token);
			if (ClassPath.IsEmpty())
			{
				return nullptr;
			}
			return LoadObject<UClass>(nullptr, *ClassPath);
		}
		return ResolveClassByShortName(Token);
	}
}

TSharedRef<FConverter> FClassConverter::Create()
{
	return MakeShared<FClassConverter>();
}

bool FClassConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	const FString SourceToken(String);
	UClass* Class = ResolveClass(String);
	if (!Class)
	{
		UE_LOG(LogWidgetMarkup, Warning, TEXT("ClassConverter failed: cannot resolve class token '%s'."), *SourceToken);
		return false;
	}

	if (const FClassProperty* ClassProperty = CastField<FClassProperty>(&Property))
	{
		if (UClass* MetaClass = ClassProperty->MetaClass; MetaClass && !Class->IsChildOf(MetaClass))
		{
			UE_LOG(LogWidgetMarkup, Warning,
				TEXT("ClassConverter failed: class '%s' is not a child of MetaClass '%s' for property '%s'."),
				*Class->GetPathName(), *MetaClass->GetPathName(), *Property.GetName());
			return false;
		}
		ClassProperty->SetObjectPropertyValue(Data, Class);
		return true;
	}

	if (const FSoftClassProperty* SoftClassProperty = CastField<FSoftClassProperty>(&Property))
	{
		if (UClass* MetaClass = SoftClassProperty->MetaClass; MetaClass && !Class->IsChildOf(MetaClass))
		{
			UE_LOG(LogWidgetMarkup, Warning,
				TEXT("ClassConverter failed: class '%s' is not a child of MetaClass '%s' for property '%s'."),
				*Class->GetPathName(), *MetaClass->GetPathName(), *Property.GetName());
			return false;
		}
		SoftClassProperty->SetPropertyValue(Data, FSoftObjectPtr(Class));
		return true;
	}

	UE_LOG(LogWidgetMarkup, Warning, TEXT("ClassConverter failed: unsupported property class '%s' for property '%s'."),
		*Property.GetClass()->GetName(), *Property.GetName());
	return false;
}
