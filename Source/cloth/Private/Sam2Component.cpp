// Sam2Component.cpp

#include "Sam2Component.h"
#include "Sam2ModelInstance.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "HAL/PlatformFilemanager.h"

// 定义锁定常量（兼容不同UE版本）
#ifndef LOCK_READ_ONLY
#define LOCK_READ_ONLY 1
#endif
#ifndef LOCK_WRITE_ONLY
#define LOCK_WRITE_ONLY 2
#endif

USam2Component::USam2Component()
{
    // SAM2专用设置
}

USam2Component::~USam2Component()
{
    // TUniquePtr自动清理
}

void USam2Component::BeginPlay()
{
    // 调用UActorComponent::BeginPlay()，跳过UONNXComponent的BeginPlay
    UActorComponent::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("SAM2 Component BeginPlay - attempting to initialize SAM2 models..."));

    // 尝试自动初始化SAM2
    if (!bIsInitialized)
    {
        if (Initialize())
        {
            UE_LOG(LogTemp, Log, TEXT("SAM2 Component initialized successfully"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("SAM2 Component initialization failed - check model paths"));
        }
    }
}


bool USam2Component::InitializeModel()
{
    return InitializeSam2Model();
}

bool USam2Component::InitializeSam2Model()
{
    try
    {
        // 构造完整的模型路径
        FString ProjectDir = FPaths::ProjectDir();
        FString FullEncoderPath = FPaths::Combine(ProjectDir, Sam2EncoderPath);
        FString FullDecoderPath = FPaths::Combine(ProjectDir, Sam2DecoderPath);

        UE_LOG(LogTemp, Log, TEXT("Initializing SAM2 with Encoder: %s, Decoder: %s"), 
               *FullEncoderPath, *FullDecoderPath);

        Sam2Instance = MakeUnique<FSam2ModelInstance>(FullEncoderPath, FullDecoderPath);

        if (Sam2Instance && Sam2Instance->IsInitialized())
        {
            bIsInitialized = true;
            UE_LOG(LogTemp, Log, TEXT("SAM2 Component initialized successfully"));
            return true;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to initialize SAM2 Model Instance"));
            Sam2Instance.Reset();
            return false;
        }
    }
    catch (const std::exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("Exception creating SAM2 Model Instance: %s"), UTF8_TO_TCHAR(e.what()));
        Sam2Instance.Reset();
        return false;
    }
}

bool USam2Component::RunInference(const TArray<float>& InputData, TArray<float>& OutputData)
{
    UE_LOG(LogTemp, Warning, TEXT("RunInference called on SAM2 component - use RunSam2Segmentation instead"));
    return false;
}

bool USam2Component::IsInitialized() const
{
    return bIsInitialized && Sam2Instance && Sam2Instance->IsInitialized();
}

bool USam2Component::RunSam2Segmentation(const FSam2Input& Input, FSam2Output& Output)
{
    if (!Sam2Instance || !Sam2Instance->IsInitialized())
    {
        UE_LOG(LogTemp, Error, TEXT("SAM2 instance not initialized"));
        return false;
    }

    return Sam2Instance->RunInference(Input, Output);
}

bool USam2Component::SetImageFromTexture(UTexture2D* Texture, FSam2Input& Sam2Input)
{
    if (!Texture)
    {
        UE_LOG(LogTemp, Error, TEXT("Texture is null"));
        return false;
    }

    int32 Width, Height;
    bool bSuccess = ConvertTextureToFloatArray(Texture, Sam2Input.ImageData, Width, Height);
    if (bSuccess)
    {
        Sam2Input.ImageWidth = Width;
        Sam2Input.ImageHeight = Height;
    }
    return bSuccess;
}

void USam2Component::AddPromptPoint(FSam2Input& Sam2Input, FVector2D Point, bool bIsForeground)
{
    Sam2Input.PromptPoints.Add(Point);
    Sam2Input.PromptLabels.Add(bIsForeground ? 1 : 0);

    UE_LOG(LogTemp, Log, TEXT("Added prompt point (%f, %f) with label %d"), 
           Point.X, Point.Y, bIsForeground ? 1 : 0);
}

