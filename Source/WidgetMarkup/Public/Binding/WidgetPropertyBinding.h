#pragma once

#include "CoreMinimal.h"
#include "Utilities/WidgetPropertyPath.h"

#include "WidgetPropertyBinding.generated.h"

USTRUCT(BlueprintType)
struct WIDGETMARKUP_API FWidgetPropertyBinding
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "PropertyBinding")
	FString SourceExpression;

	UPROPERTY(BlueprintReadOnly, Category = "PropertyBinding")
	FName TargetObjectName;

	UPROPERTY(BlueprintReadOnly, Category = "PropertyBinding")
	FString TargetPropertyPath;

	UPROPERTY(BlueprintReadOnly, Category = "PropertyBinding")
	FString SourceFile;

	UPROPERTY(BlueprintReadOnly, Category = "PropertyBinding")
	int32 SourceLine = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "PropertyBinding")
	int32 SourceColumn = INDEX_NONE;

	static FWidgetPropertyBinding Create(
		const FStringView& InSourceExpression,
		FName InTargetObjectName,
		const FWidgetPropertyPath& InTargetPropertyPath,
		const FStringView& InSourceFile = FStringView(),
		int32 InSourceLine = INDEX_NONE,
		int32 InSourceColumn = INDEX_NONE)
	{
		FWidgetPropertyBinding Binding;
		Binding.SourceExpression = FString(InSourceExpression);
		Binding.TargetObjectName = InTargetObjectName;
		Binding.TargetPropertyPath = InTargetPropertyPath.GetPathName().ToString();
		Binding.SourceFile = FString(InSourceFile);
		Binding.SourceLine = InSourceLine;
		Binding.SourceColumn = InSourceColumn;
		return Binding;
	}
};

