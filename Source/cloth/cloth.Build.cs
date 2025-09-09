// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class cloth : ModuleRules
{
	public cloth(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        // ʹ���Լ���ONNX Runtime 1.20�汾����NNE�汾ƥ�䣩
        string OnnxRuntimePath = Path.Combine(PluginDirectory, "ThirdParty", "OnnxRuntime");
        string includePath = Path.Combine(OnnxRuntimePath, "include");
        PublicIncludePaths.AddRange(new string[] { includePath });

        // ����ONNX Runtime���ļ�
        string libPath = Path.Combine(OnnxRuntimePath, "lib");
        PublicAdditionalLibraries.Add(Path.Combine(libPath, "onnxruntime.lib"));

        // ��������ʱDLL����
        RuntimeDependencies.Add(Path.Combine(libPath, "onnxruntime.dll"));
        RuntimeDependencies.Add(Path.Combine(libPath, "onnxruntime_providers_shared.dll"));

        // �����ӳټ���DLL
        PublicDelayLoadDLLs.Add("onnxruntime.dll");
        PublicDelayLoadDLLs.Add("onnxruntime_providers_shared.dll");
        PrivateIncludePaths.AddRange(
            new string[] {
				// ... add other private include paths required here ...
			}
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "Projects"
				// ... add other public dependencies that you statically link with here ...
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "ImageWrapper"
				// ... add private dependencies that you statically link with here ...	
			}
            );

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "UnrealEd",
                "ToolMenus",
                "EditorStyle",
                "EditorWidgets",
                "PropertyEditor"
            });
        }


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );
    }
}
