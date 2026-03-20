// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "WidgetMarkupModule.h"

#include "ConverterRegistry.h"
#include "DirectoryWatcherModule.h"
#include "ElementNodeFactory.h"
#include "FastXml.h"
#include "WidgetBlueprint.h"
#include "ElementTreeBuilder.h"
#include "IDirectoryWatcher.h"
#include "WidgetMarkupSettings.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ContentWidget.h"
#include "Components/Image.h"
#include "Components/Widget.h"
#include "Components/SlateWrapperTypes.h"
#include "Converters/BooleanConverter.h"
#include "Converters/ColorConverter.h"
#include "Converters/EnumConverter.h"
#include "Converters/LinearColorConverter.h"
#include "Converters/MarginConverter.h"
#include "Converters/NameConverter.h"
#include "Converters/NumericConverter.h"
#include "Converters/SlateChildSizeConverter.h"
#include "Converters/SlateColorConverter.h"
#include "Converters/StringConverter.h"
#include "Converters/TextConverter.h"
#include "Converters/VectorConverter.h"
#include "ElementNodes/ContentWidgetElementNode.h"
#include "ElementNodes/PanelWidgetElementNode.h"
#include "ElementNodes/WidgetBlueprintElementNode.h"
#include "ElementNodes/WidgetElementNode.h"
#include "ElementNodes/WidgetTreeElementNode.h"

#define LOCTEXT_NAMESPACE "WidgetMarkup"

DEFINE_LOG_CATEGORY(LogWidgetMarkup);

// ---------------------------------------------------------------------------
// Name custom attribute handler
// ---------------------------------------------------------------------------

struct FNameAttributeMetaData : public FElementNode::FContext::TMetaData<FNameAttributeMetaData>
{
	TMap<FString, FString> UsedNames;
};

static FElementNode::FResult ApplyNameAttribute(FElementNode::FContext& Context, UObject* Object, FName /*TypeName*/, const FStringView& Value)
{
	if (!Object)
	{
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Name attribute target object is null.")));
	}

	// 1. Trim whitespace
	FString Name = FString(Value).TrimStartAndEnd();

	// 2. Non-empty check
	if (Name.IsEmpty())
	{
		return FElementNode::FResult::Failure().Error(
			FText::Format(
				FText::FromString(TEXT("Name attribute on '{0}' must not be empty.")),
				FText::FromString(Object->GetClass()->GetName())));
	}

	// 3. Reject '.' — it breaks FindObject path parsing; reject internal spaces for Blueprint variable validity
	if (Name.Contains(TEXT(".")) || Name.Contains(TEXT(" ")))
	{
		return FElementNode::FResult::Failure().Error(
			FText::Format(
				FText::FromString(TEXT("Name attribute '{0}' contains invalid characters (no '.' or spaces allowed).")),
				FText::FromString(Name)));
	}

	// 4. Uniqueness check (case-insensitive) via typed parse-session metadata
	TSharedRef<FNameAttributeMetaData> NameMetaData = Context.GetOrAddMetaData<FNameAttributeMetaData>();
	const FString NormalizedName = Name.ToLower();
	if (const FString* ExistingDesc = NameMetaData->UsedNames.Find(NormalizedName))
	{
		return FElementNode::FResult::Failure().Error(
			FText::Format(
				FText::FromString(TEXT("Duplicate Name '{0}': already used by {1} in this blueprint.")),
				FText::FromString(Name),
				FText::FromString(*ExistingDesc)));
	}

	// 5. Record in context before rename (store pre-rename name for diagnostic clarity)
	NameMetaData->UsedNames.Add(NormalizedName, FString::Printf(TEXT("%s (%s)"), *Object->GetClass()->GetName(), *Object->GetName()));

	// 6. Rename the UObject to the desired name
	if (!Object->Rename(*Name, nullptr, REN_DontCreateRedirectors | REN_NonTransactional))
	{
		return FElementNode::FResult::Failure().Error(
			FText::Format(
				FText::FromString(TEXT("Failed to rename object to '{0}' — name may already be in use in this scope.")),
				FText::FromString(Name)));
	}

	// 7. Mark as Blueprint variable so UMG compiler generates a same-named property (Widget only)
	if (UWidget* Widget = Cast<UWidget>(Object))
	{
		Widget->bIsVariable = true;
	}

	return FElementNode::FResult::Success();
}

