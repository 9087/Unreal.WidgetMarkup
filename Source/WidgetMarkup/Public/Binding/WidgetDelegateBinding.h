#pragma once

#include "CoreMinimal.h"

#include "WidgetDelegateBinding.generated.h"

USTRUCT(BlueprintType)
struct WIDGETMARKUP_API FWidgetDelegateBinding
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "DelegateBinding")
	FName TargetWidgetName;

	UPROPERTY(BlueprintReadOnly, Category = "DelegateBinding")
	FName DelegatePropertyName;

	UPROPERTY(BlueprintReadOnly, Category = "DelegateBinding")
	FString FunctionName;

	static FWidgetDelegateBinding Create(
		FName InTargetWidgetName,
		FName InDelegatePropertyName,
		const FStringView& InFunctionName)
	{
		FWidgetDelegateBinding Binding;
		Binding.TargetWidgetName = InTargetWidgetName;
		Binding.DelegatePropertyName = InDelegatePropertyName;
		Binding.FunctionName = FString(InFunctionName);
		return Binding;
	}
};
