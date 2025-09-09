// Sam2TestActor.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Texture2D.h"
#include "Sam2Component.h"
#include "Sam2TestActor.generated.h"

/**
 * ASam2TestActor
 * 用于测试SAM2图像分割功能的Actor
 * 自动加载测试图像，设置提示点，运行推理并保存结果
 */
UCLASS()
class CLOTH_API ASam2TestActor : public AActor
{
    GENERATED_BODY()

public:
    ASam2TestActor();

protected:
    virtual void BeginPlay() override;

public:
    // SAM2组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SAM2 Test")
    USam2Component* Sam2Component;

    // 测试图像资产
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Test")
    UTexture2D* TestImage;

    // 提示点位置（相对坐标 0.0-1.0）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Test")
    FVector2D PromptPoint = FVector2D(0.5f, 0.5f);

    // 提示点是否为前景点
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Test")
    bool bIsForegroundPoint = true;

    // 输出掩码文件名
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Test")
    FString OutputFileName = TEXT("sam2_mask_result.png");

    // 叠加可视化文件名
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Test")
    FString OverlayFileName = TEXT("sam2_overlay_result.png");

    // 是否在开始时自动运行测试
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Test")
    bool bAutoRunTest = true;

    // 测试函数
    UFUNCTION(BlueprintCallable, Category = "SAM2 Test")
    void RunSAM2Test();

    // 加载测试图像
    UFUNCTION(BlueprintCallable, Category = "SAM2 Test")
    bool LoadTestImage();

    // 保存掩码结果
    UFUNCTION(BlueprintCallable, Category = "SAM2 Test")
    bool SaveMaskResult(UTexture2D* MaskTexture);

    // 保存叠加结果
    UFUNCTION(BlueprintCallable, Category = "SAM2 Test")
    bool SaveOverlayResult(UTexture2D* OverlayTexture);

    // 将纹理保存为PNG文件
    UFUNCTION(BlueprintCallable, Category = "SAM2 Test")
    bool SaveTextureAsPNG(UTexture2D* Texture, const FString& FileName);

private:
    // 测试是否已经运行
    bool bTestCompleted = false;

    // 辅助函数：获取项目目录路径
    FString GetProjectDirectory() const;

    // 辅助函数：获取Content目录路径  
    FString GetContentDirectory() const;

    // 辅助函数：获取TestResult目录路径
    FString GetTestResultDirectory() const;
};