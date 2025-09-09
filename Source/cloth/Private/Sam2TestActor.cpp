// Sam2TestActor.cpp

#include "Sam2TestActor.h"
#include "ImageLoadHelper.h"
#include "Engine/Engine.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "TextureResource.h"
#include "Engine/TextureLODSettings.h"
#include "UObject/ConstructorHelpers.h"

// 定义锁定常量（兼容不同UE版本）
#ifndef LOCK_READ_ONLY
#define LOCK_READ_ONLY 1
#endif

ASam2TestActor::ASam2TestActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // 创建SAM2组件
    Sam2Component = CreateDefaultSubobject<USam2Component>(TEXT("Sam2Component"));
    if (Sam2Component)
    {
        // 设置SAM2模型路径
        Sam2Component->Sam2EncoderPath = TEXT("Content/Model/sam2_hiera_tiny_encoder.onnx");
        Sam2Component->Sam2DecoderPath = TEXT("Content/Model/sam2_hiera_tiny_decoder.onnx");
    }
}

void ASam2TestActor::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("Sam2TestActor BeginPlay"));

    if (bAutoRunTest && !bTestCompleted)
    {
        // 延迟1秒后运行测试，确保所有组件都已初始化
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASam2TestActor::RunSAM2Test, 1.0f, false);
    }
}

void ASam2TestActor::RunSAM2Test()
{
    UE_LOG(LogTemp, Log, TEXT("Starting SAM2 test..."));

    if (bTestCompleted)
    {
        UE_LOG(LogTemp, Warning, TEXT("SAM2 test already completed"));
        return;
    }

    // 步骤1: 检查SAM2组件
    if (!Sam2Component)
    {
        UE_LOG(LogTemp, Error, TEXT("SAM2 component not found"));
        return;
    }

    if (!Sam2Component->IsInitialized())
    {
        UE_LOG(LogTemp, Error, TEXT("SAM2 component not initialized"));
        return;
    }

    // 步骤2: 加载测试图像
    if (!LoadTestImage())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load test image"));
        return;
    }

    if (!TestImage)
    {
        UE_LOG(LogTemp, Error, TEXT("Test image is null"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Loaded test image: %dx%d"), TestImage->GetSizeX(), TestImage->GetSizeY());

    // 步骤3: 准备SAM2输入
    FSam2Input Sam2Input;
    
    // 从纹理设置图像数据
    if (!Sam2Component->SetImageFromTexture(TestImage, Sam2Input))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to set image data from texture"));
        return;
    }

    // 添加提示点
    Sam2Component->ClearPromptPoints(Sam2Input);
    Sam2Component->AddPromptPoint(Sam2Input, PromptPoint, bIsForegroundPoint);

    UE_LOG(LogTemp, Log, TEXT("SAM2 input prepared: image %dx%d, prompt point (%.3f, %.3f), foreground=%s"),
           Sam2Input.ImageWidth, Sam2Input.ImageHeight, PromptPoint.X, PromptPoint.Y,
           bIsForegroundPoint ? TEXT("true") : TEXT("false"));

    // 步骤4: 运行推理
    FSam2Output Sam2Output;
    if (!Sam2Component->RunSam2Segmentation(Sam2Input, Sam2Output))
    {
        UE_LOG(LogTemp, Error, TEXT("SAM2 segmentation failed"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("SAM2 inference completed: %d masks, IoU=%.3f"),
           Sam2Output.NumMasks, Sam2Output.IouScores.Num() > 0 ? Sam2Output.IouScores[0] : 0.0f);

    // 步骤5: 创建掩码纹理
    UTexture2D* MaskTexture = Sam2Component->CreateMaskTexture(Sam2Output, 0);
    if (!MaskTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create mask texture"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Created mask texture: %dx%d"), MaskTexture->GetSizeX(), MaskTexture->GetSizeY());

    // 步骤6: 保存掩码结果
    bool bMaskSaved = SaveMaskResult(MaskTexture);

    // 步骤7: 尝试创建叠加可视化图像（如果失败不会影响主要功能）
    UTexture2D* OverlayTexture = nullptr;
    bool bOverlaySaved = false;
    
    UE_LOG(LogTemp, Log, TEXT("Attempting to create overlay texture..."));
    try 
    {
        OverlayTexture = Sam2Component->CreateOverlayTexture(TestImage, Sam2Input, Sam2Output, 0);
        if (OverlayTexture)
        {
            UE_LOG(LogTemp, Log, TEXT("Created overlay texture: %dx%d"), OverlayTexture->GetSizeX(), OverlayTexture->GetSizeY());
            bOverlaySaved = SaveOverlayResult(OverlayTexture);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to create overlay texture, but continuing..."));
        }
    }
    catch (...)
    {
        UE_LOG(LogTemp, Error, TEXT("Exception occurred while creating overlay texture"));
    }

    // 步骤8: 报告结果
    if (bMaskSaved)
    {
        UE_LOG(LogTemp, Log, TEXT("SAM2 test completed successfully!"));
        UE_LOG(LogTemp, Log, TEXT("Mask result saved to: %s"), *OutputFileName);
        if (bOverlaySaved)
        {
            UE_LOG(LogTemp, Log, TEXT("Overlay result saved to: %s"), *OverlayFileName);
        }
        bTestCompleted = true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save mask result"));
    }
}

bool ASam2TestActor::LoadTestImage()
{
    UE_LOG(LogTemp, Log, TEXT("Loading Test01 image..."));

    // 使用ImageLoadHelper加载图像
    TestImage = UImageLoadHelper::FindAndLoadTest01Image();

    if (TestImage)
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully loaded Test01 image: %dx%d"), 
               TestImage->GetSizeX(), TestImage->GetSizeY());
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Test01 image using ImageLoadHelper"));
        return false;
    }
}

bool ASam2TestActor::SaveMaskResult(UTexture2D* MaskTexture)
{
    if (!MaskTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("Mask texture is null"));
        return false;
    }

    FString ResultPath = FPaths::Combine(GetTestResultDirectory(), OutputFileName);
    return SaveTextureAsPNG(MaskTexture, ResultPath);
}

bool ASam2TestActor::SaveOverlayResult(UTexture2D* OverlayTexture)
{
    if (!OverlayTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("Overlay texture is null"));
        return false;
    }

    FString ResultPath = FPaths::Combine(GetTestResultDirectory(), OverlayFileName);
    return SaveTextureAsPNG(OverlayTexture, ResultPath);
}

bool ASam2TestActor::SaveTextureAsPNG(UTexture2D* Texture, const FString& FilePath)
{
    return UImageLoadHelper::SaveTextureAsPNG(Texture, FilePath);
}

FString ASam2TestActor::GetProjectDirectory() const
{
    return FPaths::ProjectDir();
}

FString ASam2TestActor::GetContentDirectory() const
{
    return FPaths::Combine(GetProjectDirectory(), TEXT("Content"));
}

FString ASam2TestActor::GetTestResultDirectory() const
{
    return FPaths::Combine(GetProjectDirectory(), TEXT("Content"), TEXT("TestResult"));
}