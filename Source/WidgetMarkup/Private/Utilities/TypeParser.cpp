// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "TypeParser.h"

#include "EdGraphSchema_K2.h"
#include "UObject/Interface.h"
#include "UObject/UObjectIterator.h"

namespace
{
	bool StartsWithNoCase(const FString& Text, const TCHAR* Prefix)
	{
		return Text.StartsWith(Prefix, ESearchCase::IgnoreCase);
	}

	FName ToPinCategory(const FString& TypeToken)
	{
		if (TypeToken.Equals(TEXT("Bool"), ESearchCase::IgnoreCase)) return UEdGraphSchema_K2::PC_Boolean;
		if (TypeToken.Equals(TEXT("Integer"), ESearchCase::IgnoreCase)) return UEdGraphSchema_K2::PC_Int;
		if (TypeToken.Equals(TEXT("Int64"), ESearchCase::IgnoreCase)) return UEdGraphSchema_K2::PC_Int64;
		if (TypeToken.Equals(TEXT("Float"), ESearchCase::IgnoreCase)) return UEdGraphSchema_K2::PC_Float;
		if (TypeToken.Equals(TEXT("Double"), ESearchCase::IgnoreCase)) return UEdGraphSchema_K2::PC_Double;
		if (TypeToken.Equals(TEXT("Byte"), ESearchCase::IgnoreCase)) return UEdGraphSchema_K2::PC_Byte;
		if (TypeToken.Equals(TEXT("String"), ESearchCase::IgnoreCase)) return UEdGraphSchema_K2::PC_String;
		if (TypeToken.Equals(TEXT("Text"), ESearchCase::IgnoreCase)) return UEdGraphSchema_K2::PC_Text;
		if (TypeToken.Equals(TEXT("Name"), ESearchCase::IgnoreCase)) return UEdGraphSchema_K2::PC_Name;
		if (TypeToken.Equals(TEXT("Vector"), ESearchCase::IgnoreCase)) return UEdGraphSchema_K2::PC_Struct;
		if (TypeToken.Equals(TEXT("Vector2D"), ESearchCase::IgnoreCase)) return UEdGraphSchema_K2::PC_Struct;
		if (TypeToken.Equals(TEXT("Rotator"), ESearchCase::IgnoreCase)) return UEdGraphSchema_K2::PC_Struct;
		if (TypeToken.Equals(TEXT("Transform"), ESearchCase::IgnoreCase)) return UEdGraphSchema_K2::PC_Struct;
		if (TypeToken.Equals(TEXT("Color"), ESearchCase::IgnoreCase)) return UEdGraphSchema_K2::PC_Struct;
		if (TypeToken.Equals(TEXT("LinearColor"), ESearchCase::IgnoreCase)) return UEdGraphSchema_K2::PC_Struct;
		return NAME_None;
	}

	UScriptStruct* ResolveBuiltinStruct(const FString& TypeToken)
	{
		if (TypeToken.Equals(TEXT("Vector"), ESearchCase::IgnoreCase)) return TBaseStructure<FVector>::Get();
		if (TypeToken.Equals(TEXT("Vector2D"), ESearchCase::IgnoreCase)) return TBaseStructure<FVector2D>::Get();
		if (TypeToken.Equals(TEXT("Rotator"), ESearchCase::IgnoreCase)) return TBaseStructure<FRotator>::Get();
		if (TypeToken.Equals(TEXT("Transform"), ESearchCase::IgnoreCase)) return TBaseStructure<FTransform>::Get();
		if (TypeToken.Equals(TEXT("Color"), ESearchCase::IgnoreCase)) return TBaseStructure<FColor>::Get();
		if (TypeToken.Equals(TEXT("LinearColor"), ESearchCase::IgnoreCase)) return TBaseStructure<FLinearColor>::Get();
		return nullptr;
	}

	FEdGraphTerminalType ToTerminalType(const FEdGraphPinType& InPinType)
	{
		return FEdGraphTerminalType::FromPinType(InPinType);
	}
}

bool FTypeParser::ParseType(const FString& InTypeText, FEdGraphPinType& OutPinType, FString& OutError)
{
	return ParseTypeInternal(InTypeText, OutPinType, OutError, true);
}