void USam2Component::ClearPromptPoints(FSam2Input& Sam2Input)
{
    Sam2Input.PromptPoints.Empty();
    Sam2Input.PromptLabels.Empty();

    UE_LOG(LogTemp, Log, TEXT("Cleared all prompt points"));
}

UTexture2D* USam2Component::CreateMaskTexture(const FSam2Output& Output, int32 MaskIndex)
{
    if (MaskIndex >= Output.NumMasks || MaskIndex < 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid mask index %d (total masks: %d)"), MaskIndex, Output.NumMasks);
        return nullptr;
    }

    if (Output.OriginalWidth <= 0 || Output.OriginalHeight <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid original dimensions %dx%d"), Output.OriginalWidth, Output.OriginalHeight);
        return nullptr;
    }

    // 获取1024×1024的原始掩码数据
    TArray<float> RawMaskData = Output.GetMask(MaskIndex);
    if (RawMaskData.Num() != 1024 * 1024)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid mask data size: %d, expected: %d"), RawMaskData.Num(), 1024 * 1024);
        return nullptr;
    }

    // 执行后处理：从1024×1024转换回原始尺寸
    TArray<uint8> FinalMaskData;
    if (Sam2Instance)
    {
        if (!Sam2Instance->PostprocessMask(RawMaskData, Output.OriginalWidth, Output.OriginalHeight,
                                          Output.Scale, Output.XOffset, Output.YOffset, FinalMaskData))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to postprocess mask"));
            return nullptr;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SAM2 instance not available for postprocessing"));
        return nullptr;
    }

    // 创建原始尺寸的纹理
    UTexture2D* MaskTexture = UTexture2D::CreateTransient(Output.OriginalWidth, Output.OriginalHeight, PF_G8);
    if (!MaskTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create mask texture"));
        return nullptr;
    }

    // 锁定纹理数据进行写入
    FTexture2DMipMap& Mip = MaskTexture->GetPlatformData()->Mips[0];
    void* TextureData = Mip.BulkData.Lock(LOCK_WRITE_ONLY);
    uint8* Pixels = static_cast<uint8*>(TextureData);

    // 复制后处理的掩码数据
    FMemory::Memcpy(Pixels, FinalMaskData.GetData(), FinalMaskData.Num());

    // 解锁纹理数据
    Mip.BulkData.Unlock();
    MaskTexture->UpdateResource();

    UE_LOG(LogTemp, Log, TEXT("Created final mask texture %dx%d from mask index %d (postprocessed from 1024x1024)"), 
           Output.OriginalWidth, Output.OriginalHeight, MaskIndex);

    return MaskTexture;
}

