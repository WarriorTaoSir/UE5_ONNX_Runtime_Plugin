// GenericOnnxTestActor.cpp

#include "GenericOnnxTestActor.h"
#include "Engine/Engine.h"

AGenericOnnxTestActor::AGenericOnnxTestActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // 创建通用ONNX组件
    OnnxComponent = CreateDefaultSubobject<UONNXComponent>(TEXT("OnnxComponent"));
    if (OnnxComponent)
    {
        // 默认配置
        OnnxComponent->ModelFilePath = ModelFilePath;
    }

    // 设置默认的输入形状（以典型的图像分类模型为例）
    InputShape = {1, 3, 224, 224}; // [Batch, Channels, Height, Width]
}

void AGenericOnnxTestActor::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("GenericOnnxTestActor BeginPlay"));

    if (bAutoRunTest && !bTestCompleted)
    {
        // 延迟1秒后运行测试，确保所有组件都已初始化
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGenericOnnxTestActor::RunGenericOnnxTest, 1.0f, false);
    }
}

void AGenericOnnxTestActor::RunGenericOnnxTest()
{
    UE_LOG(LogTemp, Log, TEXT("Starting Generic ONNX test..."));

    if (bTestCompleted)
    {
        UE_LOG(LogTemp, Warning, TEXT("Generic ONNX test already completed"));
        return;
    }

    // 步骤1: 检查ONNX组件
    if (!OnnxComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("ONNX component not found"));
        return;
    }

    // 步骤2: 设置模型路径并初始化
    if (!ModelFilePath.IsEmpty())
    {
        if (!OnnxComponent->LoadModelFromFile(ModelFilePath))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to load ONNX model from: %s"), *ModelFilePath);
            return;
        }
    }

    if (!OnnxComponent->IsInitialized())
    {
        UE_LOG(LogTemp, Error, TEXT("ONNX component not initialized"));
        return;
    }

    // 步骤3: 准备测试数据
    if (TestInputData.Num() == 0)
    {
        GenerateRandomInputData();
    }

    // 验证输入数据大小
    int32 ExpectedSize = 1;
    for (int32 Dim : InputShape)
    {
        ExpectedSize *= Dim;
    }

    if (TestInputData.Num() != ExpectedSize)
    {
        UE_LOG(LogTemp, Warning, TEXT("Input data size (%d) doesn't match expected size (%d). Regenerating..."), 
               TestInputData.Num(), ExpectedSize);
        GenerateRandomInputData();
    }

    UE_LOG(LogTemp, Log, TEXT("Running inference with input size: %d"), TestInputData.Num());

    // 步骤4: 运行推理
    TArray<float> OutputData;
    if (OnnxComponent->RunInference(TestInputData, OutputData))
    {
        UE_LOG(LogTemp, Log, TEXT("Generic ONNX inference completed successfully!"));
        UE_LOG(LogTemp, Log, TEXT("Input size: %d, Output size: %d"), TestInputData.Num(), OutputData.Num());

        // 显示输出的前几个值
        int32 NumToShow = FMath::Min(10, OutputData.Num());
        FString OutputSample = TEXT("Output sample: ");
        for (int32 i = 0; i < NumToShow; ++i)
        {
            OutputSample += FString::Printf(TEXT("%.4f "), OutputData[i]);
        }
        if (OutputData.Num() > NumToShow)
        {
            OutputSample += TEXT("...");
        }
        UE_LOG(LogTemp, Log, TEXT("%s"), *OutputSample);

        bTestCompleted = true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Generic ONNX inference failed"));
    }
}

void AGenericOnnxTestActor::GenerateRandomInputData()
{
    // 计算总的输入大小
    int32 TotalSize = 1;
    for (int32 Dim : InputShape)
    {
        TotalSize *= Dim;
    }

    if (TotalSize <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid input shape"));
        return;
    }

    TestInputData.Empty(TotalSize);
    TestInputData.AddUninitialized(TotalSize);

    // 生成随机数据（通常在0-1范围内，适合大多数预处理过的模型输入）
    for (int32 i = 0; i < TotalSize; ++i)
    {
        TestInputData[i] = FMath::FRand(); // 生成0-1之间的随机数
    }

    UE_LOG(LogTemp, Log, TEXT("Generated random input data with %d elements"), TotalSize);
}