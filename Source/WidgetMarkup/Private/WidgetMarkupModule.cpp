// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "WidgetMarkupModule.h"

#include "ConverterRegistry.h"
#include "Misc/PackageName.h"
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

void FWidgetMarkupModule::StartupModule()
{
	FElementNodeFactory::Get().Register<UWidgetTree>(FElementNodeFactory::FOnCreateElementNode::CreateStatic(FWidgetTreeElementNode::Create));
	FElementNodeFactory::Get().Register<UWidget>(FElementNodeFactory::FOnCreateElementNode::CreateStatic(FWidgetElementNode::Create));
	FElementNodeFactory::Get().Register<UPanelWidget>(FElementNodeFactory::FOnCreateElementNode::CreateStatic(FPanelWidgetElementNode::Create));
	FElementNodeFactory::Get().Register<UContentWidget>(FElementNodeFactory::FOnCreateElementNode::CreateStatic(FContentWidgetElementNode::Create));
	FElementNodeFactory::Get().Register<UWidgetBlueprint>(FElementNodeFactory::FOnCreateElementNode::CreateStatic(FWidgetBlueprintElementNode::Create));

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

UObject* FWidgetMarkupModule::CompileFromSourceCode(FName Name, const FString& XML)
{
	auto PackageName = FString::Printf(TEXT("/WidgetMarkup/%s"), *Name.ToString());
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

FString FWidgetMarkupModule::ToAbsolutePath(const FString& SourceFilePath)
{
	// Single convention: path is relative to project content directory (no package/asset concept for .unrealwidgetmarkup)
	const FString Path = SourceFilePath.TrimStartAndEnd();
	if (Path.IsEmpty())
	{
		return FString();
	}
	if (FPaths::IsRelative(Path))
	{
		return FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectContentDir(), Path));
	}
	return FPaths::ConvertRelativePathToFull(Path);
}

UObject* FWidgetMarkupModule::CompileFromFile(const FString& SourceFilePath)
{
	UE_LOG(LogWidgetMarkup, Display, TEXT("Compile Source File '%s'."), *SourceFilePath);

	const FString AbsolutePath = ToAbsolutePath(SourceFilePath);
	if (AbsolutePath.IsEmpty())
	{
		UE_LOG(LogWidgetMarkup, Error, TEXT("CompileFromFile failed: source path is empty or invalid ('%s')."), *SourceFilePath);
		return nullptr;
	}
	FString XML;
	if (!FFileHelper::LoadFileToString(XML, *AbsolutePath, FFileHelper::EHashOptions::None, FILEREAD_AllowWrite) || XML.IsEmpty())
	{
		UE_LOG(LogWidgetMarkup, Error, TEXT("CompileFromFile failed: could not read file or file is empty ('%s')."), *AbsolutePath);
		return nullptr;
	}
	FString ObjectPath;
	if (!ConvertFilePathToObjectPath(AbsolutePath, ObjectPath))
	{
		UE_LOG(LogWidgetMarkup, Error, TEXT("CompileFromFile failed: path is not under configured source directory ('%s')."), *AbsolutePath);
		return nullptr;
	}
	return CompileFromSourceCode(FName(ObjectPath), XML);
}

UObject* FWidgetMarkupModule::GetObjectFromName(FName Name)
{
	auto Object = Objects.Find(Name);
	return Object ? *Object : nullptr;
}

UObject* FWidgetMarkupModule::GetObjectFromFile(const FString& SourceFilePath)
{
	const FString AbsolutePath = ToAbsolutePath(SourceFilePath);
	if (AbsolutePath.IsEmpty())
	{
		return nullptr;
	}
	FString ObjectPath;
	if (!ConvertFilePathToObjectPath(AbsolutePath, ObjectPath))
	{
		return nullptr;
	}
	return GetObjectFromName(FName(ObjectPath));
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

bool FWidgetMarkupModule::ConvertFilePathToObjectPath(const FString& FilePath, FString& OutObjectPath)
{
	auto SourceFileFullPath = FPaths::ConvertRelativePathToFull(FilePath);
	FString SourceFileDirectoryPath;
	if (!FPackageName::TryConvertLongPackageNameToFilename(GetDefault<UWidgetMarkupSettings>()->SourceFileDirectoryPath.Path, SourceFileDirectoryPath))
	{
		return false;
	}
	SourceFileDirectoryPath = FPaths::ConvertRelativePathToFull(SourceFileDirectoryPath);
	if (!FPaths::DirectoryExists(SourceFileDirectoryPath))
	{
		return false;
	}
	if (!SourceFileFullPath.StartsWith(SourceFileDirectoryPath))
	{
		return false;
	}
	// Use full path for MakePathRelativeTo so both paths are absolute (FilePath may be relative)
	FString ObjectPath = SourceFileFullPath;
	if (!FPaths::MakePathRelativeTo(ObjectPath, *SourceFileDirectoryPath))
	{
		return false;
	}
	OutObjectPath = FPaths::GetBaseFilename(ObjectPath, false);
	return true;
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
	FString DirectoryPath;
	if (!FPackageName::TryConvertLongPackageNameToFilename(InDirectoryPath.Path, DirectoryPath))
	{
		return;
	}
	DirectoryPath = FPaths::ConvertRelativePathToFull(DirectoryPath);
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
		UE_LOG(LogWidgetMarkup, Display, TEXT("Source File Directory Changed: %d, '%s'."), FileChangeData.Action , *SourceFileWatchingDirectoryPath);
		switch (FileChangeData.Action)
		{
		case FFileChangeData::FCA_Added:
		case FFileChangeData::FCA_Modified:
		case FFileChangeData::FCA_RescanRequired:
			CompileFromFile(FileChangeData.Filename);
			break;
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