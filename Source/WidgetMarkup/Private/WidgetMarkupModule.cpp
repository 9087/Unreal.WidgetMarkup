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
#include "Engine/Blueprint.h"
#include "Components/ContentWidget.h"
#include "Components/Image.h"
#include "Components/ListView.h"
#include "Components/Widget.h"
#include "Components/SlateWrapperTypes.h"
#include "Converters/BooleanConverter.h"
#include "Converters/ClassConverter.h"
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
#include "ElementNodes/BlueprintElementNode.h"
#include "PropertySetters/ListViewListItemsPropertySetter.h"
#include "PropertyRuns/BlueprintImplementsPropertyRun.h"
#include "PropertyRuns/BlueprintSuperPropertyRun.h"
#include "PropertyRuns/ListViewListItemsPropertyRun.h"
#include "PropertyRuns/ObjectNamePropertyRun.h"
#include "Utilities/WidgetPropertyPath.h"
#include "ElementNodes/ContentWidgetElementNode.h"
#include "ElementNodes/PanelWidgetElementNode.h"
#include "ElementNodes/WidgetBlueprintElementNode.h"
#include "ElementNodes/WidgetElementNode.h"
#include "ElementNodes/WidgetTreeElementNode.h"
#include "Misc/PackageName.h"

#define LOCTEXT_NAMESPACE "WidgetMarkup"

DEFINE_LOG_CATEGORY(LogWidgetMarkup);

void FWidgetMarkupModule::StartupModule()
{
	FElementNodeFactory::Get().Register<UWidgetTree>(FElementNodeFactory::FOnCreateElementNode::CreateStatic(FWidgetTreeElementNode::Create));
	FElementNodeFactory::Get().Register<UWidget>(FElementNodeFactory::FOnCreateElementNode::CreateStatic(FWidgetElementNode::Create));
	FElementNodeFactory::Get().Register<UPanelWidget>(FElementNodeFactory::FOnCreateElementNode::CreateStatic(FPanelWidgetElementNode::Create));
	FElementNodeFactory::Get().Register<UContentWidget>(FElementNodeFactory::FOnCreateElementNode::CreateStatic(FContentWidgetElementNode::Create));
	FElementNodeFactory::Get().Register<UBlueprint>(FElementNodeFactory::FOnCreateElementNode::CreateStatic(FBlueprintElementNode::Create));
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
	FConverterRegistry::Get().Register(FClassProperty::StaticClass()->GetFName(), FConverterRegistry::FOnCreateConverter::CreateStatic(FClassConverter::Create));
	FConverterRegistry::Get().Register(FSoftClassProperty::StaticClass()->GetFName(), FConverterRegistry::FOnCreateConverter::CreateStatic(FClassConverter::Create));
	FConverterRegistry::Get().Register(NAME_Color, FConverterRegistry::FOnCreateConverter::CreateStatic(FColorConverter::Create));
	FConverterRegistry::Get().Register(NAME_LinearColor, FConverterRegistry::FOnCreateConverter::CreateStatic(FLinearColorConverter::Create));
	FConverterRegistry::Get().Register(FMargin::StaticStruct()->GetFName(), FConverterRegistry::FOnCreateConverter::CreateStatic(FMarginConverter::Create));
	FConverterRegistry::Get().Register(FSlateChildSize::StaticStruct()->GetFName(), FConverterRegistry::FOnCreateConverter::CreateStatic(FSlateChildSizeConverter::Create));
	FConverterRegistry::Get().Register(FSlateColor::StaticStruct()->GetFName(), FConverterRegistry::FOnCreateConverter::CreateStatic(FSlateColorConverter::Create));
	FConverterRegistry::Get().Register(NAME_Vector, FConverterRegistry::FOnCreateConverter::CreateStatic(TVectorConverter<FVector::FReal, 3>::Create));
	FConverterRegistry::Get().Register(NAME_Vector2D, FConverterRegistry::FOnCreateConverter::CreateStatic(TVectorConverter<FVector2D::FReal, 2>::Create));
	
	RegisterCustomPropertyRun(UObject::StaticClass(), TEXT("Name"), FOnCreatePropertyRun::CreateStatic(&FObjectNamePropertyRun::Create));
	RegisterCustomPropertyRun(UBlueprint::StaticClass(), TEXT("Super"), FOnCreatePropertyRun::CreateStatic(&FBlueprintSuperPropertyRun::Create));
	RegisterCustomPropertyRun(UBlueprint::StaticClass(), TEXT("Implements"), FOnCreatePropertyRun::CreateStatic(&FBlueprintImplementsPropertyRun::Create));
	RegisterCustomPropertyRun(UListView::StaticClass(), TEXT("ListItems"), FOnCreatePropertyRun::CreateStatic(&FListViewListItemsPropertyRun::Create));
	RegisterCustomPropertySetter(UListView::StaticClass(), TEXT("ListItems"), FOnCreatePropertySetter::CreateStatic(&FListViewListItemsPropertySetter::Create));
	
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FWidgetMarkupModule::OnPostEngineInit);
}