UTexture2D* USam2Component::CreateOverlayTexture(UTexture2D* OriginalImage, const FSam2Input& Input, const FSam2Output& Output, int32 MaskIndex)
{
    if (!OriginalImage)
    {
        UE_LOG(LogTemp, Error, TEXT("Original image is null"));
        return nullptr;
    }

    if (MaskIndex >= Output.NumMasks || MaskIndex < 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid mask index %d (total masks: %d)"), MaskIndex, Output.NumMasks);
        return nullptr;
    }

    int32 ImageWidth = OriginalImage->GetSizeX();
    int32 ImageHeight = OriginalImage->GetSizeY();

    // 创建叠加纹理
    UTexture2D* OverlayTexture = UTexture2D::CreateTransient(ImageWidth, ImageHeight, PF_B8G8R8A8);
    if (!OverlayTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create overlay texture"));
        return nullptr;
    }

    // 检查原始图像是否有效
    if (!OriginalImage->GetPlatformData() || OriginalImage->GetPlatformData()->Mips.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Original image has no valid platform data"));
        return nullptr;
    }

    // 读取原始图像数据
    FTexture2DMipMap& OriginalMip = OriginalImage->GetPlatformData()->Mips[0];
    void* OriginalData = OriginalMip.BulkData.Lock(LOCK_READ_ONLY);
    if (!OriginalData)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to lock original image data"));
        return nullptr;
    }
    uint8* OriginalPixels = static_cast<uint8*>(OriginalData);

    // 获取掩码数据并进行后处理
    TArray<float> RawMaskData = Output.GetMask(MaskIndex);
    TArray<uint8> ProcessedMask;
    
    if (Sam2Instance)
    {
        if (!Sam2Instance->PostprocessMask(RawMaskData, ImageWidth, ImageHeight,
                                          Output.Scale, Output.XOffset, Output.YOffset, ProcessedMask))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to postprocess mask for overlay"));
            OriginalMip.BulkData.Unlock();
            return nullptr;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SAM2 instance not available for postprocessing"));
        OriginalMip.BulkData.Unlock();
        return nullptr;
    }

    // 检查叠加纹理是否有效
    if (!OverlayTexture->GetPlatformData() || OverlayTexture->GetPlatformData()->Mips.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Overlay texture has no valid platform data"));
        OriginalMip.BulkData.Unlock();
        return nullptr;
    }

    // 创建叠加图像
    FTexture2DMipMap& OverlayMip = OverlayTexture->GetPlatformData()->Mips[0];
    void* OverlayData = OverlayMip.BulkData.Lock(LOCK_WRITE_ONLY);
    if (!OverlayData)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to lock overlay texture data"));
        OriginalMip.BulkData.Unlock();
        return nullptr;
    }
    uint8* OverlayPixels = static_cast<uint8*>(OverlayData);

    // 检查掩码数据大小是否匹配
    if (ProcessedMask.Num() != ImageWidth * ImageHeight)
    {
        UE_LOG(LogTemp, Error, TEXT("Mask size mismatch: expected %d, got %d"), ImageWidth * ImageHeight, ProcessedMask.Num());
        OriginalMip.BulkData.Unlock();
        OverlayMip.BulkData.Unlock();
        return nullptr;
    }

    // 叠加掩码和原图
    for (int32 i = 0; i < ImageWidth * ImageHeight; ++i)
    {
        int32 PixelIndex = i * 4; // BGRA格式

        // 复制原始像素
        uint8 OriginalB = OriginalPixels[PixelIndex + 0];
        uint8 OriginalG = OriginalPixels[PixelIndex + 1];
        uint8 OriginalR = OriginalPixels[PixelIndex + 2];

        // 获取掩码值
        uint8 MaskValue = ProcessedMask[i];

        // 叠加半透明的绿色掩码（掩码区域显示为绿色调）
        if (MaskValue > 128) // 只对高置信度的掩码区域进行叠加
        {
            // 混合绿色：原图 * 0.7 + 绿色 * 0.3
            OverlayPixels[PixelIndex + 0] = static_cast<uint8>(OriginalB * 0.7f + 0 * 0.3f);      // B
            OverlayPixels[PixelIndex + 1] = static_cast<uint8>(OriginalG * 0.7f + 255 * 0.3f);    // G
            OverlayPixels[PixelIndex + 2] = static_cast<uint8>(OriginalR * 0.7f + 0 * 0.3f);      // R
        }
        else
        {
            // 保持原图
            OverlayPixels[PixelIndex + 0] = OriginalB;
            OverlayPixels[PixelIndex + 1] = OriginalG;
            OverlayPixels[PixelIndex + 2] = OriginalR;
        }
        OverlayPixels[PixelIndex + 3] = 255; // Alpha
    }

    // 在提示点位置绘制蓝色圆点
    for (int32 PointIndex = 0; PointIndex < Input.PromptPoints.Num(); ++PointIndex)
    {
        FVector2D Point = Input.PromptPoints[PointIndex];
        int32 Label = PointIndex < Input.PromptLabels.Num() ? Input.PromptLabels[PointIndex] : 1;

        // 转换相对坐标到像素坐标
        int32 PointX = static_cast<int32>(Point.X * ImageWidth);
        int32 PointY = static_cast<int32>(Point.Y * ImageHeight);

        // 确保坐标在有效范围内
        PointX = FMath::Clamp(PointX, 5, ImageWidth - 6);
        PointY = FMath::Clamp(PointY, 5, ImageHeight - 6);

        // 选择颜色：前景点用蓝色，背景点用红色
        uint8 PointB = Label > 0 ? 255 : 0;   // 蓝色分量
        uint8 PointG = 0;                      // 绿色分量
        uint8 PointR = Label > 0 ? 0 : 255;   // 红色分量

        // 绘制5x5的实心圆点
        for (int32 dy = -2; dy <= 2; ++dy)
        {
            for (int32 dx = -2; dx <= 2; ++dx)
            {
                if (dx * dx + dy * dy <= 4) // 圆形判断
                {
                    int32 DrawX = PointX + dx;
                    int32 DrawY = PointY + dy;

                    if (DrawX >= 0 && DrawX < ImageWidth && DrawY >= 0 && DrawY < ImageHeight)
                    {
                        int32 DrawIndex = (DrawY * ImageWidth + DrawX) * 4;
                        OverlayPixels[DrawIndex + 0] = PointB; // B
                        OverlayPixels[DrawIndex + 1] = PointG; // G
                        OverlayPixels[DrawIndex + 2] = PointR; // R
                        OverlayPixels[DrawIndex + 3] = 255;    // A
                    }
                }
            }
        }

        UE_LOG(LogTemp, Log, TEXT("Drew %s point at pixel (%d, %d)"), 
               Label > 0 ? TEXT("foreground (blue)") : TEXT("background (red)"), PointX, PointY);
    }

    // 解锁纹理数据
    OriginalMip.BulkData.Unlock();
    OverlayMip.BulkData.Unlock();
    OverlayTexture->UpdateResource();

    UE_LOG(LogTemp, Log, TEXT("Created overlay texture %dx%d with %d prompt points"), 
           ImageWidth, ImageHeight, Input.PromptPoints.Num());

    return OverlayTexture;
}

