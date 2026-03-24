#include "ClassConverter.h"

#include "WidgetMarkupModule.h"
#include "Utilities/TypeResolver.h"
#include "UObject/UnrealType.h"

TSharedRef<FConverter> FClassConverter::Create()
{
	return MakeShared<FClassConverter>();
}

bool FClassConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	const FString SourceToken(String);
	UClass* Class = TTypeResolver<UClass>::Resolve(String);
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
