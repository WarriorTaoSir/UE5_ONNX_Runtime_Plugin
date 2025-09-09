// OnnxComponent.h - 重构后的纯通用ONNX组件（替换原有版本）

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OnnxModelAsset.h"
#include "OnnxModelInstance.h"
#include "OnnxComponent.generated.h"

// Forward declarations
class UOnnxModelAsset;
class FOnnxModelInstance;

/**
 * 通用ONNX张量数据结构
 */
USTRUCT(BlueprintType)
struct CLOTH_API FOnnxTensorInfo
{
    GENERATED_BODY()

    // 张量名称
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ONNX Tensor")
    FString Name;

    // 张量形状 [N, C, H, W] 等
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ONNX Tensor")
    TArray<int32> Shape;

    // 张量数据类型（暂时只支持float）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ONNX Tensor")
    FString DataType = TEXT("float32");

    FOnnxTensorInfo()
    {
        Name = TEXT("");
        Shape.Empty();
        DataType = TEXT("float32");
    }
};

/**
 * UONNXComponent
 * 重构后的纯通用ONNX模型推理组件
 * 移除了所有SAM2特定的代码，专注于提供通用ONNX推理功能
 */
UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class CLOTH_API UONNXComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UONNXComponent();
    virtual ~UONNXComponent();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    // === 基本配置 ===
    
    // ONNX模型资产
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ONNX Model")
    UOnnxModelAsset* ModelAsset;

    // 模型文件路径（当没有ModelAsset时使用）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ONNX Model")
    FString ModelFilePath;

    // === 核心接口 ===

    // 初始化ONNX模型
    UFUNCTION(BlueprintCallable, Category = "ONNX Inference")
    virtual bool Initialize();

    // 运行推理（单输入单输出）
    UFUNCTION(BlueprintCallable, Category = "ONNX Inference")
    virtual bool RunInference(const TArray<float>& InputData, TArray<float>& OutputData);

    // 检查是否已初始化
    UFUNCTION(BlueprintCallable, Category = "ONNX Inference")
    virtual bool IsInitialized() const;

    // 从文件路径加载模型
    UFUNCTION(BlueprintCallable, Category = "ONNX Inference")
    bool LoadModelFromFile(const FString& FilePath);

    // 获取模型信息
    UFUNCTION(BlueprintCallable, Category = "ONNX Model Info")
    TArray<FOnnxTensorInfo> GetModelInputInfo() const;

    UFUNCTION(BlueprintCallable, Category = "ONNX Model Info")
    TArray<FOnnxTensorInfo> GetModelOutputInfo() const;

    // 重置模型和清理资源
    UFUNCTION(BlueprintCallable, Category = "ONNX Inference")
    void Reset();

protected:
    // ONNX模型实例
    TUniquePtr<FOnnxModelInstance> ModelInstance;

    // 初始化标志
    bool bIsInitialized = false;

    // 初始化模型实例（可被子类重写）
    virtual bool InitializeModel();
};