void FWidgetMarkupModule::StartupModule()
{
	FElementNodeFactory::Get().Register<UWidgetTree>(FElementNodeFactory::FOnCreateElementNode::CreateStatic(FWidgetTreeElementNode::Create));
	FElementNodeFactory::Get().Register<UWidget>(FElementNodeFactory::FOnCreateElementNode::CreateStatic(FWidgetElementNode::Create));
	FElementNodeFactory::Get().Register<UPanelWidget>(FElementNodeFactory::FOnCreateElementNode::CreateStatic(FPanelWidgetElementNode::Create));
	FElementNodeFactory::Get().Register<UContentWidget>(FElementNodeFactory::FOnCreateElementNode::CreateStatic(FContentWidgetElementNode::Create));
	FElementNodeFactory::Get().Register<UWidgetBlueprint>(FElementNodeFactory::FOnCreateElementNode::CreateStatic(FWidgetBlueprintElementNode::Create));

	RegisterCustomAttribute<UObject>(FName("Name"), NAME_StrProperty, FOnApplyCustomAttribute::CreateStatic(&ApplyNameAttribute));

	FConverterRegistry::Get().Register(NAME_ByteProperty, FConverterRegistry::FOnCreateConverter::CreateStatic(TNumericConverter<uint8>::Create));
	FConverterRegistry::Get().Register(NAME_IntProperty, FConverterRegistry::FOnCreateConverter::CreateStatic(TNumericConverter<int>::Create));
	FConverterRegistry::Get().Register(NAME_BoolProperty, FConverterRegistry::FOnCreateConverter::CreateStatic(FBooleanConverter::Create));
	FConverterRegistry::Get().Register(NAME_FloatProperty, FConverterRegistry::FOnCreateConverter::CreateStatic(TNumericConverter<float>::Create));
	FConverterRegistry::Get().Register(NAME_NameProperty, FConverterRegistry::FOnCreateConverter::CreateStatic(FNameConverter::Create));
	FConverterRegistry::Get().Register(NAME_DoubleProperty, FConverterRegistry::FOnCreateConverter::CreateStatic(TNumericConverter<double>::Create));
	FConverterRegistry::Get().Register(NAME_StrProperty, FConverterRegistry::FOnCreateConverter::CreateStatic(FStringConverter::Create));
	FConverterRegistry::Get().Register(NAME_TextProperty, FConverterRegistry::FOnCreateConverter::CreateStatic(FTextConverter::Create));
	FConverterRegistry::Get().Register(NAME_Int64Property, FConverterRegistry::FOnCreateConverter::CreateStatic(TNumericConverter<int64>::Create));
	FConverterRegistry::Get().Register(NAME_Int32Property, FConverterRegistry::FOnCreateConverter::CreateStatic(TNumericConverter<int32>::Create));
	FConverterRegistry::Get().Register(NAME_Int16Property, FConverterRegistry::FOnCreateConverter::CreateStatic(TNumericConverter<int16>::Create));
	FConverterRegistry::Get().Register(NAME_Int8Property, FConverterRegistry::FOnCreateConverter::CreateStatic(TNumericConverter<int8>::Create));
	FConverterRegistry::Get().Register(NAME_UInt64Property, FConverterRegistry::FOnCreateConverter::CreateStatic(TNumericConverter<uint64>::Create));
	FConverterRegistry::Get().Register(NAME_UInt32Property, FConverterRegistry::FOnCreateConverter::CreateStatic(TNumericConverter<uint32>::Create));
	FConverterRegistry::Get().Register(NAME_UInt16Property, FConverterRegistry::FOnCreateConverter::CreateStatic(TNumericConverter<uint16>::Create));
	FConverterRegistry::Get().Register(NAME_EnumProperty, FConverterRegistry::FOnCreateConverter::CreateStatic(FEnumConverter::Create));
	FConverterRegistry::Get().Register(NAME_Color, FConverterRegistry::FOnCreateConverter::CreateStatic(FColorConverter::Create));
	FConverterRegistry::Get().Register(NAME_LinearColor, FConverterRegistry::FOnCreateConverter::CreateStatic(FLinearColorConverter::Create));
	FConverterRegistry::Get().Register(FMargin::StaticStruct()->GetFName(), FConverterRegistry::FOnCreateConverter::CreateStatic(FMarginConverter::Create));
	FConverterRegistry::Get().Register(FSlateChildSize::StaticStruct()->GetFName(), FConverterRegistry::FOnCreateConverter::CreateStatic(FSlateChildSizeConverter::Create));
	FConverterRegistry::Get().Register(FSlateColor::StaticStruct()->GetFName(), FConverterRegistry::FOnCreateConverter::CreateStatic(FSlateColorConverter::Create));
	FConverterRegistry::Get().Register(NAME_Vector, FConverterRegistry::FOnCreateConverter::CreateStatic(TVectorConverter<FVector::FReal, 3>::Create));
	FConverterRegistry::Get().Register(NAME_Vector2D, FConverterRegistry::FOnCreateConverter::CreateStatic(TVectorConverter<FVector2D::FReal, 2>::Create));
	
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FWidgetMarkupModule::OnPostEngineInit);
}

