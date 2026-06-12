// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Fonts/FontProviderInterface.h"
#include "WidgetMarkupFontProvider.generated.h"

/**
 * Runtime font provider that implements IFontProviderInterface.
 *
 * Created by FObjectConverter when a Font.FontObject string resolves to a
 * file path that is not a UE asset.  Slate's GetCompositeFont() discovers
 * this via IFontProviderInterface and uses the CompositeFont set up here.
 */
UCLASS()
class UWidgetMarkupFontProvider : public UObject, public IFontProviderInterface
{
	GENERATED_BODY()

public:
	// IFontProviderInterface
	virtual const FCompositeFont* GetCompositeFont() const override;
	virtual bool IsSdfFont() const override { return false; }
	virtual EFontRasterizationMode GetFontRasterizationMode() const override { return EFontRasterizationMode::Bitmap; }
	virtual const FFontSdfSettings& GetSdfSettings() const override { static FFontSdfSettings Dummy; return Dummy; }

	/**
	 * Create a provider from a font name or file path.
	 * Only activates for FSlateFontInfo::FontObject properties.
	 * @param Property  The property being converted (used to validate FSlateFontInfo::FontObject).
	 * @param InFontName  Font name (e.g. "seguisym") or absolute path.
	 * @return Valid provider, or nullptr if not applicable or font could not be resolved.
	 */
	static UWidgetMarkupFontProvider* CreateFromFontName(const FProperty& Property, const FString& InFontName);

	/** Set up the composite font from a resolved file path. */
	void InitializeFromPath(const FString& InPath);

	TSharedPtr<FCompositeFont> CompositeFont;
};
