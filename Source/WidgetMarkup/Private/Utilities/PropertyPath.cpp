// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "Utilities/PropertyPath.h"

namespace
{
	static bool TryParseArrayIndexToken(const FString& Token, int32& OutArrayIndex)
	{
		if (!LexTryParseString(OutArrayIndex, *Token))
		{
			return false;
		}

		return OutArrayIndex >= 0;
	}

	static bool IsValidPropertyToken(const FString& Token)
	{
		return !Token.IsEmpty() && !Token.Contains(TEXT(".")) && !Token.Contains(TEXT("[")) && !Token.Contains(TEXT("]"));
	}

	static bool AreElementsEqual(const FPropertyPathElement& Left, const FPropertyPathElement& Right)
	{
		if (Left.Type != Right.Type || Left.bIsAny != Right.bIsAny)
		{
			return false;
		}

		switch (Left.Type)
		{
		case EPropertyPathElementType::Property:
		case EPropertyPathElementType::MapKey:
			return Left.Name.Equals(Right.Name, ESearchCase::CaseSensitive);

		case EPropertyPathElementType::ArrayIndex:
			return Left.ArrayIndex == Right.ArrayIndex;

		default:
			return false;
		}
	}
}

FPropertyPathElement FPropertyPathElement::MakeProperty(const FStringView& PropertyName)
{
	FPropertyPathElement Element;
	Element.Type = EPropertyPathElementType::Property;
	Element.Name = FString(PropertyName);
	return Element;
}

FPropertyPathElement FPropertyPathElement::MakeAnyProperty()
{
	FPropertyPathElement Element;
	Element.Type = EPropertyPathElementType::Property;
	Element.bIsAny = true;
	return Element;
}

FPropertyPathElement FPropertyPathElement::MakeArrayIndex(int32 InArrayIndex)
{
	FPropertyPathElement Element;
	Element.Type = EPropertyPathElementType::ArrayIndex;
	Element.ArrayIndex = InArrayIndex;
	return Element;
}

FPropertyPathElement FPropertyPathElement::MakeAnyArrayIndex()
{
	FPropertyPathElement Element;
	Element.Type = EPropertyPathElementType::ArrayIndex;
	Element.bIsAny = true;
	return Element;
}

FPropertyPathElement FPropertyPathElement::MakeMapKey(const FStringView& MapKey)
{
	FPropertyPathElement Element;
	Element.Type = EPropertyPathElementType::MapKey;
	Element.Name = FString(MapKey);
	return Element;
}

FPropertyPathElement FPropertyPathElement::MakeAnyMapKey()
{
	FPropertyPathElement Element;
	Element.Type = EPropertyPathElementType::MapKey;
	Element.bIsAny = true;
	return Element;
}

bool FPropertyPathElement::Matches(const FPropertyPathElement& Candidate) const
{
	if (Type != Candidate.Type)
	{
		return false;
	}

	if (bIsAny)
	{
		return true;
	}

	switch (Type)
	{
	case EPropertyPathElementType::Property:
	case EPropertyPathElementType::MapKey:
		return Name.Equals(Candidate.Name, ESearchCase::CaseSensitive);

	case EPropertyPathElementType::ArrayIndex:
		return ArrayIndex == Candidate.ArrayIndex;

	default:
		return false;
	}
}

FString FPropertyPathElement::ToString() const
{
	switch (Type)
	{
	case EPropertyPathElementType::Property:
		return bIsAny ? TEXT("*") : Name;

	case EPropertyPathElementType::ArrayIndex:
		return bIsAny ? TEXT("[*]") : FString::Printf(TEXT("[%d]"), ArrayIndex);

	case EPropertyPathElementType::MapKey:
		return bIsAny ? TEXT("[*]") : FString::Printf(TEXT("[%s]"), *Name);

	default:
		return FString();
	}
}