UClass* FTypeParser::ResolveClass(const FString& InClassText)
{
	const FString Token = NormalizeToken(InClassText);
	if (Token.IsEmpty())
	{
		return nullptr;
	}

	if (UClass* Class = UClass::TryFindTypeSlow<UClass>(Token, EFindFirstObjectOptions::None))
	{
		return Class;
	}

	const FString PrefixedToken = Token.StartsWith(TEXT("U"), ESearchCase::CaseSensitive) ? Token : FString(TEXT("U")) + Token;
	if (UClass* Class = UClass::TryFindTypeSlow<UClass>(PrefixedToken, EFindFirstObjectOptions::None))
	{
		return Class;
	}

	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;
		if (!Class)
		{
			continue;
		}
		const FString ClassName = Class->GetName();
		if (ClassName.Equals(Token, ESearchCase::IgnoreCase))
		{
			return Class;
		}
		if (ClassName.Len() > 1 && ClassName[0] == TCHAR('U') && ClassName.RightChop(1).Equals(Token, ESearchCase::IgnoreCase))
		{
			return Class;
		}
	}

	return nullptr;
}

UClass* FTypeParser::ResolveInterface(const FString& InInterfaceText)
{
	UClass* Class = ResolveClass(InInterfaceText);
	if (!Class || !Class->IsChildOf(UInterface::StaticClass()))
	{
		return nullptr;
	}
	return Class;
}

UScriptStruct* FTypeParser::ResolveStruct(const FString& InStructText)
{
	const FString Token = NormalizeToken(InStructText);
	if (Token.IsEmpty())
	{
		return nullptr;
	}

	if (UScriptStruct* Struct = UClass::TryFindTypeSlow<UScriptStruct>(Token, EFindFirstObjectOptions::None))
	{
		return Struct;
	}

	const FString PrefixedToken = Token.StartsWith(TEXT("F"), ESearchCase::CaseSensitive) ? Token : FString(TEXT("F")) + Token;
	if (UScriptStruct* Struct = UClass::TryFindTypeSlow<UScriptStruct>(PrefixedToken, EFindFirstObjectOptions::None))
	{
		return Struct;
	}

	for (TObjectIterator<UScriptStruct> It; It; ++It)
	{
		UScriptStruct* Struct = *It;
		if (!Struct)
		{
			continue;
		}
		const FString StructName = Struct->GetName();
		if (StructName.Equals(Token, ESearchCase::IgnoreCase))
		{
			return Struct;
		}
		if (StructName.Len() > 1 && StructName[0] == TCHAR('F') && StructName.RightChop(1).Equals(Token, ESearchCase::IgnoreCase))
		{
			return Struct;
		}
	}

	return nullptr;
}

UEnum* FTypeParser::ResolveEnum(const FString& InEnumText)
{
	const FString Token = NormalizeToken(InEnumText);
	if (Token.IsEmpty())
	{
		return nullptr;
	}

	if (UEnum* Enum = UClass::TryFindTypeSlow<UEnum>(Token, EFindFirstObjectOptions::None))
	{
		return Enum;
	}

	const FString PrefixedToken = Token.StartsWith(TEXT("E"), ESearchCase::CaseSensitive) ? Token : FString(TEXT("E")) + Token;
	if (UEnum* Enum = UClass::TryFindTypeSlow<UEnum>(PrefixedToken, EFindFirstObjectOptions::None))
	{
		return Enum;
	}

	for (TObjectIterator<UEnum> It; It; ++It)
	{
		UEnum* Enum = *It;
		if (!Enum)
		{
			continue;
		}
		const FString EnumName = Enum->GetName();
		if (EnumName.Equals(Token, ESearchCase::IgnoreCase))
		{
			return Enum;
		}
		if (EnumName.Len() > 1 && EnumName[0] == TCHAR('E') && EnumName.RightChop(1).Equals(Token, ESearchCase::IgnoreCase))
		{
			return Enum;
		}
	}

	return nullptr;
}

