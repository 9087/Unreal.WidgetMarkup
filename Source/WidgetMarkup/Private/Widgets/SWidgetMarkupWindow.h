// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

class UWidget;
class UBlueprint;
class UWidgetBlueprintGeneratedClass;
class IDetailsView;

class SWidgetMarkupWindow : public SWindow
{
	SLATE_DECLARE_WIDGET(SWidgetMarkupWindow, SWindow)

public:
	SLATE_BEGIN_ARGS(SWidgetMarkupWindow)
		{}
	SLATE_END_ARGS()

	SWidgetMarkupWindow();
	virtual ~SWidgetMarkupWindow() override;

	void Construct(const FArguments& InArgs, const FString& InPackagePath);

protected:
	void HandleOnObjectCompiled(FName Name, UObject* Object);
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;
	void RefreshContent();
	bool TryBuildWidgetPreview(UObject* Object);
	bool TryBuildDetailsView(UObject* Object);

	FString PackagePath;
	FPreviewScene PreviewScene;
	TObjectPtr<UWidget> Widget;
	TSharedPtr<IDetailsView> BlueprintDetailsView;
};