bool USam2Component::ConvertTextureToFloatArray(UTexture2D* Texture, TArray<float>& ImageData, int32& Width, int32& Height)
{
    if (!Texture)
    {
        return false;
    }

    Width = Texture->GetSizeX();
    Height = Texture->GetSizeY();

    // 检查纹理格式
    if (Texture->GetPixelFormat() != PF_B8G8R8A8)
    {
        UE_LOG(LogTemp, Warning, TEXT("Texture format is not BGRA8, conversion may not work correctly"));
    }

    // 读取纹理数据
    FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
    void* TextureData = Mip.BulkData.Lock(LOCK_READ_ONLY);

    if (!TextureData)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to lock texture data"));
        return false;
    }

    uint8* Pixels = static_cast<uint8*>(TextureData);

    // 转换为RGB浮点数组（HWC格式）
    ImageData.Empty(Width * Height * 3);
    ImageData.AddZeroed(Width * Height * 3);

    for (int32 y = 0; y < Height; ++y)
    {
        for (int32 x = 0; x < Width; ++x)
        {
            int32 PixelIndex = y * Width + x;
            int32 ByteIndex = PixelIndex * 4; // BGRA格式

            // UE5的纹理是BGRA格式，需要转换为RGB
            uint8 B = Pixels[ByteIndex + 0];
            uint8 G = Pixels[ByteIndex + 1];
            uint8 R = Pixels[ByteIndex + 2];

            // 转换为[0,1]范围的浮点值，HWC格式
            int32 HWCIndex = PixelIndex * 3;
            ImageData[HWCIndex + 0] = R / 255.0f;     // R通道
            ImageData[HWCIndex + 1] = G / 255.0f;     // G通道
            ImageData[HWCIndex + 2] = B / 255.0f;     // B通道
        }
    }

    Mip.BulkData.Unlock();

    UE_LOG(LogTemp, Log, TEXT("Converted texture %dx%d to float array with %d elements"), Width, Height, ImageData.Num());
    return true;
}