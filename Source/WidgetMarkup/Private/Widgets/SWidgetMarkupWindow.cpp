// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "SWidgetMarkupWindow.h"

#include "IDetailsView.h"
#include "Misc/PackageName.h"
#include "PropertyEditorModule.h"
#include "WidgetBlueprint.h"
#include "WidgetMarkupModule.h"
#include "Engine/Blueprint.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"

SLATE_IMPLEMENT_WIDGET(SWidgetMarkupWindow)

void SWidgetMarkupWindow::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
}

SWidgetMarkupWindow::SWidgetMarkupWindow()
	: PreviewScene(
		FPreviewScene::ConstructionValues()
		.AllowAudioPlayback(true)
		.ShouldSimulatePhysics(true)
	)
{
}

SWidgetMarkupWindow::~SWidgetMarkupWindow()
{
	if (auto WidgetMarkupModulePtr = FModuleManager::Get().GetModulePtr<FWidgetMarkupModule>(TEXT("WidgetMarkup")))
	{
		WidgetMarkupModulePtr->GetOnObjectCompiled().RemoveAll(this);
	}
}

void SWidgetMarkupWindow::Construct(const FArguments& InArgs, const FString& InPackagePath)
{
	PackagePath = InPackagePath;

	FString AssetName = FPackageName::GetShortName(InPackagePath);
	Super::Construct(
		SWindow::FArguments()
		.Title(FText::FromString(AssetName))
		.ClientSize(FVector2D(800, 600))
	);
	RefreshContent();
	auto& WidgetMarkupModule = FModuleManager::Get().LoadModuleChecked<FWidgetMarkupModule>(TEXT("WidgetMarkup"));
	WidgetMarkupModule.GetOnObjectCompiled().AddSP(this, &SWidgetMarkupWindow::HandleOnObjectCompiled);
}

void SWidgetMarkupWindow::HandleOnObjectCompiled(FName Name, UObject* Object)
{
	RefreshContent();
}

void SWidgetMarkupWindow::AddReferencedObjects(FReferenceCollector& Collector)
{
	SWindow::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(Widget);
}

FString SWidgetMarkupWindow::GetReferencerName() const
{
	return TEXT("SWidgetMarkupWindow");
}

bool SWidgetMarkupWindow::TryBuildWidgetPreview(UObject* Object)
{
	if (auto WidgetBlueprint = Cast<UWidgetBlueprint>(Object))
	{
		if (auto WidgetClass = Cast<UWidgetBlueprintGeneratedClass>(WidgetBlueprint->GeneratedClass))
		{
			Widget = CreateWidget(PreviewScene.GetWorld(), WidgetClass);
			if (Widget)
			{
				return true;
			}
		}
	}
	else if (auto WidgetTreeTemplate = Cast<UWidgetTree>(Object))
	{
		if (auto WidgetTree = NewObject<UWidgetTree>(GetTransientPackage(), WidgetTreeTemplate->GetClass(), NAME_None, RF_NoFlags, WidgetTreeTemplate))
		{
			Widget = WidgetTree->RootWidget;
			if (Widget)
			{
				return true;
			}
		}
	}
	else if (auto WidgetTemplate = Cast<UWidget>(Object))
	{
		Widget = NewObject<UWidget>(GetTransientPackage(), WidgetTemplate->GetClass(), NAME_None, RF_NoFlags, WidgetTemplate);
		if (Widget)
		{
			return true;
		}
	}

	return false;
}

bool SWidgetMarkupWindow::TryBuildDetailsView(UObject* Object)
{
	auto Blueprint = Cast<UBlueprint>(Object);
	if (!Blueprint || !Blueprint->GeneratedClass)
	{
		return false;
	}

	UObject* DetailsObject = Blueprint->GeneratedClass->GetDefaultObject();
	if (!DetailsObject)
	{
		return false;
	}

	if (!BlueprintDetailsView.IsValid())
	{
		FDetailsViewArgs DetailsViewArgs;
		DetailsViewArgs.bUpdatesFromSelection = false;
		DetailsViewArgs.bLockable = false;
		DetailsViewArgs.bAllowSearch = true;
		DetailsViewArgs.bShowScrollBar = true;
		DetailsViewArgs.bShowOptions = false;
		DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
		DetailsViewArgs.bHideSelectionTip = true;

		auto& PropertyEditorModule = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		BlueprintDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
		BlueprintDetailsView->SetIsPropertyReadOnlyDelegate(FIsPropertyReadOnly::CreateLambda([](const FPropertyAndParent&)
		{
			return true;
		}));
	}

	BlueprintDetailsView->SetObject(DetailsObject);
	this->SetContent(BlueprintDetailsView.ToSharedRef());
	return true;
}

void SWidgetMarkupWindow::RefreshContent()
{
	Widget = nullptr;

	auto& WidgetMarkupModule = FModuleManager::Get().LoadModuleChecked<FWidgetMarkupModule>(TEXT("WidgetMarkup"));
	auto Object = WidgetMarkupModule.GetObjectFromPackagePath(PackagePath);
	if (!Object)
	{
		Object = WidgetMarkupModule.CompileFromPackagePath(PackagePath);
	}

	if (!Object)
	{
		this->SetContent(SNullWidget::NullWidget);
		return;
	}

	if (TryBuildWidgetPreview(Object))
	{
		this->SetContent(Widget->TakeWidget());
		return;
	}

	if (TryBuildDetailsView(Object))
	{
		return;
	}

	this->SetContent(SNullWidget::NullWidget);
}

static FAutoConsoleCommand GWidgetMarkupShow
(
	TEXT("WidgetMarkup.Show"),
	TEXT("Show the Widget Markup Content in a Window. Expects a package path like /Game/WidgetMarkup/MyWidget (no file extension)."),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			return;
		}
		const FString& PackagePath = Args[0];
		if (!PackagePath.StartsWith(TEXT("/Game/")))
		{
			UE_LOG(LogWidgetMarkup, Error, TEXT("WidgetMarkup.Show: expected a package path like /Game/WidgetMarkup/MyWidget, got '%s'."), *PackagePath);
			return;
		}
		FSlateApplication::Get().AddWindow(SNew(SWidgetMarkupWindow, PackagePath));
	})
);
