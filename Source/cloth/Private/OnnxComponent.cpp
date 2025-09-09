// OnnxComponent.cpp - 重构后的纯通用ONNX组件实现

#include "OnnxComponent.h"
#include "OnnxModelInstance.h"
#include "HAL/PlatformFilemanager.h"

UONNXComponent::UONNXComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    ModelAsset = nullptr;
    ModelFilePath = TEXT("");
    bIsInitialized = false;
}

UONNXComponent::~UONNXComponent()
{
    // TUniquePtr自动清理
}

void UONNXComponent::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("ONNX Component BeginPlay - attempting to initialize model..."));

    // 尝试自动初始化
    if (!bIsInitialized)
    {
        if (Initialize())
        {
            UE_LOG(LogTemp, Log, TEXT("ONNX Component initialized successfully"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("ONNX Component initialization failed - no model specified or file not found"));
        }
    }
}

void UONNXComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Reset();
    Super::EndPlay(EndPlayReason);
}

bool UONNXComponent::Initialize()
{
    UE_LOG(LogTemp, Log, TEXT("Initializing ONNX Component..."));

    if (bIsInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("ONNX Component already initialized"));
        return true;
    }

    return InitializeModel();
}

bool UONNXComponent::InitializeModel()
{
    try
    {
        if (ModelAsset)
        {
            // 使用UOnnxModelAsset
            ModelInstance = MakeUnique<FOnnxModelInstance>(ModelAsset);
            UE_LOG(LogTemp, Log, TEXT("Loading ONNX model from asset"));
        }
        else if (!ModelFilePath.IsEmpty())
        {
            // 使用文件路径
            if (!FPaths::FileExists(ModelFilePath))
            {
                UE_LOG(LogTemp, Error, TEXT("ONNX model file not found: %s"), *ModelFilePath);
                return false;
            }

            // 创建临时的空资产来初始化实例
            ModelInstance = MakeUnique<FOnnxModelInstance>(nullptr);
            UE_LOG(LogTemp, Log, TEXT("Loading ONNX model from file path: %s"), *ModelFilePath);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No model asset or file path specified"));
            return false;
        }

        if (ModelInstance && ModelInstance->IsInitialized())
        {
            bIsInitialized = true;
            UE_LOG(LogTemp, Log, TEXT("ONNX Model Instance created successfully"));
            return true;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to initialize ONNX Model Instance"));
            ModelInstance.Reset();
            return false;
        }
    }
    catch (const std::exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("Exception creating ONNX Model Instance: %s"), UTF8_TO_TCHAR(e.what()));
        ModelInstance.Reset();
        return false;
    }
}

bool UONNXComponent::RunInference(const TArray<float>& InputData, TArray<float>& OutputData)
{
    if (!IsInitialized())
    {
        UE_LOG(LogTemp, Error, TEXT("ONNX Component not initialized"));
        return false;
    }

    if (!ModelInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("Model instance is null"));
        return false;
    }

    return ModelInstance->Run(InputData, OutputData);
}

bool UONNXComponent::IsInitialized() const
{
    return bIsInitialized && ModelInstance && ModelInstance->IsInitialized();
}

bool UONNXComponent::LoadModelFromFile(const FString& FilePath)
{
    if (FilePath.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("File path is empty"));
        return false;
    }

    if (!FPaths::FileExists(FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Model file not found: %s"), *FilePath);
        return false;
    }

    // 重置现有状态
    Reset();

    // 设置新的文件路径
    ModelFilePath = FilePath;
    ModelAsset = nullptr; // 清除资产引用

    // 重新初始化
    return Initialize();
}

TArray<FOnnxTensorInfo> UONNXComponent::GetModelInputInfo() const
{
    TArray<FOnnxTensorInfo> InputInfo;
    
    if (IsInitialized())
    {
        // TODO: 从ModelInstance获取实际的输入信息
        // 这需要扩展FOnnxModelInstance来暴露模型元数据
        FOnnxTensorInfo DefaultInput;
        DefaultInput.Name = TEXT("input");
        DefaultInput.Shape = {1, 3, 224, 224}; // 默认形状
        DefaultInput.DataType = TEXT("float32");
        InputInfo.Add(DefaultInput);
    }

    return InputInfo;
}

TArray<FOnnxTensorInfo> UONNXComponent::GetModelOutputInfo() const
{
    TArray<FOnnxTensorInfo> OutputInfo;
    
    if (IsInitialized())
    {
        // TODO: 从ModelInstance获取实际的输出信息
        FOnnxTensorInfo DefaultOutput;
        DefaultOutput.Name = TEXT("output");
        DefaultOutput.Shape = {1, 1000}; // 默认形状
        DefaultOutput.DataType = TEXT("float32");
        OutputInfo.Add(DefaultOutput);
    }

    return OutputInfo;
}

void UONNXComponent::Reset()
{
    ModelInstance.Reset();
    bIsInitialized = false;
    UE_LOG(LogTemp, Log, TEXT("ONNX Component reset"));
}