void FWidgetMarkupModule::ShutdownModule()
{
	FElementNodeFactory::Get().Unregister<UWidgetTree>();
	FElementNodeFactory::Get().Unregister<UWidget>();
	FElementNodeFactory::Get().Unregister<UPanelWidget>();
	FElementNodeFactory::Get().Unregister<UContentWidget>();
	FElementNodeFactory::Get().Unregister<UWidgetBlueprint>();

	UnregisterCustomAttribute<UObject>(FName("Name"));

	FConverterRegistry::Get().Unregister(NAME_ByteProperty);
	FConverterRegistry::Get().Unregister(NAME_IntProperty);
	FConverterRegistry::Get().Unregister(NAME_BoolProperty);
	FConverterRegistry::Get().Unregister(NAME_FloatProperty);
	FConverterRegistry::Get().Unregister(NAME_NameProperty);
	FConverterRegistry::Get().Unregister(NAME_DoubleProperty);
	FConverterRegistry::Get().Unregister(NAME_StrProperty);
	FConverterRegistry::Get().Unregister(NAME_TextProperty);
	FConverterRegistry::Get().Unregister(NAME_Int64Property);
	FConverterRegistry::Get().Unregister(NAME_Int32Property);
	FConverterRegistry::Get().Unregister(NAME_Int16Property);
	FConverterRegistry::Get().Unregister(NAME_Int8Property);
	FConverterRegistry::Get().Unregister(NAME_UInt64Property);
	FConverterRegistry::Get().Unregister(NAME_UInt32Property);
	FConverterRegistry::Get().Unregister(NAME_UInt16Property);
	FConverterRegistry::Get().Unregister(NAME_EnumProperty);
	FConverterRegistry::Get().Unregister(NAME_Color);
	FConverterRegistry::Get().Unregister(NAME_LinearColor);
	FConverterRegistry::Get().Unregister(FMargin::StaticStruct()->GetFName());
	FConverterRegistry::Get().Unregister(FSlateChildSize::StaticStruct()->GetFName());
	FConverterRegistry::Get().Unregister(FSlateColor::StaticStruct()->GetFName());
	FConverterRegistry::Get().Unregister(NAME_Vector);
	FConverterRegistry::Get().Unregister(NAME_Vector2D);

	FCoreDelegates::OnPostEngineInit.RemoveAll(this);
	StopSourceFileWatching();
	CustomAttributes.Empty();
}

bool FWidgetMarkupModule::RegisterCustomAttribute(UStruct* Struct, FName AttributeName, FName TypeName, FOnApplyCustomAttribute InOnApplyCustomAttribute)
{
	if (!Struct || !InOnApplyCustomAttribute.IsBound())
	{
		UE_LOG(LogWidgetMarkup, Warning, TEXT("RegisterCustomAttribute failed: Struct is null or delegate is not bound (Struct='%s', AttributeName='%s')."),
			Struct ? *Struct->GetName() : TEXT("nullptr"), *AttributeName.ToString());
		return false;
	}
	TMap<FName, FCustomAttributeDescriptor>& InnerMap = CustomAttributes.FindOrAdd(Struct);
	if (InnerMap.Contains(AttributeName))
	{
		UE_LOG(LogWidgetMarkup, Warning, TEXT("RegisterCustomAttribute: overwriting existing (Struct='%s', AttributeName='%s')."), *Struct->GetName(), *AttributeName.ToString());
	}
	FCustomAttributeDescriptor& Descriptor = InnerMap.FindOrAdd(AttributeName);
	Descriptor.TypeName = TypeName;
	Descriptor.ApplyDelegate = InOnApplyCustomAttribute;
	return true;
}