void FWidgetMarkupModule::ShutdownModule()
{
	FElementNodeFactory::Get().Unregister<UWidgetTree>();
	FElementNodeFactory::Get().Unregister<UWidget>();
	FElementNodeFactory::Get().Unregister<UPanelWidget>();
	FElementNodeFactory::Get().Unregister<UContentWidget>();
	FElementNodeFactory::Get().Unregister<UWidgetBlueprint>();
	FElementNodeFactory::Get().Unregister<UBlueprint>();

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
	FConverterRegistry::Get().Unregister(FClassProperty::StaticClass()->GetFName());
	FConverterRegistry::Get().Unregister(FSoftClassProperty::StaticClass()->GetFName());
	FConverterRegistry::Get().Unregister(NAME_Color);
	FConverterRegistry::Get().Unregister(NAME_LinearColor);
	FConverterRegistry::Get().Unregister(FMargin::StaticStruct()->GetFName());
	FConverterRegistry::Get().Unregister(FSlateChildSize::StaticStruct()->GetFName());
	FConverterRegistry::Get().Unregister(FSlateColor::StaticStruct()->GetFName());
	FConverterRegistry::Get().Unregister(NAME_Vector);
	FConverterRegistry::Get().Unregister(NAME_Vector2D);

	UnregisterCustomPropertyRun(UObject::StaticClass(), TEXT("Name"));
	UnregisterCustomPropertyRun(UBlueprint::StaticClass(), TEXT("Super"));
	UnregisterCustomPropertyRun(UBlueprint::StaticClass(), TEXT("Implements"));
	UnregisterCustomPropertyRun(UListView::StaticClass(), TEXT("ListItems"));
	UnregisterCustomPropertySetter(UListView::StaticClass(), TEXT("ListItems"));

	FCoreDelegates::OnPostEngineInit.RemoveAll(this);
	StopSourceFileWatching();
	PropertyRunCreateDelegates.Empty();
	PropertySetterCreateDelegates.Empty();
}

// ---------------------------------------------------------------------------
// Path helpers: PackagePath (/Game/.../AssetName) <-> absolute disk file path
// ---------------------------------------------------------------------------

// /Game/WidgetMarkup/Foo  ->  <ContentDir>/WidgetMarkup/Foo.unrealwidgetmarkup
// /PluginName/Foo         ->  <PluginContentDir>/Foo.unrealwidgetmarkup
// Uses FPackageName to support all mounted content roots (project + plugins).
static bool TryConvertPackagePathToAbsoluteSourceFilePath(const FString& PackagePath, FStringView Extension, FString& OutAbsoluteFilePath)
{
	return FPackageName::TryConvertLongPackageNameToFilename(PackagePath, OutAbsoluteFilePath, FString(Extension));
}

// <ContentDir>/WidgetMarkup/Foo.unrealwidgetmarkup    ->  /Game/WidgetMarkup/Foo
// <PluginContentDir>/Foo.unrealwidgetmarkup           ->  /PluginName/Foo
// Uses FPackageName to support all mounted content roots (project + plugins).
static bool TryConvertAbsoluteSourceFilePathToPackagePath(const FString& AbsoluteFilePath, FStringView Extension, FString& OutPackagePath)
{
	if (FPaths::IsRelative(AbsoluteFilePath))
	{
		return false;
	}

	// Strip custom extension before passing to FPackageName (it only knows .uasset/.umap etc.)
	FString FilePathWithoutExt = AbsoluteFilePath;
	const FString ExtensionString(Extension);
	if (FilePathWithoutExt.EndsWith(ExtensionString))
	{
		FilePathWithoutExt = FilePathWithoutExt.LeftChop(ExtensionString.Len());
	}
	return FPackageName::TryConvertFilenameToLongPackageName(FilePathWithoutExt, OutPackagePath);
}

