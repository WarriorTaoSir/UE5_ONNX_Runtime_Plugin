//OnnxModelInstance.cpp

#include "OnnxModelInstance.h"
#include "OnnxModelAsset.h"

// 包含ONNX Runtime的实现头文件
#if PLATFORM_WINDOWS && PLATFORM_64BITS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif
#include "onnxruntime_cxx_api.h"
#if PLATFORM_WINDOWS && PLATFORM_64BITS
#include "Windows/HideWindowsPlatformTypes.h"
#endif

FOnnxModelInstance::FOnnxModelInstance(UOnnxModelAsset* InModelAsset): env_(nullptr), session_(nullptr), bIsInitialized_(false)
{
    UE_LOG(LogTemp, Log, TEXT("Creating FOnnxModelInstance..."));
    
    try
    {
        // 创建ONNX环境
        env_ = MakeUnique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "ClothModel");
        UE_LOG(LogTemp, Log, TEXT("ONNX Environment created"));
        
        // 硬编码加载multiply_by_two.onnx模型
        FString ProjectDir = FPaths::ProjectDir();
        FString ModelPath = FPaths::Combine(ProjectDir, TEXT("Content"), TEXT("Model"), TEXT("sam2_hiera_tiny_decoder.onnx"));
        
        UE_LOG(LogTemp, Log, TEXT("Attempting to load model: %s"), *ModelPath);
        
        // 检查文件是否存在
        if (FPaths::FileExists(ModelPath))
        {
            
            // 创建会话选项
            Ort::SessionOptions sessionOptions;
            sessionOptions.SetIntraOpNumThreads(1);
            
            // 创建会话
            session_ = MakeUnique<Ort::Session>(*env_, *ModelPath, sessionOptions);
            UE_LOG(LogTemp, Log, TEXT("ONNX Session created successfully"));
            
            // 获取模型输入输出信息
            size_t numInputNodes = session_->GetInputCount();
            size_t numOutputNodes = session_->GetOutputCount();
            
            UE_LOG(LogTemp, Log, TEXT("Model info - Inputs: %d, Outputs: %d"), numInputNodes, numOutputNodes);
            
            if (numInputNodes > 0)
            {
                Ort::AllocatorWithDefaultOptions allocator;
                auto inputName = session_->GetInputNameAllocated(0, allocator);
                inputNodeName_ = FString(UTF8_TO_TCHAR(inputName.get()));
                UE_LOG(LogTemp, Log, TEXT("Input node name: %s"), *inputNodeName_);
            }
            
            if (numOutputNodes > 0)
            {
                Ort::AllocatorWithDefaultOptions allocator;
                auto outputName = session_->GetOutputNameAllocated(0, allocator);
                outputNodeName_ = FString(UTF8_TO_TCHAR(outputName.get()));
                UE_LOG(LogTemp, Log, TEXT("Output node name: %s"), *outputNodeName_);
            }
            
            bIsInitialized_ = true;
            UE_LOG(LogTemp, Log, TEXT("FOnnxModelInstance initialized successfully with sam2_hiera_tiny_decoder.onnx"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Model file not found: %s - only Env created"), *ModelPath);
            bIsInitialized_ = true; // 至少环境创建成功了
        }
    }
    catch (const Ort::Exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("ONNX Runtime error in constructor: %s"), UTF8_TO_TCHAR(e.what()));
        bIsInitialized_ = false;
    }
    catch (const std::exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("Standard exception in constructor: %s"), UTF8_TO_TCHAR(e.what()));
        bIsInitialized_ = false;
    }
}

FOnnxModelInstance::~FOnnxModelInstance()
{
    ;
}
bool FOnnxModelInstance::IsInitialized() const
{
    return bIsInitialized_;
}

bool FOnnxModelInstance::Run(const TArray<float>& InputData, TArray<float>& OutputData)
{
    return false;
}