void FWidgetMarkupModule::UnregisterCustomAttribute(UStruct* Struct, FName AttributeName)
{
	if (!Struct)
	{
		return;
	}
	TMap<FName, FCustomAttributeDescriptor>* InnerMap = CustomAttributes.Find(Struct);
	if (InnerMap)
	{
		InnerMap->Remove(AttributeName);
		if (InnerMap->Num() == 0)
		{
			CustomAttributes.Remove(Struct);
		}
	}
}

bool FWidgetMarkupModule::FindCustomAttributeDescriptor(UStruct* Struct, FName AttributeName, FCustomAttributeDescriptor& OutDescriptor) const
{
	if (!Struct)
	{
		return false;
	}
	UStruct* Best = nullptr;
	const TMap<FName, FCustomAttributeDescriptor>* BestInnerMap = nullptr;
	for (const auto& KeyValuePair : CustomAttributes)
	{
		UStruct* RegisteredStruct = KeyValuePair.Key.Get();
		if (!RegisteredStruct)
		{
			continue;
		}
		if (Struct->IsChildOf(RegisteredStruct))
		{
			if (!Best || RegisteredStruct->IsChildOf(Best))
			{
				Best = RegisteredStruct;
				BestInnerMap = &KeyValuePair.Value;
			}
		}
	}
	if (!BestInnerMap)
	{
		return false;
	}
	const FCustomAttributeDescriptor* Descriptor = BestInnerMap->Find(AttributeName);
	if (!Descriptor)
	{
		return false;
	}
	OutDescriptor = *Descriptor;
	return true;
}

// ---------------------------------------------------------------------------
// Path helpers: PackagePath (/Game/.../AssetName) <-> absolute disk file path
// ---------------------------------------------------------------------------

// /Game/WidgetMarkup/ListViewExample  ->  <ProjectDir>/Content/WidgetMarkup/ListViewExample.unrealwidgetmarkup
// Anchors to GetProjectFilePath() (set from the absolute command-line arg) to avoid
// BaseDir-relative resolution errors in non-standard editor binary locations.
static bool TryConvertPackagePathToAbsoluteSourceFilePath(const FString& PackagePath, FStringView Extension, FString& OutAbsoluteFilePath)
{
	static const FString GamePrefix = TEXT("/Game/");
	if (!PackagePath.StartsWith(GamePrefix))
	{
		return false;
	}
	const FString RelativePath = PackagePath.Mid(GamePrefix.Len());
	const FString ProjectDir = FPaths::GetPath(FPaths::GetProjectFilePath());
	OutAbsoluteFilePath = FPaths::Combine(ProjectDir, TEXT("Content"), RelativePath);
	OutAbsoluteFilePath += FString(Extension);
	FPaths::NormalizeFilename(OutAbsoluteFilePath);
	return true;
}

// <ProjectDir>/Content/WidgetMarkup/ListViewExample.unrealwidgetmarkup  ->  /Game/WidgetMarkup/ListViewExample
static bool TryConvertAbsoluteSourceFilePathToPackagePath(const FString& AbsoluteFilePath, FStringView Extension, FString& OutPackagePath)
{
	if (FPaths::IsRelative(AbsoluteFilePath))
	{
		return false;
	}

	const FString ProjectDir = FPaths::GetPath(FPaths::GetProjectFilePath());
	FString ContentDir = ProjectDir / TEXT("Content");
	FPaths::NormalizeFilename(ContentDir);
	if (!ContentDir.EndsWith(TEXT("/")))
	{
		ContentDir += TEXT("/");
	}
	FString NormalizedFilePath = AbsoluteFilePath;
	FPaths::NormalizeFilename(NormalizedFilePath);
	FPaths::CollapseRelativeDirectories(NormalizedFilePath);
	if (!NormalizedFilePath.StartsWith(ContentDir))
	{
		return false;
	}
	FString RelativePath = NormalizedFilePath.Mid(ContentDir.Len());
	const FString ExtensionString(Extension);
	if (RelativePath.EndsWith(ExtensionString))
	{
		RelativePath = RelativePath.LeftChop(ExtensionString.Len());
	}
	OutPackagePath = TEXT("/Game/") + RelativePath;
	return true;
}