// ---------------------------------------------------------------------------

TSharedPtr<IPropertyRun> FWidgetMarkupModule::CreateCustomPropertyRun(UStruct* InStruct, FName InPropertyPath) const
{
	const FString PropertyPathString = InPropertyPath.ToString();

	if (!InStruct)
	{
		return nullptr;
	}

	UStruct* BestStruct = nullptr;
	const TMap<FWidgetPropertyPath, FOnCreatePropertyRun>* BestRegistry = nullptr;
	for (const auto& KeyValuePair : PropertyRunCreateDelegates)
	{
		UStruct* Struct = KeyValuePair.Key.Get();
		if (!Struct)
		{
			continue;
		}
		if (InStruct->IsChildOf(Struct))
		{
			if (!BestStruct || Struct->IsChildOf(BestStruct))
			{
				BestStruct = Struct;
				BestRegistry = &KeyValuePair.Value;
			}
		}
	}
	if (!BestRegistry)
	{
		return nullptr;
	}

	FWidgetPropertyPath PropertyPath;
	FString ParseError;
	if (!FWidgetPropertyPath::TryParse(PropertyPathString, PropertyPath, &ParseError))
	{
		return nullptr;
	}

	auto Found = BestRegistry->Find(PropertyPath);
	if (!Found || !Found->IsBound())
	{
		return nullptr;
	}
	return Found->Execute();
}

TSharedRef<IPropertyRun> FWidgetMarkupModule::CreatePropertyRun(UStruct* InStruct, FName InPropertyPath) const
{
	TSharedPtr<IPropertyRun> CustomPropertyRun = CreateCustomPropertyRun(InStruct, InPropertyPath);
	if (CustomPropertyRun.IsValid())
	{
		return CustomPropertyRun.ToSharedRef();
	}
	return MakeShared<FPropertyRun>();
}

TSharedPtr<FPropertySetter> FWidgetMarkupModule::CreateCustomPropertySetter(UStruct* InStruct, FName InPropertyPath) const
{
	const FString PropertyPathString = InPropertyPath.ToString();

	if (!InStruct)
	{
		return nullptr;
	}

	UStruct* BestStruct = nullptr;
	const TMap<FWidgetPropertyPath, FOnCreatePropertySetter>* BestRegistry = nullptr;
	for (const auto& KeyValuePair : PropertySetterCreateDelegates)
	{
		UStruct* Struct = KeyValuePair.Key.Get();
		if (!Struct)
		{
			continue;
		}
		if (InStruct->IsChildOf(Struct))
		{
			if (!BestStruct || Struct->IsChildOf(BestStruct))
			{
				BestStruct = Struct;
				BestRegistry = &KeyValuePair.Value;
			}
		}
	}
	if (!BestRegistry)
	{
		return nullptr;
	}

	FWidgetPropertyPath PropertyPath;
	FString ParseError;
	if (!FWidgetPropertyPath::TryParse(PropertyPathString, PropertyPath, &ParseError))
	{
		return nullptr;
	}

	auto Found = BestRegistry->Find(PropertyPath);
	if (!Found || !Found->IsBound())
	{
		return nullptr;
	}
	return Found->Execute();
}

UObject* FWidgetMarkupModule::CompileFromSourceCode(FName PackagePath, const FString& XML)
{
	// Normalize to long package path so object map keys are always long names.
	const FString PackagePathString = PackagePath.ToString();
	const FString LongPackagePath = PackagePathString.StartsWith(TEXT("/")) ? PackagePathString : FString::Printf(TEXT("/WidgetMarkup/%s"), *PackagePathString);
	const FName LongPackagePathName(*LongPackagePath);
	auto Package = CreatePackage(*LongPackagePath);
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
		UE_LOG(LogWidgetMarkup, Error, TEXT("CompileFromSourceCode failed: no root element produced for '%s'."), *LongPackagePath);
		return nullptr;
	}
	auto Object = RootElementNode->GetObject();
	Objects.FindOrAdd(LongPackagePathName) = Object;
	GetOnObjectCompiled().Broadcast(LongPackagePathName, Object);
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