bool FPropertyPath::TryParse(const FStringView& InText, FPropertyPath& OutPath, FString* OutError)
{
	OutPath.Reset();

	const FString Text(InText);
	if (Text.IsEmpty())
	{
		if (OutError)
		{
			*OutError = TEXT("Property path must not be empty.");
		}
		return false;
	}

	int32 Position = 0;
	while (Position < Text.Len())
	{
		const int32 SegmentStart = Position;
		while (Position < Text.Len() && Text[Position] != TCHAR('.') && Text[Position] != TCHAR('['))
		{
			++Position;
		}

		const FString PropertyToken = Text.Mid(SegmentStart, Position - SegmentStart);
		const bool bBracketSegmentFollows = Position < Text.Len() && Text[Position] == TCHAR('[');
		if (PropertyToken.IsEmpty())
		{
			if (!bBracketSegmentFollows)
			{
				if (OutError)
				{
					*OutError = FString::Printf(TEXT("Invalid empty property segment in property path '%s'."), *Text);
				}
				return false;
			}
		}
		else if (PropertyToken == TEXT("*"))
		{
			OutPath.AppendAnyProperty();
		}
		else if (IsValidPropertyToken(PropertyToken))
		{
			OutPath.AppendProperty(PropertyToken);
		}
		else
		{
			if (OutError)
			{
				*OutError = FString::Printf(TEXT("Invalid property segment '%s' in property path '%s'."), *PropertyToken, *Text);
			}
			return false;
		}

		while (Position < Text.Len() && Text[Position] == TCHAR('['))
		{
			const int32 TokenStart = Position + 1;
			const int32 TokenEnd = Text.Find(TEXT("]"), ESearchCase::CaseSensitive, ESearchDir::FromStart, TokenStart);
			if (TokenEnd == INDEX_NONE)
			{
				if (OutError)
				{
					*OutError = FString::Printf(TEXT("Unterminated bracket segment in property path '%s'."), *Text);
				}
				return false;
			}

			const FString BracketToken = Text.Mid(TokenStart, TokenEnd - TokenStart);
			if (BracketToken.IsEmpty())
			{
				if (OutError)
				{
					*OutError = FString::Printf(TEXT("Empty bracket segment in property path '%s'."), *Text);
				}
				return false;
			}

			if (BracketToken == TEXT("*"))
			{
				OutPath.AppendAnyArrayIndex();
			}
			else
			{
				int32 ArrayIndex = INDEX_NONE;
				if (!TryParseArrayIndexToken(BracketToken, ArrayIndex))
				{
					if (OutError)
					{
						*OutError = FString::Printf(TEXT("Map key segment '[%s]' is not supported yet in property path '%s'."), *BracketToken, *Text);
					}
					return false;
				}
				OutPath.AppendArrayIndex(ArrayIndex);
			}

			Position = TokenEnd + 1;
		}

		if (Position >= Text.Len())
		{
			break;
		}

		if (Text[Position] != TCHAR('.'))
		{
			if (OutError)
			{
				*OutError = FString::Printf(TEXT("Unexpected character '%c' in property path '%s'."), Text[Position], *Text);
			}
			return false;
		}

		++Position;
		if (Position >= Text.Len())
		{
			if (OutError)
			{
				*OutError = FString::Printf(TEXT("Property path '%s' must not end with '.'."), *Text);
			}
			return false;
		}
	}

	return !OutPath.IsEmpty();
}

void FPropertyPath::Reset()
{
	Elements.Reset();
	CanonicalString.Reset();
	CanonicalName = NAME_None;
}

bool FPropertyPath::IsEmpty() const
{
	return Elements.IsEmpty();
}

bool FPropertyPath::HasAny() const
{
	for (const FPropertyPathElement& Element : Elements)
	{
		if (Element.bIsAny)
		{
			return true;
		}
	}

	return false;
}

FString FPropertyPath::ToString() const
{
	return CanonicalString;
}

const FName& FPropertyPath::GetCanonicalName() const
{
	return CanonicalName;
}

bool FPropertyPath::operator==(const FPropertyPath& Other) const
{
	return CanonicalName == Other.CanonicalName;
}

bool FPropertyPath::Matches(const FPropertyPath& Candidate) const
{
	if (Elements.Num() != Candidate.Elements.Num())
	{
		return false;
	}

	for (int32 ElementIndex = 0; ElementIndex < Elements.Num(); ++ElementIndex)
	{
		if (!Elements[ElementIndex].Matches(Candidate.Elements[ElementIndex]))
		{
			return false;
		}
	}

	return true;
}