// ---------------------------------------------------------------------------

UObject* FWidgetMarkupModule::CompileFromSourceCode(FName Name, const FString& XML)
{
	// If Name already follows UE package naming (starts with /), use it directly as the package path.
	// Otherwise apply the /WidgetMarkup/ prefix for names supplied by the Compile console command.
	const FString NameStr = Name.ToString();
	const FString PackageName = NameStr.StartsWith(TEXT("/")) ? NameStr : FString::Printf(TEXT("/WidgetMarkup/%s"), *NameStr);
	auto Package = CreatePackage(*PackageName);
	Package->SetFlags(RF_Transient | RF_Public);
	Package->SetPackageFlags(PKG_InMemoryOnly);

	FText ErrorMessage;
	int32 ErrorLineNumber;
	auto WidgetTreeBuilder = MakeShared<FElementTreeBuilder>(Package);
	if (!FFastXml::ParseXmlFile(&WidgetTreeBuilder.Get(), nullptr, const_cast<TCHAR*>(*XML), GWarn, true, false, ErrorMessage, ErrorLineNumber))
	{
		UE_LOG(LogWidgetMarkup, Error, TEXT("CompileFromSourceCode failed: XML parse error at line %d: %s"), ErrorLineNumber, *ErrorMessage.ToString());
		return nullptr;
	}
	auto RootElementNode = WidgetTreeBuilder->GetRootElementNode();
	if (!RootElementNode.IsValid())
	{
		UE_LOG(LogWidgetMarkup, Error, TEXT("CompileFromSourceCode failed: no root element produced for '%s'."), *Name.ToString());
		return nullptr;
	}
	auto Object = RootElementNode->GetObject();
	Objects.FindOrAdd(Name) = Object;
	GetOnObjectCompiled().Broadcast(Name, Object);
	return Object;
}

UObject* FWidgetMarkupModule::CompileFromPackagePath(const FString& PackagePath)
{
	UE_LOG(LogWidgetMarkup, Display, TEXT("Compile Package Path '%s'."), *PackagePath);

	FString AbsoluteFilePath;
	if (!TryConvertPackagePathToAbsoluteSourceFilePath(PackagePath, TEXT(".unrealwidgetmarkup"), AbsoluteFilePath))
	{
		UE_LOG(LogWidgetMarkup, Error, TEXT("CompileFromPackagePath failed: invalid package path '%s' (expected /Game/... format)."), *PackagePath);
		return nullptr;
	}
	FString XML;
	if (!FFileHelper::LoadFileToString(XML, *AbsoluteFilePath, FFileHelper::EHashOptions::None, FILEREAD_AllowWrite) || XML.IsEmpty())
	{
		UE_LOG(LogWidgetMarkup, Error, TEXT("CompileFromPackagePath failed: could not read file or file is empty ('%s')."), *AbsoluteFilePath);
		return nullptr;
	}
	return CompileFromSourceCode(FName(PackagePath), XML);
}

UObject* FWidgetMarkupModule::GetObjectFromName(FName Name)
{
	auto Object = Objects.Find(Name);
	return Object ? *Object : nullptr;
}

UObject* FWidgetMarkupModule::GetObjectFromPackagePath(const FString& PackagePath)
{
	return GetObjectFromName(FName(PackagePath));
}

void FWidgetMarkupModule::AddReferencedObjects(FReferenceCollector& Collector)
{
	for (auto& KeyValuePair : Objects)
	{
		Collector.AddReferencedObject(KeyValuePair.Value);
	}
}

FString FWidgetMarkupModule::GetReferencerName() const
{
	return TEXT("WidgetMarkupModule");
}

FWidgetMarkupModule::FOnObjectCompiled& FWidgetMarkupModule::GetOnObjectCompiled()
{
	return OnObjectCompiled;
}

void FWidgetMarkupModule::OnPostEngineInit()
{
	StartSourceFileWatching(UWidgetMarkupSettings::Get().SourceFileDirectoryPath);
}

