// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "WidgetMarkupWindow.h"

#include "Misc/PackageName.h"
#include "WidgetBlueprint.h"
#include "WidgetMarkupModule.h"
#include "Blueprint/UserWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/SWindow.h"

UWidgetMarkupWindow::UWidgetMarkupWindow() = default;

UWidgetMarkupWindow* UWidgetMarkupWindow::CreateWidgetMarkupWindow(UObject* Outer, const FString& InPackagePath)
{
	UObject* ObjectOuter = Outer ? Outer : GetTransientPackage();
	TStrongObjectPtr<UWidgetMarkupWindow> WidgetMarkupWindow(NewObject<UWidgetMarkupWindow>(ObjectOuter));
	if (!WidgetMarkupWindow->SetPackagePath(InPackagePath))
	{
		return nullptr;
	}
	return WidgetMarkupWindow.Get();
}

bool UWidgetMarkupWindow::SetPackagePath(const FString& InPackagePath)
{
	PackagePath = InPackagePath;

	auto& WidgetMarkupModule = FModuleManager::Get().LoadModuleChecked<FWidgetMarkupModule>(TEXT("WidgetMarkup"));
	WidgetMarkupModule.GetOnObjectCompiled().RemoveAll(this);
	WidgetMarkupModule.GetOnObjectCompiled().AddUObject(this, &UWidgetMarkupWindow::HandleOnObjectCompiled);

	RebuildWidget();
	return true;
}

bool UWidgetMarkupWindow::OpenWindow()
{
	if (PackagePath.IsEmpty() || !FSlateApplication::IsInitialized())
	{
		return false;
	}

	if (SlateWindow.IsValid())
	{
		SlateWindow->BringToFront(true);
		return true;
	}

	const FString AssetName = FPackageName::GetShortName(PackagePath);
	TSharedRef<SWindow> NewWindow = SNew(SWindow)
		.Title(FText::FromString(AssetName))
		.ClientSize(FVector2D(800, 600))
		[SNullWidget::NullWidget];
	NewWindow->SetOnWindowClosed(FOnWindowClosed::CreateUObject(this, &UWidgetMarkupWindow::HandleSlateWindowClosed));
	SlateWindow = NewWindow;
	FSlateApplication::Get().AddWindow(NewWindow);
	RebuildWidget();
	return true;
}

void UWidgetMarkupWindow::RebuildWidget()
{
	if (bIsRebuilding || !SlateWindow.IsValid())
	{
		return;
	}

	TGuardValue<bool> RebuildGuard(bIsRebuilding, true);

	Widget = nullptr;

	auto& WidgetMarkupModule = FModuleManager::Get().LoadModuleChecked<FWidgetMarkupModule>(TEXT("WidgetMarkup"));
	UObject* Object = WidgetMarkupModule.GetObjectOrCompileFromPackage(PackagePath);

	TSharedRef<SWidget> NewContent = SNullWidget::NullWidget;

	if (UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(Object))
	{
		if (UWidgetBlueprintGeneratedClass* WidgetClass = Cast<UWidgetBlueprintGeneratedClass>(WidgetBlueprint->GeneratedClass))
		{
			if (!PreviewScene)
			{
				PreviewScene = MakeShared<FPreviewScene>(
					FPreviewScene::ConstructionValues()
					.AllowAudioPlayback(true)
					.ShouldSimulatePhysics(true)
				);
			}
			Widget = CreateWidget(PreviewScene->GetWorld(), WidgetClass);
			if (Widget)
			{
				NewContent = Widget->TakeWidget();
			}
		}
	}

	SlateWindow->SetContent(NewContent);
}

void UWidgetMarkupWindow::CloseWindow()
{
	if (SlateWindow.IsValid())
	{
		SlateWindow->RequestDestroyWindow();
	}
	SlateWindow.Reset();
}

void UWidgetMarkupWindow::HandleSlateWindowClosed(const TSharedRef<SWindow>& ClosedWindow)
{
	if (SlateWindow == ClosedWindow)
	{
		SlateWindow.Reset();
	}

	OnWindowClosed.Broadcast();
}

void UWidgetMarkupWindow::BeginDestroy()
{
	if (FWidgetMarkupModule* WidgetMarkupModule = FModuleManager::Get().GetModulePtr<FWidgetMarkupModule>(TEXT("WidgetMarkup")))
	{
		WidgetMarkupModule->GetOnObjectCompiled().RemoveAll(this);
	}
	PreviewScene.Reset();
	CloseWindow();
	Super::BeginDestroy();
}

void UWidgetMarkupWindow::HandleOnObjectCompiled(FName Name, UObject* Object)
{
	if (Name == FName(*PackagePath))
	{
		RebuildWidget();
	}
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
		const FString& InputPackagePath = Args[0];
		if (!InputPackagePath.StartsWith(TEXT("/Game/")))
		{
			UE_LOG(LogWidgetMarkup, Error, TEXT("WidgetMarkup.Show: expected a package path like /Game/WidgetMarkup/MyWidget, got '%s'."), *InputPackagePath);
			return;
		}

		UWidgetMarkupWindow* WindowObject = UWidgetMarkupWindow::CreateWidgetMarkupWindow(GetTransientPackage(), InputPackagePath);
		WindowObject->AddToRoot();
		if (!WindowObject || !WindowObject->OpenWindow())
		{
			UE_LOG(LogWidgetMarkup, Error, TEXT("WidgetMarkup.Show: failed to create or open window for '%s'."), *InputPackagePath);
			return;
		}
		WindowObject->OnWindowClosed.AddLambda([WindowObject]()
		{
			WindowObject->RemoveFromRoot();
		});
	})
);
