// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyRuns/BlueprintImplementsPropertyRun.h"

#include "Engine/Blueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Utilities/TypeParser.h"

TSharedRef<IPropertyRun> FBlueprintImplementsPropertyRun::Create()
{
	return MakeShared<FBlueprintImplementsPropertyRun>();
}

FElementNode::FResult FBlueprintImplementsPropertyRun::OnBegin(FElementNode::FContext& /*Context*/, UObject* Object, const FStringView& /*PropertyName*/, const FStringView& PropertyValue)
{
	UBlueprint* Blueprint = Cast<UBlueprint>(Object);
	if (!Blueprint)
	{
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Implements property target is not a Blueprint.")));
	}

	const FString ImplementsText = FString(PropertyValue).TrimStartAndEnd();
	if (ImplementsText.IsEmpty())
	{
		return FElementNode::FResult::Success();
	}

	TArray<FString> InterfaceTokens;
	ImplementsText.ParseIntoArray(InterfaceTokens, TEXT(","), true);

	for (FString& Token : InterfaceTokens)
	{
		Token.TrimStartAndEndInline();
		if (Token.IsEmpty())
		{
			continue;
		}

		UClass* InterfaceClass = FTypeParser::ResolveInterface(Token);
		if (!InterfaceClass)
		{
			return FElementNode::FResult::Failure().Error(FText::Format(
				FText::FromString(TEXT("Implements property '{0}' cannot be resolved to a valid UInterface.")),
				FText::FromString(Token)));
		}

		FBlueprintEditorUtils::ImplementNewInterface(Blueprint, FTopLevelAssetPath(InterfaceClass));
	}

	return FElementNode::FResult::Success();
}

FElementNode::FResult FBlueprintImplementsPropertyRun::OnEnd(FElementNode::FContext& /*Context*/)
{
	return FElementNode::FResult::Success();
}
