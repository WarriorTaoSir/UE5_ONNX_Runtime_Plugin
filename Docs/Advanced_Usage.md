# UE5 ONNX Plugin 使用指南

本指南详细说明如何使用重构后的Cloth插件来导入和推理各种ONNX模型。

## 目录

1. [插件架构概述](#插件架构概述)
2. [通用ONNX模型推理](#通用onnx模型推理)
3. [SAM2专用功能](#sam2专用功能)  
4. [扩展新模型类型](#扩展新模型类型)
5. [最佳实践](#最佳实践)

---

## 插件架构概述

### 核心组件结构

插件采用了分层的面向对象设计，支持多种ONNX模型类型：

```
UActorComponent
├── UONNXComponent (通用ONNX推理基础类)
└── USam2Component (SAM2专用功能继承类)
```

### 关键类说明

- **UONNXComponent**: 纯通用ONNX组件，提供基础的模型加载和推理功能
- **USam2Component**: 专门为SAM2图像分割任务设计的组件
- **FOnnxModelInstance**: 通用ONNX Runtime会话封装
- **FSam2ModelInstance**: SAM2双模型(encoder-decoder)推理引擎
- **AGenericOnnxTestActor**: 测试通用ONNX模型的示例Actor

---

## 通用ONNX模型推理

### 1. 基本使用方法

#### 在C++中使用

```cpp
// 在Actor头文件中声明组件
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
UONNXComponent* OnnxComponent;

// 在构造函数中创建组件
OnnxComponent = CreateDefaultSubobject<UONNXComponent>(TEXT("OnnxComponent"));

// 加载模型并运行推理
void AMyActor::RunInference()
{
    // 方法1: 通过文件路径加载
    if (OnnxComponent->LoadModelFromFile(TEXT("Content/Model/my_model.onnx")))
    {
        TArray<float> InputData = {/* 你的输入数据 */};
        TArray<float> OutputData;
        
        if (OnnxComponent->RunInference(InputData, OutputData))
        {
            // 处理输出结果
            UE_LOG(LogTemp, Log, TEXT("推理成功，输出大小: %d"), OutputData.Num());
        }
    }
}
```

#### 在蓝图中使用

1. 在Actor蓝图中添加**ONNX Component**
2. 设置**Model File Path**为你的模型路径（例如：`Content/Model/my_model.onnx`）
3. 调用**Initialize**节点初始化模型
4. 使用**Run Inference**节点进行推理

### 2. 模型文件管理

#### 推荐的目录结构
```
Content/
└── Model/
    ├── classification/
    │   ├── resnet50.onnx
    │   └── mobilenet.onnx
    ├── detection/
    │   └── yolo.onnx
    └── sam2/
        ├── sam2_hiera_tiny_encoder.onnx
        └── sam2_hiera_tiny_decoder.onnx
```

#### 支持的路径格式
- 相对路径：`Content/Model/resnet50.onnx`
- 绝对路径：`E:/MyProject/Content/Model/resnet50.onnx`
- 项目相对路径：`$(ProjectDir)Content/Model/resnet50.onnx`

### 3. 使用GenericOnnxTestActor进行测试

```cpp
// 创建测试Actor实例
AGenericOnnxTestActor* TestActor = GetWorld()->SpawnActor<AGenericOnnxTestActor>();

// 配置测试参数
TestActor->ModelFilePath = TEXT("Content/Model/my_model.onnx");
TestActor->InputShape = {1, 3, 224, 224}; // 批次=1, 通道=3, 高=224, 宽=224
TestActor->bAutoRunTest = true;

// 手动运行测试
TestActor->RunGenericOnnxTest();
```

---

## SAM2专用功能

### 1. SAM2组件使用

```cpp
// 创建SAM2专用组件
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SAM2")
USam2Component* Sam2Component;

Sam2Component = CreateDefaultSubobject<USam2Component>(TEXT("Sam2Component"));

// 配置SAM2模型路径
Sam2Component->EncoderModelPath = TEXT("Content/Model/sam2/sam2_hiera_tiny_encoder.onnx");
Sam2Component->DecoderModelPath = TEXT("Content/Model/sam2/sam2_hiera_tiny_decoder.onnx");

// 运行SAM2推理
void AMyActor::RunSam2Segmentation()
{
    FSam2Input Input;
    FSam2Output Output;
    
    // 设置输入图像
    if (Sam2Component->SetImageFromTexture(InputTexture, Input))
    {
        // 添加提示点 (相对坐标 0.0-1.0)
        Sam2Component->AddPromptPoint(Input, FVector2D(0.5f, 0.5f), true); // 前景点
        
        // 运行推理
        if (Sam2Component->RunSam2Segmentation(Input, Output))
        {
            // 创建掩码纹理
            UTexture2D* MaskTexture = Sam2Component->CreateMaskTexture(Output, 0);
            
            // 创建叠加可视化
            UTexture2D* OverlayTexture = Sam2Component->CreateOverlayTexture(
                InputTexture, Input, Output, 0
            );
        }
    }
}
```

### 2. SAM2蓝图工作流

1. 添加**Sam2 Component**到Actor
2. 设置**Encoder Model Path**和**Decoder Model Path**
3. 使用**Set Image From Texture**加载图像
4. 使用**Add Prompt Point**添加点击点
5. 调用**Run Sam2 Segmentation**执行分割
6. 使用**Create Mask Texture**或**Create Overlay Texture**获取结果

---

## 扩展新模型类型

### 1. 创建专用组件类

当你需要支持一个新的ONNX模型类型（如目标检测、语音识别等）时，可以继承UONNXComponent：

```cpp
// YoloComponent.h
#pragma once

#include "CoreMinimal.h"
#include "OnnxComponent.h"
#include "YoloComponent.generated.h"

// YOLO专用输入输出结构
USTRUCT(BlueprintType)
struct CLOTH_API FYoloInput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YOLO")
    TArray<float> ImageData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YOLO") 
    int32 ImageWidth = 640;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YOLO")
    int32 ImageHeight = 640;
};

USTRUCT(BlueprintType)
struct CLOTH_API FYoloOutput
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YOLO")
    TArray<FVector4> BoundingBoxes; // (x, y, width, height)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YOLO")
    TArray<float> Confidences;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YOLO")
    TArray<int32> ClassIds;
};

UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class CLOTH_API UYoloComponent : public UONNXComponent
{
    GENERATED_BODY()

public:
    UYoloComponent();

    // YOLO专用推理接口
    UFUNCTION(BlueprintCallable, Category = "YOLO Detection")
    bool RunYoloDetection(const FYoloInput& Input, FYoloOutput& Output);

    // 图像预处理
    UFUNCTION(BlueprintCallable, Category = "YOLO Detection")
    bool PrepareImageForYolo(UTexture2D* Texture, FYoloInput& YoloInput);

    // 后处理：非极大值抑制等
    UFUNCTION(BlueprintCallable, Category = "YOLO Detection")
    void PostprocessYoloOutput(FYoloOutput& Output, float ConfThreshold = 0.5f, float NmsThreshold = 0.4f);

protected:
    // YOLO专用配置
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YOLO Settings")
    FString ClassNamesFile = TEXT("Content/Model/coco_classes.txt");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YOLO Settings")
    int32 InputSize = 640;

private:
    // 加载类别名称
    void LoadClassNames();

    TArray<FString> ClassNames;
};
```

### 2. 实现专用功能

```cpp
// YoloComponent.cpp
#include "YoloComponent.h"

UYoloComponent::UYoloComponent()
{
    // 设置默认模型路径
    ModelFilePath = TEXT("Content/Model/yolov8n.onnx");
    LoadClassNames();
}

bool UYoloComponent::RunYoloDetection(const FYoloInput& Input, FYoloOutput& Output)
{
    if (!IsInitialized())
    {
        UE_LOG(LogTemp, Error, TEXT("YOLO组件未初始化"));
        return false;
    }

    // 调用基类的通用推理
    TArray<float> RawOutput;
    if (!RunInference(Input.ImageData, RawOutput))
    {
        return false;
    }

    // YOLO专用后处理
    ParseYoloOutput(RawOutput, Output);
    PostprocessYoloOutput(Output);

    return true;
}

void UYoloComponent::ParseYoloOutput(const TArray<float>& RawOutput, FYoloOutput& Output)
{
    // 解析YOLO输出格式 [x, y, w, h, conf, class_probs...]
    // 具体实现取决于你的YOLO模型输出格式
}
```

### 3. 创建测试Actor

```cpp
// YoloTestActor.h
UCLASS()
class CLOTH_API AYoloTestActor : public AActor
{
    GENERATED_BODY()

public:
    AYoloTestActor();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "YOLO Test")
    UYoloComponent* YoloComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YOLO Test")
    UTexture2D* TestImage;

    UFUNCTION(BlueprintCallable, Category = "YOLO Test")
    void RunYoloTest();

private:
    void DrawBoundingBoxes(const FYoloOutput& Output);
};
```

---

## 最佳实践

### 1. 性能优化建议

- **预分配内存**: 为频繁使用的TArray预分配空间
- **复用组件**: 避免频繁创建和销毁ONNX组件
- **异步推理**: 对于耗时的推理任务考虑使用异步处理

```cpp
// 预分配输入数组大小
TArray<float> InputData;
InputData.Reserve(1 * 3 * 224 * 224); // 避免动态扩容
```

### 2. 内存管理

- 所有ONNX Runtime会话都通过TUniquePtr自动管理
- 大型图像数据使用完毕后及时清理
- 避免在Tick函数中执行重型推理任务

### 3. 错误处理

```cpp
bool RunSafeInference()
{
    if (!OnnxComponent || !OnnxComponent->IsInitialized())
    {
        UE_LOG(LogTemp, Error, TEXT("ONNX组件未准备就绪"));
        return false;
    }

    try
    {
        TArray<float> Input, Output;
        return OnnxComponent->RunInference(Input, Output);
    }
    catch (const std::exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("推理异常: %s"), UTF8_TO_TCHAR(e.what()));
        return false;
    }
}
```

### 4. 调试技巧

- 启用详细日志：设置LogTemp等级为Verbose
- 检查输入输出张量形状是否匹配
- 使用GenericOnnxTestActor验证新模型
- 保存中间结果到文件进行分析

### 5. 模型优化

- 使用ONNX Runtime的优化选项
- 考虑量化模型以减少内存占用
- 针对目标硬件选择合适的执行提供商（CPU/GPU）

---

## 常见问题解答

### Q: 如何检查模型是否成功加载？
A: 使用`IsInitialized()`方法检查，并查看控制台日志输出。

### Q: 支持哪些ONNX版本？
A: 插件使用ONNX Runtime 1.20，支持ONNX格式版本1.0-1.15。

### Q: 如何处理不同的输入输出数据类型？
A: 当前主要支持float32，其他类型需要在FOnnxModelInstance中扩展支持。

### Q: 能否同时运行多个不同的ONNX模型？
A: 可以，每个Actor可以包含多个不同类型的ONNX组件。

### Q: 如何获取模型的输入输出信息？
A: 使用`GetModelInputInfo()`和`GetModelOutputInfo()`方法（需要扩展实现）。

---

## 更新日志

### v2.0.0 (重构版本)
- 将SAM2特定代码从通用组件中分离
- 创建UONNXComponent作为通用基础类
- 添加USam2Component专用类
- 提供AGenericOnnxTestActor测试工具
- 改进架构的可扩展性

### v1.0.0 (初始版本)
- 基础ONNX Runtime集成
- SAM2图像分割功能
- 基本的蓝图接口

---

这个指南为后续开发者提供了完整的插件使用和扩展方法。如有问题，请参考源码注释或创建Issue。