void FWidgetMarkupModule::StartSourceFileWatching(const FDirectoryPath& InDirectoryPath)
{
	if (!ensure(!bSourceFileWatchingStarted))
	{
		return;
	}
	if (InDirectoryPath.Path.IsEmpty())
	{
		return;
	}
	// Convert the /Game/... package directory path to an absolute disk path.
	// Uses GetProjectFilePath() as anchor to avoid BaseDir-relative resolution errors
	// when the editor binary is not at the standard engine directory.
	static const FString GamePrefix = TEXT("/Game/");
	if (!InDirectoryPath.Path.StartsWith(GamePrefix))
	{
		UE_LOG(LogWidgetMarkup, Warning, TEXT("StartSourceFileWatching: SourceFileDirectoryPath '%s' is not a /Game/... package path, skipping."), *InDirectoryPath.Path);
		return;
	}
	const FString RelativeDir = InDirectoryPath.Path.Mid(GamePrefix.Len());
	const FString ProjectDir = FPaths::GetPath(FPaths::GetProjectFilePath());
	FString DirectoryPath = FPaths::Combine(ProjectDir, TEXT("Content"), RelativeDir);
	FPaths::NormalizeFilename(DirectoryPath);
	if (!FPaths::DirectoryExists(DirectoryPath))
	{
		return;
	}
	auto& DirectoryWatcherModule = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(FName("DirectoryWatcher"));
	if (auto DirectoryWatcher = DirectoryWatcherModule.Get())
	{
		if (DirectoryWatcher->RegisterDirectoryChangedCallback_Handle(
			DirectoryPath,
			IDirectoryWatcher::FDirectoryChanged::CreateRaw(this, &FWidgetMarkupModule::HandleOnSourceFileDirectoryChanged),
			SourceFileWatchingDelegateHandle
		))
		{
			SourceFileWatchingDirectoryPath = DirectoryPath;
			bSourceFileWatchingStarted = true;
			UE_LOG(LogWidgetMarkup, Display, TEXT("Start Source File Watching for '%s'."), *SourceFileWatchingDirectoryPath);
		}
	}
}

void FWidgetMarkupModule::StopSourceFileWatching()
{
	if (!bSourceFileWatchingStarted)
	{
		return;
	}
	if (SourceFileWatchingDirectoryPath.IsEmpty())
	{
		return;
	}
	if (auto DirectoryWatcherModule = FModuleManager::GetModulePtr<FDirectoryWatcherModule>(FName("DirectoryWatcher")))
	{
		if (auto DirectoryWatcher = DirectoryWatcherModule->Get())
		{
			DirectoryWatcher->UnregisterDirectoryChangedCallback_Handle(
				SourceFileWatchingDirectoryPath,
				SourceFileWatchingDelegateHandle
			);
		}
	}
	UE_LOG(LogWidgetMarkup, Display, TEXT("Stop Source File Watching for '%s'."), *SourceFileWatchingDirectoryPath);
	SourceFileWatchingDirectoryPath.Empty();
	bSourceFileWatchingStarted = false;
}

void FWidgetMarkupModule::HandleOnSourceFileDirectoryChanged(const TArray<struct FFileChangeData>& FileChanges)
{
	for (const auto& FileChangeData : FileChanges)
	{
		switch (FileChangeData.Action)
		{
		case FFileChangeData::FCA_Added:
		case FFileChangeData::FCA_Modified:
		case FFileChangeData::FCA_RescanRequired:
		{
			FString AbsoluteFilePath = FileChangeData.Filename;
			if (FPaths::IsRelative(AbsoluteFilePath))
			{
				AbsoluteFilePath = FPaths::Combine(SourceFileWatchingDirectoryPath, AbsoluteFilePath);
			}
			FPaths::NormalizeFilename(AbsoluteFilePath);
			FPaths::CollapseRelativeDirectories(AbsoluteFilePath);

			FString PackagePath;
			if (!TryConvertAbsoluteSourceFilePathToPackagePath(AbsoluteFilePath, TEXT(".unrealwidgetmarkup"), PackagePath))
			{
				UE_LOG(LogWidgetMarkup, Warning, TEXT("Source File Changed: could not convert to package path ('%s')."), *AbsoluteFilePath);
				break;
			}
			UE_LOG(LogWidgetMarkup, Display, TEXT("Source File Changed: '%s' -> '%s'."), *AbsoluteFilePath, *PackagePath);
			CompileFromPackagePath(PackagePath);
			break;
		}
		case FFileChangeData::FCA_Removed:
			break;
		case FFileChangeData::FCA_Unknown:
			break;
		default:
			ensureMsgf(false, TEXT("Not implemented for File Change Action: %d!"), FileChangeData.Action);
		}
	}
}

