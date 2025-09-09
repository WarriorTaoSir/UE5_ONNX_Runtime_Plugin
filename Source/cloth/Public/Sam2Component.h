// Sam2Component.h - SAM2专用组件

#pragma once

#include "CoreMinimal.h"
#include "OnnxComponent.h"
#include "Engine/Texture2D.h"
#include "Sam2ModelInstance.h"
#include "Sam2Component.generated.h"

// Forward declarations
class FSam2ModelInstance;
struct FSam2Input;
struct FSam2Output;

/**
 * USam2Component
 * SAM2 (Segment Anything Model 2) 专用组件
 * 继承自通用ONNX组件，专门处理图像分割任务
 */
UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class CLOTH_API USam2Component : public UONNXComponent
{
    GENERATED_BODY()

public:
    USam2Component();
    virtual ~USam2Component();

protected:
    virtual void BeginPlay() override;

public:
    // === SAM2特定配置 ===

    // SAM2 Encoder模型文件路径
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Settings")
    FString Sam2EncoderPath = TEXT("Content/Model/sam2_hiera_tiny_encoder.onnx");

    // SAM2 Decoder模型文件路径
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Settings")
    FString Sam2DecoderPath = TEXT("Content/Model/sam2_hiera_tiny_decoder.onnx");

    // === SAM2专用接口 ===

    // SAM2图像分割推理
    UFUNCTION(BlueprintCallable, Category = "SAM2 Segmentation")
    bool RunSam2Segmentation(const FSam2Input& Input, FSam2Output& Output);

    // 设置图像数据用于SAM2推理（从UTexture2D转换）
    UFUNCTION(BlueprintCallable, Category = "SAM2 Segmentation")
    bool SetImageFromTexture(UTexture2D* Texture, FSam2Input& Sam2Input);

    // 添加提示点到SAM2输入
    UFUNCTION(BlueprintCallable, Category = "SAM2 Segmentation")
    void AddPromptPoint(UPARAM(ref) FSam2Input& Sam2Input, FVector2D Point, bool bIsForeground = true);

    // 清除所有提示点
    UFUNCTION(BlueprintCallable, Category = "SAM2 Segmentation")
    void ClearPromptPoints(UPARAM(ref) FSam2Input& Sam2Input);

    // 从SAM2输出创建掩码纹理
    UFUNCTION(BlueprintCallable, Category = "SAM2 Segmentation")
    UTexture2D* CreateMaskTexture(const FSam2Output& Output, int32 MaskIndex = 0);

    // 创建掩码叠加到原图的可视化结果
    UFUNCTION(BlueprintCallable, Category = "SAM2 Segmentation")
    UTexture2D* CreateOverlayTexture(UTexture2D* OriginalImage, const FSam2Input& Input, const FSam2Output& Output, int32 MaskIndex = 0);

    // === 重写基类方法 ===

    virtual bool RunInference(const TArray<float>& InputData, TArray<float>& OutputData) override;
    virtual bool IsInitialized() const override;

protected:
    // SAM2特定推理实例
    TUniquePtr<FSam2ModelInstance> Sam2Instance;

    // 重写基类的初始化方法
    virtual bool InitializeModel() override;

private:
    // 初始化SAM2模型
    bool InitializeSam2Model();

    // 将UTexture2D转换为浮点数组
    bool ConvertTextureToFloatArray(UTexture2D* Texture, TArray<float>& ImageData, int32& Width, int32& Height);
};