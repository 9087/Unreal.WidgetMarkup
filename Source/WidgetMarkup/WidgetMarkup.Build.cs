// Copyright 2025 Wu Zhiwei. All Rights Reserved.

using UnrealBuildTool;

public class WidgetMarkup : ModuleRules
{
	public WidgetMarkup(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		OptimizeCode = CodeOptimization.Never;
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"UMG",
			"UMGEditor",
			"BlueprintGraph",
			"UnrealEd",
			"PropertyEditor",
			"XmlParser",
			"DirectoryWatcher",
			"DeveloperSettings",
			"Json",
		});
	}
}