IMPLEMENT_MODULE(FWidgetMarkupModule, WidgetMarkup)

static FAutoConsoleCommand GWidgetMarkupCompile
(
	TEXT("WidgetMarkup.Compile"),
	TEXT("Compile for Widget Markup Blueprint Generated Class."),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 2)
		{
			return;
		}
		FWidgetMarkupModule& WidgetMarkupModule = FModuleManager::GetModuleChecked<FWidgetMarkupModule>("WidgetMarkup");
		WidgetMarkupModule.CompileFromSourceCode(*Args[0], *Args[1]);
	})
);

enum class ESlotCapacityPolicy
{
	None = 0,
	Single = 1,
	Multiple = 2,
};

struct FIntelliSenseDataGenerator
{
	FString GenerateBasicTypeIntelliSenseJsonObject(const FProperty* Property)
	{
		auto TypeName = Property->GetCPPType();
		if (!BasicTypeNames.Contains(TypeName))
		{
			BasicTypeNames.Add(TypeName);
			auto Data = MakeShared<FJsonObject>();
			Data->SetStringField(TEXT("name"), TypeName);
			TArray<TSharedPtr<FJsonValue>> Properties;
			Data->SetArrayField(TEXT("properties"), Properties);
			Types.Add(MakeShared<FJsonValueObject>(Data));
			if (TypeName == TEXT("float") || TypeName == TEXT("double"))
			{
				Data->SetStringField(TEXT("pattern"), TEXT("^[+-]?(?:(\\d+\\.?\\d*|\\.\\d+)(?:[Ee][+-]?\\d+)?|INF|NaN)$"));
			}
			else if (TypeName == TEXT("int"))
			{
				Data->SetStringField(TEXT("pattern"), TEXT("^[+-]?\\d+$"));
			}
			else
			{
				UE_LOG(LogWidgetMarkup, Warning, TEXT("No regex pattern defined for type %s!"), *TypeName);
			}
		}
		return TypeName;
	}

	FString GenerateEnumIntelliSenseJsonObject(const FEnumProperty* EnumProperty)
	{
		check(EnumProperty);
		auto Enum = EnumProperty->GetEnum();
		check(Enum);
		if (EnumClasses.Contains(Enum))
		{
			return Enum->GetName();
		}
		EnumClasses.Add(Enum);
		auto Data = MakeShared<FJsonObject>();
		Data->SetStringField(TEXT("name"), Enum->GetName());
		TArray<TSharedPtr<FJsonValue>> Properties;
		Data->SetArrayField(TEXT("properties"), Properties);
		Types.Add(MakeShared<FJsonValueObject>(Data));
		TArray<TSharedPtr<FJsonValue>> Choices;
		for (int32 Index = 0; Index < Enum->NumEnums(); Index++)
		{
			auto EnumValue = Enum->GetValueByIndex(Index);
			if (EnumValue == Enum->GetMaxEnumValue())
			{
				continue;
			}
			auto Name = Enum->GetNameStringByValue(EnumValue);
			Choices.Add(MakeShared<FJsonValueString>(Name));
		}
		Data->SetArrayField(TEXT("choices"), Choices);
		return Enum->GetName();
	}

