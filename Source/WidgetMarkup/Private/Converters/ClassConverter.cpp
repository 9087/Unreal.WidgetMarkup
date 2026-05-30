#include "ClassConverter.h"

#include "WidgetBlueprint.h"
#include "WidgetMarkupModule.h"
#include "Utilities/TypeResolver.h"
#include "UObject/UnrealType.h"
#include "Modules/ModuleManager.h"

TSharedRef<FConverter> FClassConverter::Create()
{
	return MakeShared<FClassConverter>();
}

bool FClassConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	const FString SourceToken(String);
	UClass* Class = TTypeResolver<UClass>::Resolve(String);

	// If standard resolution fails and the token looks like a WidgetMarkup package
	// path, compile the markup and use its generated class.
	if (!Class && SourceToken.StartsWith(TEXT("/")))
	{
		if (FWidgetMarkupModule* Module = FModuleManager::GetModulePtr<FWidgetMarkupModule>(TEXT("WidgetMarkup")))
		{
			if (UObject* Compiled = Module->GetObjectOrCompileFromPackage(SourceToken))
			{
				if (UWidgetBlueprint* WidgetBP = Cast<UWidgetBlueprint>(Compiled))
				{
					Class = WidgetBP->GeneratedClass;
				}
			}
		}
	}

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