UObject* FWidgetMarkupModule::GetObjectFromPackagePath(const FString& PackagePath)
{
	auto Object = Objects.Find(FName(PackagePath));
	return Object ? *Object : nullptr;
}

UObject* FWidgetMarkupModule::GetObjectOrCompileFromPackage(const FString& PackagePath)
{
	if (UObject* Object = GetObjectFromPackagePath(PackagePath))
	{
		return Object;
	}
	return CompileFromPackagePath(PackagePath);
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

bool FWidgetMarkupModule::RegisterCustomPropertyRun(UStruct* InStruct, FName InPropertyPath, FOnCreatePropertyRun InOnCreatePropertyRun)
{
	const FString PropertyPathString = InPropertyPath.ToString();

	FWidgetPropertyPath PropertyPath;
	FString ParseError;
	if (!FWidgetPropertyPath::TryParse(PropertyPathString, PropertyPath, &ParseError))
	{
		UE_LOG(LogWidgetMarkup, Error, TEXT("RegisterCustomProperty failed: invalid PropertyPath '%s' for Struct '%s': %s"), *PropertyPathString, *InStruct->GetName(), *ParseError);
		return false;
	}
	auto& Registry = PropertyRunCreateDelegates.FindOrAdd(InStruct);
	if (Registry.Contains(PropertyPath))
	{
		UE_LOG(LogWidgetMarkup, Warning, TEXT("RegisterCustomProperty: overwriting existing entry (Struct='%s', PropertyPath='%s')."), *InStruct->GetName(), *PropertyPath.GetPathName().ToString());
	}
	Registry.Add(PropertyPath, InOnCreatePropertyRun);
	return true;
}

bool FWidgetMarkupModule::RegisterCustomPropertySetter(UStruct* InStruct, FName InPropertyPath, FOnCreatePropertySetter InOnCreatePropertySetter)
{
	const FString PropertyPathString = InPropertyPath.ToString();

	FWidgetPropertyPath PropertyPath;
	FString ParseError;
	if (!FWidgetPropertyPath::TryParse(PropertyPathString, PropertyPath, &ParseError))
	{
		UE_LOG(LogWidgetMarkup, Error, TEXT("RegisterCustomPropertySetter failed: invalid PropertyPath '%s' for Struct '%s': %s"), *PropertyPathString, *InStruct->GetName(), *ParseError);
		return false;
	}
	auto& Registry = PropertySetterCreateDelegates.FindOrAdd(InStruct);
	if (Registry.Contains(PropertyPath))
	{
		UE_LOG(LogWidgetMarkup, Warning, TEXT("RegisterCustomPropertySetter: overwriting existing entry (Struct='%s', PropertyPath='%s')."), *InStruct->GetName(), *PropertyPath.GetPathName().ToString());
	}
	Registry.Add(PropertyPath, InOnCreatePropertySetter);
	return true;
}

void FWidgetMarkupModule::UnregisterCustomPropertyRun(UStruct* InStruct, FName InPropertyPath)
{
	const FString PropertyPathString = InPropertyPath.ToString();

	if (!InStruct)
	{
		return;
	}

	FWidgetPropertyPath PropertyPath;
	FString ParseError;
	if (!FWidgetPropertyPath::TryParse(PropertyPathString, PropertyPath, &ParseError))
	{
		UE_LOG(LogWidgetMarkup, Warning, TEXT("UnregisterCustomProperty ignored invalid PropertyPath '%s' for Struct '%s': %s"), *PropertyPathString, *InStruct->GetName(), *ParseError);
		return;
	}
	auto* Registry = PropertyRunCreateDelegates.Find(InStruct);
	if (Registry)
	{
		Registry->Remove(PropertyPath);
		if (Registry->IsEmpty())
		{
			PropertyRunCreateDelegates.Remove(InStruct);
		}
	}
}

void FWidgetMarkupModule::UnregisterCustomPropertySetter(UStruct* InStruct, FName InPropertyPath)
{
	const FString PropertyPathString = InPropertyPath.ToString();

	if (!InStruct)
	{
		return;
	}

	FWidgetPropertyPath PropertyPath;
	FString ParseError;
	if (!FWidgetPropertyPath::TryParse(PropertyPathString, PropertyPath, &ParseError))
	{
		UE_LOG(LogWidgetMarkup, Warning, TEXT("UnregisterCustomPropertySetter ignored invalid PropertyPath '%s' for Struct '%s': %s"), *PropertyPathString, *InStruct->GetName(), *ParseError);
		return;
	}
	auto* Registry = PropertySetterCreateDelegates.Find(InStruct);
	if (Registry)
	{
		Registry->Remove(PropertyPath);
		if (Registry->IsEmpty())
		{
			PropertySetterCreateDelegates.Remove(InStruct);
		}
	}
}

void FWidgetMarkupModule::OnPostEngineInit()
{
	for (const FDirectoryPath& DirectoryPath : UWidgetMarkupSettings::Get().SourceFileDirectoryPaths)
	{
		StartSourceFileWatching(DirectoryPath);
	}
}

void FWidgetMarkupModule::StartSourceFileWatching(const FDirectoryPath& InDirectoryPath)
{
	if (InDirectoryPath.Path.IsEmpty())
	{
		return;
	}
	// Convert the package directory path to an absolute disk path.
	// FPackageName supports all mounted content roots (/Game/, /PluginName/, etc.).
	FString DirectoryPath;
	if (!FPackageName::TryConvertLongPackageNameToFilename(InDirectoryPath.Path + TEXT("/"), DirectoryPath))
	{
		UE_LOG(LogWidgetMarkup, Warning, TEXT("StartSourceFileWatching: could not convert '%s' to a disk path, skipping."), *InDirectoryPath.Path);
		return;
	}
	FPaths::NormalizeDirectoryName(DirectoryPath);
	if (!FPaths::DirectoryExists(DirectoryPath))
	{
		return;
	}
	if (WatchedDirectories.Contains(DirectoryPath))
	{
		return;
	}
	auto& DirectoryWatcherModule = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(FName("DirectoryWatcher"));
	if (auto DirectoryWatcher = DirectoryWatcherModule.Get())
	{
		FDelegateHandle Handle;
		if (DirectoryWatcher->RegisterDirectoryChangedCallback_Handle(
			DirectoryPath,
			IDirectoryWatcher::FDirectoryChanged::CreateLambda([this, DirectoryPath](const TArray<FFileChangeData>& FileChanges)
			{
				HandleOnSourceFileDirectoryChanged(FileChanges, DirectoryPath);
			}),
			Handle
		))
		{
			WatchedDirectories.Add(DirectoryPath, Handle);
			UE_LOG(LogWidgetMarkup, Display, TEXT("Start Source File Watching for '%s'."), *DirectoryPath);
		}
	}
}

void FWidgetMarkupModule::StopSourceFileWatching()
{
	if (WatchedDirectories.IsEmpty())
	{
		return;
	}
	if (auto DirectoryWatcherModule = FModuleManager::GetModulePtr<FDirectoryWatcherModule>(FName("DirectoryWatcher")))
	{
		if (auto DirectoryWatcher = DirectoryWatcherModule->Get())
		{
			for (auto& Pair : WatchedDirectories)
			{
				DirectoryWatcher->UnregisterDirectoryChangedCallback_Handle(Pair.Key, Pair.Value);
				UE_LOG(LogWidgetMarkup, Display, TEXT("Stop Source File Watching for '%s'."), *Pair.Key);
			}
		}
	}
	WatchedDirectories.Empty();
}

void FWidgetMarkupModule::HandleOnSourceFileDirectoryChanged(const TArray<struct FFileChangeData>& FileChanges, const FString& WatchedDirectory)
{
	for (const auto& FileChangeData : FileChanges)
	{
		switch (FileChangeData.Action)
		{
		case FFileChangeData::FCA_Added:
		case FFileChangeData::FCA_Modified:
		case FFileChangeData::FCA_RescanRequired:
		{
			// FileChangeData.Filename may be relative to the watched directory
			FString AbsoluteFilePath = FileChangeData.Filename;
			if (FPaths::IsRelative(AbsoluteFilePath))
			{
				AbsoluteFilePath = FPaths::Combine(WatchedDirectory, AbsoluteFilePath);
			}
			// Convert to fully qualified absolute path, resolving all .. segments
			AbsoluteFilePath = FPaths::ConvertRelativePathToFull(AbsoluteFilePath);
			FPaths::NormalizeFilename(AbsoluteFilePath);

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