	FString GenerateObjectOrStructIntelliSenseJsonObject(const UStruct* StructOrClass)
	{
		check(StructOrClass);
		if (StructOrClasses.Contains(StructOrClass))
		{
			return StructOrClass->GetName();
		}
		StructOrClasses.Add(StructOrClass);
		auto Data = MakeShared<FJsonObject>();
		Data->SetStringField(TEXT("name"), StructOrClass->GetName());
		if (auto Class = Cast<UClass>(StructOrClass))
		{
			if (auto SuperClass = Class->GetSuperClass())
			{
				GenerateObjectOrStructIntelliSenseJsonObject(SuperClass);
				Data->SetStringField(TEXT("super"), SuperClass->GetName());
			}
			if (!(Class->ClassFlags & CLASS_Abstract))
			{
				if (Class->IsChildOf(UWidget::StaticClass()))
				{
					if (Class->IsChildOf(UPanelWidget::StaticClass()))
					{
						auto TemporaryPanelWidget = NewObject<UPanelWidget>(GetTransientPackage(), Class);
						auto TemporaryChild = NewObject<UImage>(TemporaryPanelWidget);
						auto TemporarySlot = TemporaryPanelWidget->AddChild(TemporaryChild);
						auto SlotClass = TemporarySlot->GetClass();
						GenerateObjectOrStructIntelliSenseJsonObject(SlotClass);
						Data->SetStringField(TEXT("slotClass"), SlotClass->GetName());
						if (TemporaryPanelWidget->CanHaveMultipleChildren())
						{
							Data->SetNumberField(TEXT("slotCapacityPolicy"), (int32)ESlotCapacityPolicy::Multiple);
						}
						else
						{
							Data->SetNumberField(TEXT("slotCapacityPolicy"), (int32)ESlotCapacityPolicy::Single);
						}
					}
					else
					{
						Data->SetNumberField(TEXT("slotCapacityPolicy"), (int32)ESlotCapacityPolicy::None);
					}
				}
			}
		}
		TArray<TSharedPtr<FJsonValue>> Properties;
		for (TFieldIterator<FProperty> PropertyIt(StructOrClass, EFieldIteratorFlags::ExcludeSuper); PropertyIt; ++PropertyIt)
		{
			auto Property = *PropertyIt;
			FString Name = Property->GetName();
			FString Type;
			if (auto ObjectProperty = CastField<FObjectProperty>(Property))
			{
				Type = GenerateObjectOrStructIntelliSenseJsonObject(ObjectProperty->PropertyClass);
			}
			else if (auto StructProperty = CastField<FStructProperty>(Property))
			{
				Type = GenerateObjectOrStructIntelliSenseJsonObject(StructProperty->Struct);
			}
			else if (auto EnumProperty = CastField<FEnumProperty>(Property))
			{
				Type = GenerateEnumIntelliSenseJsonObject(EnumProperty);
			}
			else
			{
				Type = GenerateBasicTypeIntelliSenseJsonObject(Property);
			}
			auto PropertyData = MakeShared<FJsonObject>();
			PropertyData->SetStringField(TEXT("name"), Name);
			PropertyData->SetStringField(TEXT("type"), Type);
			PropertyData->SetBoolField(TEXT("hasSetter"), Property->HasSetter());
			Properties.Add(MakeShared<FJsonValueObject>(PropertyData));
		}
		Data->SetArrayField(TEXT("properties"), Properties);
		Types.Add(MakeShared<FJsonValueObject>(Data));
		return StructOrClass->GetName();
	}

	auto Generate()
	{
		auto Data = MakeShared<FJsonObject>();
		for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
		{
			UClass* Class = *ClassIt;
			if (Class->HasAnyClassFlags(CLASS_Abstract))
			{
				continue;
			}
			if (!Class->GetPackage()->GetName().StartsWith(TEXT("/Script/UMG")))
			{
				continue;
			}
			if (Class->IsChildOf(UWidget::StaticClass()) && Class != UWidget::StaticClass() && !Class->IsChildOf(UUserWidget::StaticClass()))
			{
				GenerateObjectOrStructIntelliSenseJsonObject(Class);
			}
		}
		Data->SetArrayField(TEXT("types"), MoveTemp(Types));
		return Data;
	}

	TArray<TSharedPtr<FJsonValue>> Types;
	TSet<FString> BasicTypeNames;
	TSet<const UStruct*> StructOrClasses;
	TSet<UEnum*> EnumClasses;
};

static FAutoConsoleCommand GWidgetMarkupGenerateIntelliSenseData
(
	TEXT("WidgetMarkup.GenerateIntellisenseData"),
	TEXT("Generate Intellisense Data."),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() < 1)
		{
			return;
		}
		auto FilePath = Args[0];
		FIntelliSenseDataGenerator Generator;
		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(Generator.Generate(), Writer);
		if (FPaths::IsRelative(FilePath))
		{
			FilePath = *FPaths::ProjectSavedDir() / FilePath;
		}
		FFileHelper::SaveStringToFile(OutputString, *FilePath);
	})
);

#undef LOCTEXT_NAMESPACE