bool FPropertyPath::TryMakeRelativeTo(const FPropertyPath& BasePath, FPropertyPath& OutRelativePath) const
{
	OutRelativePath.Reset();

	const TArray<FPropertyPathElement>& BaseElements = BasePath.GetElements();
	if (BaseElements.IsEmpty() || BaseElements.Num() > Elements.Num())
	{
		return false;
	}

	for (int32 ElementIndex = 0; ElementIndex < BaseElements.Num(); ++ElementIndex)
	{
		if (!AreElementsEqual(BaseElements[ElementIndex], Elements[ElementIndex]))
		{
			return false;
		}
	}

	if (BaseElements.Num() == Elements.Num())
	{
		return true;
	}

	FString RelativePathString;
	for (int32 ElementIndex = BaseElements.Num(); ElementIndex < Elements.Num(); ++ElementIndex)
	{
		const FPropertyPathElement& Element = Elements[ElementIndex];
		const bool bIsPropertyOrMap = Element.Type == EPropertyPathElementType::Property || Element.Type == EPropertyPathElementType::MapKey;
		if (bIsPropertyOrMap && !RelativePathString.IsEmpty())
		{
			RelativePathString.AppendChar(TCHAR('.'));
		}

		RelativePathString.Append(Element.ToString());
	}

	if (RelativePathString.IsEmpty())
	{
		return true;
	}

	return FPropertyPath::TryParse(FStringView(RelativePathString), OutRelativePath);
}

FPropertyPath FPropertyPath::WithAppendedProperty(const FStringView& PropertyPathString) const
{
	FPropertyPath NewPath = *this;
	FPropertyPath AppendedPath;
	const FString PropertyPathText(PropertyPathString);
	checkf(FPropertyPath::TryParse(PropertyPathString, AppendedPath), TEXT("Invalid property path string '%s'."), *PropertyPathText);
	NewPath.Elements.Append(AppendedPath.Elements);
	NewPath.RefreshCanonicalPropertyPath();
	return NewPath;
}

FPropertyPath FPropertyPath::WithAppendedAnyProperty() const
{
	FPropertyPath NewPath = *this;
	NewPath.AppendAnyProperty();
	return NewPath;
}

FPropertyPath FPropertyPath::WithAppendedArrayIndex(int32 ArrayIndex) const
{
	FPropertyPath NewPath = *this;
	NewPath.AppendArrayIndex(ArrayIndex);
	return NewPath;
}

FPropertyPath FPropertyPath::WithAppendedAnyArrayIndex() const
{
	FPropertyPath NewPath = *this;
	NewPath.AppendAnyArrayIndex();
	return NewPath;
}

FPropertyPath FPropertyPath::WithAppendedMapKey(const FStringView& MapKey) const
{
	FPropertyPath NewPath = *this;
	NewPath.AppendMapKey(MapKey);
	return NewPath;
}

FPropertyPath FPropertyPath::WithAppendedAnyMapKey() const
{
	FPropertyPath NewPath = *this;
	NewPath.AppendAnyMapKey();
	return NewPath;
}

void FPropertyPath::AppendProperty(const FStringView& PropertyName)
{
	Elements.Add(FPropertyPathElement::MakeProperty(PropertyName));
	RefreshCanonicalPropertyPath();
}

void FPropertyPath::AppendAnyProperty()
{
	Elements.Add(FPropertyPathElement::MakeAnyProperty());
	RefreshCanonicalPropertyPath();
}

void FPropertyPath::AppendArrayIndex(int32 ArrayIndex)
{
	Elements.Add(FPropertyPathElement::MakeArrayIndex(ArrayIndex));
	RefreshCanonicalPropertyPath();
}

void FPropertyPath::AppendAnyArrayIndex()
{
	Elements.Add(FPropertyPathElement::MakeAnyArrayIndex());
	RefreshCanonicalPropertyPath();
}

void FPropertyPath::AppendMapKey(const FStringView& MapKey)
{
	Elements.Add(FPropertyPathElement::MakeMapKey(MapKey));
	RefreshCanonicalPropertyPath();
}

void FPropertyPath::AppendAnyMapKey()
{
	Elements.Add(FPropertyPathElement::MakeAnyMapKey());
	RefreshCanonicalPropertyPath();
}

const TArray<FPropertyPathElement>& FPropertyPath::GetElements() const
{
	return Elements;
}

void FPropertyPath::RefreshCanonicalPropertyPath()
{
	CanonicalString.Reset();
	for (const FPropertyPathElement& Element : Elements)
	{
		if (Element.Type == EPropertyPathElementType::Property)
		{
			if (!CanonicalString.IsEmpty())
			{
				CanonicalString += TEXT(".");
			}
			CanonicalString += Element.ToString();
		}
		else
		{
			CanonicalString += Element.ToString();
		}
	}

	CanonicalName = CanonicalString.IsEmpty() ? NAME_None : FName(*CanonicalString);
}