bool FTypeParser::ParseTypeInternal(const FString& InTypeText, FEdGraphPinType& OutPinType, FString& OutError, bool bAllowContainer)
{
	const FString TypeText = NormalizeToken(InTypeText);
	if (TypeText.IsEmpty())
	{
		OutError = TEXT("Type must not be empty.");
		return false;
	}

	FString ContainerName;
	FString InnerText;
	if (ParseContainer(TypeText, ContainerName, InnerText, OutError))
	{
		if (!bAllowContainer)
		{
			OutError = FString::Printf(TEXT("Container nesting is not supported: '%s'"), *TypeText);
			return false;
		}

		if (ContainerName.Equals(TEXT("Array"), ESearchCase::IgnoreCase) || ContainerName.Equals(TEXT("Set"), ESearchCase::IgnoreCase))
		{
			FEdGraphPinType InnerPinType;
			if (!ParseTypeInternal(InnerText, InnerPinType, OutError, false))
			{
				return false;
			}
			OutPinType = InnerPinType;
			OutPinType.ContainerType = ContainerName.Equals(TEXT("Array"), ESearchCase::IgnoreCase)
				? EPinContainerType::Array
				: EPinContainerType::Set;
			return true;
		}

		if (ContainerName.Equals(TEXT("Map"), ESearchCase::IgnoreCase))
		{
			FString KeyText;
			FString ValueText;
			if (!ParseMapContainer(InnerText, KeyText, ValueText, OutError))
			{
				return false;
			}

			FEdGraphPinType KeyPinType;
			if (!ParseTypeInternal(KeyText, KeyPinType, OutError, false))
			{
				return false;
			}
			if (KeyPinType.IsContainer())
			{
				OutError = FString::Printf(TEXT("Map key type cannot be a container: '%s'"), *KeyText);
				return false;
			}

			FEdGraphPinType ValuePinType;
			if (!ParseTypeInternal(ValueText, ValuePinType, OutError, false))
			{
				return false;
			}
			if (ValuePinType.IsContainer())
			{
				OutError = FString::Printf(TEXT("Map value type cannot be a container: '%s'"), *ValueText);
				return false;
			}

			OutPinType = KeyPinType;
			OutPinType.ContainerType = EPinContainerType::Map;
			OutPinType.PinValueType = ToTerminalType(ValuePinType);
			return true;
		}

		OutError = FString::Printf(TEXT("Unsupported container type '%s' in '%s'."), *ContainerName, *TypeText);
		return false;
	}
	else if (!OutError.IsEmpty())
	{
		return false;
	}

	OutError.Empty();

	if (StartsWithNoCase(TypeText, TEXT("Enum:")))
	{
		const FString EnumText = NormalizeToken(TypeText.RightChop(5));
		if (UEnum* Enum = ResolveEnum(EnumText))
		{
			OutPinType.PinCategory = UEdGraphSchema_K2::PC_Enum;
			OutPinType.PinSubCategoryObject = Enum;
			return true;
		}
		OutError = FString::Printf(TEXT("Failed to resolve enum type '%s'."), *EnumText);
		return false;
	}

	if (StartsWithNoCase(TypeText, TEXT("Object:")) || StartsWithNoCase(TypeText, TEXT("Class:")) || StartsWithNoCase(TypeText, TEXT("SoftObject:")) || StartsWithNoCase(TypeText, TEXT("SoftClass:")))
	{
		FString Prefix;
		FString ClassText;
		if (StartsWithNoCase(TypeText, TEXT("Object:")))
		{
			Prefix = TEXT("Object");
			ClassText = NormalizeToken(TypeText.RightChop(7));
		}
		else if (StartsWithNoCase(TypeText, TEXT("Class:")))
		{
			Prefix = TEXT("Class");
			ClassText = NormalizeToken(TypeText.RightChop(6));
		}
		else if (StartsWithNoCase(TypeText, TEXT("SoftObject:")))
		{
			Prefix = TEXT("SoftObject");
			ClassText = NormalizeToken(TypeText.RightChop(11));
		}
		else
		{
			Prefix = TEXT("SoftClass");
			ClassText = NormalizeToken(TypeText.RightChop(10));
		}

		UClass* Class = ResolveClass(ClassText);
		if (!Class)
		{
			OutError = FString::Printf(TEXT("Failed to resolve class type '%s'."), *ClassText);
			return false;
		}

		if (Prefix.Equals(TEXT("Object"), ESearchCase::IgnoreCase))
		{
			OutPinType.PinCategory = UEdGraphSchema_K2::PC_Object;
		}
		else if (Prefix.Equals(TEXT("Class"), ESearchCase::IgnoreCase))
		{
			OutPinType.PinCategory = UEdGraphSchema_K2::PC_Class;
		}
		else if (Prefix.Equals(TEXT("SoftObject"), ESearchCase::IgnoreCase))
		{
			OutPinType.PinCategory = UEdGraphSchema_K2::PC_SoftObject;
		}
		else
		{
			OutPinType.PinCategory = UEdGraphSchema_K2::PC_SoftClass;
		}

		OutPinType.PinSubCategoryObject = Class;
		return true;
	}

	if (const FName PinCategory = ToPinCategory(TypeText); PinCategory != NAME_None)
	{
		OutPinType.PinCategory = PinCategory;
		if (PinCategory == UEdGraphSchema_K2::PC_Struct)
		{
			if (UScriptStruct* BuiltinStruct = ResolveBuiltinStruct(TypeText))
			{
				OutPinType.PinSubCategoryObject = BuiltinStruct;
				return true;
			}
		}
		return true;
	}

	if (UScriptStruct* Struct = ResolveStruct(TypeText))
	{
		OutPinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
		OutPinType.PinSubCategoryObject = Struct;
		return true;
	}

	if (UEnum* Enum = ResolveEnum(TypeText))
	{
		OutPinType.PinCategory = UEdGraphSchema_K2::PC_Enum;
		OutPinType.PinSubCategoryObject = Enum;
		return true;
	}

	OutError = FString::Printf(TEXT("Unsupported variable Type '%s'."), *TypeText);
	return false;
}

