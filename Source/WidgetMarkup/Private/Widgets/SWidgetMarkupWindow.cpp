// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "SWidgetMarkupWindow.h"

#include "WidgetBlueprint.h"
#include "WidgetMarkupModule.h"
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

void SWidgetMarkupWindow::Construct(const FArguments& InArgs, const FString& InSourceFilePath)
{
	Super::Construct(
		SWindow::FArguments()
		.ClientSize(FVector2D(800, 600))
	);
	SourceFilePath = InSourceFilePath;
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

void SWidgetMarkupWindow::RefreshContent()
{
	auto& WidgetMarkupModule = FModuleManager::Get().LoadModuleChecked<FWidgetMarkupModule>(TEXT("WidgetMarkup"));
	auto Object = WidgetMarkupModule.GetObjectFromFile(SourceFilePath);
	if (!Object)
	{
		Object = WidgetMarkupModule.CompileFromFile(SourceFilePath);
	}
	if (Object)
	{
		if (auto WidgetBlueprint = Cast<UWidgetBlueprint>(Object))
		{
			Widget = CreateWidget(PreviewScene.GetWorld(), Cast<UWidgetBlueprintGeneratedClass>(WidgetBlueprint->GeneratedClass));
		}
		else if (auto WidgetTreeTemplate = Cast<UWidgetTree>(Object))
		{
			if (auto WidgetTree = NewObject<UWidgetTree>(GetTransientPackage(), WidgetTreeTemplate->GetClass(), NAME_None, RF_NoFlags, WidgetTreeTemplate))
			{
				Widget = WidgetTree->RootWidget;
			}
		}
		else if (auto WidgetTemplate = Cast<UWidget>(Object))
		{
			Widget = NewObject<UWidget>(GetTransientPackage(), WidgetTemplate->GetClass(), NAME_None, RF_NoFlags, WidgetTemplate);
		}
	}
	if (Widget)
	{
		this->SetContent(Widget->TakeWidget());
	}
}

static FAutoConsoleCommand GWidgetMarkupShow
(
	TEXT("WidgetMarkup.Show"),
	TEXT("Show the Widget Markup Content in a Window."),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			return;
		}
		const auto& SourceFilePath = Args[0];
		FSlateApplication::Get().AddWindow(SNew(SWidgetMarkupWindow, SourceFilePath));
	})
);
