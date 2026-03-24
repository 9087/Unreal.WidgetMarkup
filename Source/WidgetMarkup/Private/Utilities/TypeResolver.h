// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Generic type resolver template with unified resolution semantics:
 * - Case-sensitive matching (no case-insensitive fallback)
 * - No automatic U-prefix completion for short names
 * - Support for long dotted tokens (e.g., "Game.WidgetMarkup.MyClass")
 * - First-match ambiguity handling (returns first match without warning)
 */
template<typename T>
class TTypeResolver
{
public:
	/**
	 * Resolve a type token to the corresponding UStruct-derived type.
	 * 
	 * @param Token Input token (short name like "Button" or long path like "Game.WidgetMarkup.MyWidget")
	 * @return Resolved type pointer or nullptr if not found
	 */
	static T* Resolve(const FStringView& Token);

private:
	/**
	 * Resolve short-name tokens using case-sensitive exact match.
	 * Specializations differ but unified by template dispatch.
	 */
	static T* ResolveShortName(const FString& Token);
};
