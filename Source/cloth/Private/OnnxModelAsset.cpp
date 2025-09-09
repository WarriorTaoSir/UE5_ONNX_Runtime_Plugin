// OnnxModelAsset.cpp

#include "OnnxModelAsset.h"
#if WITH_EDITOR
#include "Misc/Paths.h" // Required for path conversion (需要用来转换路径)


// 必须包含ONNX Runtime API才能解析模型。
#if PLATFORM_WINDOWS && PLATFORM_64BITS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif

// Include the ONNX Runtime C++ API header.
#include "onnxruntime_cxx_api.h" 

#if PLATFORM_WINDOWS && PLATFORM_64BITS
#include "Windows/HideWindowsPlatformTypes.h"
#endif

void UOnnxModelAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // 获取被修改的属性的名称。
	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    // 检查被修改的属性是否是我们的"ModelFile"。
    if (PropertyName == GET_MEMBER_NAME_CHECKED(UOnnxModelAsset, modelFile_))
    {
        // 清除之前的元数据。
        inputNodeNames_.Empty();
		outputNodeNames_.Empty();

        // 检查文件路径是否有效。
		if (modelFile_.FilePath.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("ONNX Model file path is empty for asset: %s"), *GetName());
			return;
		}
        // 将相对项目路径转换为ONNX Runtime可以读取的绝对路径。
        const FString absolutePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir(), modelFile_.FilePath);
        try
        {
            // 创建一个临时的环境和会话来检查模型。
            Ort::Env TempEnv(ORT_LOGGING_LEVEL_WARNING, "ModelAsset_Editor_Parse");
            Ort::SessionOptions SessionOptions;
            
            // ONNX Runtime期望一个宽字符字符串作为路径。
            Ort::Session TempSession(TempEnv, TCHAR_TO_WCHAR(*absolutePath), SessionOptions);

            // 使用分配器来管理名称的内存。
			Ort::AllocatorWithDefaultOptions Allocator;

            // 获取输入节点信息
			for (size_t i = 0; i < TempSession.GetInputCount(); ++i)
			{
				char* TempInputName = TempSession.GetInputNameAllocated(i, Allocator).get();
				inputNodeNames_.Add(FString(TempInputName));

			}

            // 获取输出节点信息
			for (size_t i = 0; i < TempSession.GetOutputCount(); ++i)
			{
				char* TempOutputName = TempSession.GetOutputNameAllocated(i, Allocator).get();
				outputNodeNames_.Add(FString(TempOutputName));

			}
            UE_LOG(LogTemp, Log, TEXT("Successfully parsed metadata for ONNX model: %s"), *GetName());
        }
        catch(const Ort::Exception& e)
        {
            // 如果解析失败，记录来自ONNX Runtime的错误。
			UE_LOG(LogTemp, Error, TEXT("Failed to parse ONNX model: %s. ERROR: %hs"), *GetName(), e.what());
        }
    }
}

#endif // WITH_EDITOR