bool FTypeParser::ParseContainer(const FString& InTypeText, FString& OutContainerName, FString& OutInnerText, FString& OutError)
{
	OutContainerName.Empty();
	OutInnerText.Empty();
	OutError.Empty();

	const int32 LtIndex = InTypeText.Find(TEXT("<"));
	if (LtIndex == INDEX_NONE)
	{
		return false;
	}
	if (!InTypeText.EndsWith(TEXT(">")))
	{
		OutError = FString::Printf(TEXT("Invalid container type syntax '%s'."), *InTypeText);
		return false;
	}

	OutContainerName = NormalizeToken(InTypeText.Left(LtIndex));
	OutInnerText = NormalizeToken(InTypeText.Mid(LtIndex + 1, InTypeText.Len() - LtIndex - 2));
	if (OutContainerName.IsEmpty() || OutInnerText.IsEmpty())
	{
		OutError = FString::Printf(TEXT("Invalid container type syntax '%s'."), *InTypeText);
		return false;
	}
	return true;
}

bool FTypeParser::ParseMapContainer(const FString& InInnerText, FString& OutKeyText, FString& OutValueText, FString& OutError)
{
	OutKeyText.Empty();
	OutValueText.Empty();

	const int32 CommaIndex = InInnerText.Find(TEXT(","));
	if (CommaIndex == INDEX_NONE)
	{
		OutError = FString::Printf(TEXT("Map type requires two type parameters: '%s'."), *InInnerText);
		return false;
	}

	if (InInnerText.Find(TEXT(","), ESearchCase::CaseSensitive, ESearchDir::FromStart, CommaIndex + 1) != INDEX_NONE)
	{
		OutError = FString::Printf(TEXT("Map type must have exactly two type parameters: '%s'."), *InInnerText);
		return false;
	}

	OutKeyText = NormalizeToken(InInnerText.Left(CommaIndex));
	OutValueText = NormalizeToken(InInnerText.Mid(CommaIndex + 1));
	if (OutKeyText.IsEmpty() || OutValueText.IsEmpty())
	{
		OutError = FString::Printf(TEXT("Map key/value type must not be empty: '%s'."), *InInnerText);
		return false;
	}
	return true;
}

FString FTypeParser::NormalizeToken(const FString& InText)
{
	return InText.TrimStartAndEnd();
}
