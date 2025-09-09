// ImageLoadHelper.cpp

#include "ImageLoadHelper.h"
#include "Engine/Engine.h"
#include "Engine/AssetManager.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "TextureResource.h"
#include "Engine/TextureLODSettings.h"

// 定义锁定常量（兼容不同UE版本）
#ifndef LOCK_READ_ONLY
#define LOCK_READ_ONLY 1
#endif
#ifndef LOCK_WRITE_ONLY
#define LOCK_WRITE_ONLY 2
#endif

UTexture2D* UImageLoadHelper::LoadTextureFromAssetPath(const FString& AssetPath)
{
    // 尝试从资产路径加载纹理
    if (AssetPath.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Asset path is empty"));
        return nullptr;
    }

    // 构造完整的资产路径
    FString FullAssetPath = AssetPath;
    if (!FullAssetPath.StartsWith(TEXT("/Game/")))
    {
        FullAssetPath = FString::Printf(TEXT("/Game/%s"), *AssetPath);
    }

    // 加载资产
    UObject* LoadedObject = StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FullAssetPath);
    UTexture2D* LoadedTexture = Cast<UTexture2D>(LoadedObject);

    if (LoadedTexture)
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully loaded texture from asset path: %s"), *FullAssetPath);
        return LoadedTexture;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load texture from asset path: %s"), *FullAssetPath);
        return nullptr;
    }
}

UTexture2D* UImageLoadHelper::LoadTextureFromFile(const FString& FilePath)
{
    if (!FPaths::FileExists(FilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Image file not found: %s"), *FilePath);
        return nullptr;
    }

    // 读取文件数据
    TArray<uint8> ImageData;
    if (!FFileHelper::LoadFileToArray(ImageData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to read image file: %s"), *FilePath);
        return nullptr;
    }

    // 获取文件扩展名
    FString Extension = FPaths::GetExtension(FilePath).ToLower();
    
    // 获取图像包装器
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper;

    if (Extension == TEXT("png"))
    {
        ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
    }
    else if (Extension == TEXT("jpg") || Extension == TEXT("jpeg"))
    {
        ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
    }
    else if (Extension == TEXT("bmp"))
    {
        ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Unsupported image format: %s"), *Extension);
        return nullptr;
    }

    if (!ImageWrapper.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create image wrapper"));
        return nullptr;
    }

    // 解码图像
    if (!ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to set compressed data"));
        return nullptr;
    }

    // 获取原始数据
    TArray<uint8> RawData;
    if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get raw image data"));
        return nullptr;
    }

    // 创建纹理
    int32 Width = ImageWrapper->GetWidth();
    int32 Height = ImageWrapper->GetHeight();

    UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
    if (!NewTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create transient texture"));
        return nullptr;
    }

    // 复制像素数据
    FTexture2DMipMap& Mip = NewTexture->GetPlatformData()->Mips[0];
    void* TextureData = Mip.BulkData.Lock(LOCK_WRITE_ONLY);
    FMemory::Memcpy(TextureData, RawData.GetData(), RawData.Num());
    Mip.BulkData.Unlock();
    NewTexture->UpdateResource();

    UE_LOG(LogTemp, Log, TEXT("Successfully loaded texture from file: %s (%dx%d)"), *FilePath, Width, Height);
    return NewTexture;
}

bool UImageLoadHelper::SaveTextureAsPNG(UTexture2D* Texture, const FString& FilePath)
{
    if (!Texture)
    {
        UE_LOG(LogTemp, Error, TEXT("Texture is null"));
        return false;
    }

    // 确保目录存在
    FString Directory = FPaths::GetPath(FilePath);
    if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*Directory))
    {
        if (!FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*Directory))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create directory: %s"), *Directory);
            return false;
        }
    }

    // 读取纹理数据
    FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
    void* TextureData = Mip.BulkData.Lock(LOCK_READ_ONLY);

    if (!TextureData)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to lock texture data"));
        return false;
    }

    int32 Width = Texture->GetSizeX();
    int32 Height = Texture->GetSizeY();
    EPixelFormat Format = Texture->GetPixelFormat();

    // 转换为BGRA格式
    TArray<uint8> ImageData;

    if (Format == PF_G8)
    {
        // 灰度图转换为BGRA
        uint8* PixelData = static_cast<uint8*>(TextureData);
        ImageData.Empty(Width * Height * 4);

        for (int32 i = 0; i < Width * Height; ++i)
        {
            uint8 GrayValue = PixelData[i];
            ImageData.Add(GrayValue);  // B
            ImageData.Add(GrayValue);  // G
            ImageData.Add(GrayValue);  // R
            ImageData.Add(255);        // A
        }
    }
    else if (Format == PF_B8G8R8A8)
    {
        // 已经是BGRA格式
        uint8* PixelData = static_cast<uint8*>(TextureData);
        ImageData.Empty(Width * Height * 4);
        ImageData.AddUninitialized(Width * Height * 4);
        FMemory::Memcpy(ImageData.GetData(), PixelData, Width * Height * 4);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Unsupported pixel format: %d"), (int32)Format);
        Mip.BulkData.Unlock();
        return false;
    }

    Mip.BulkData.Unlock();

    // 获取PNG包装器
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

    if (!ImageWrapper.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create PNG image wrapper"));
        return false;
    }

    // 设置原始数据
    if (!ImageWrapper->SetRaw(ImageData.GetData(), ImageData.Num(), Width, Height, ERGBFormat::BGRA, 8))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to set raw image data"));
        return false;
    }

    // 获取压缩数据
    const TArray64<uint8>& CompressedData64 = ImageWrapper->GetCompressed();
    TArray<uint8> CompressedData(CompressedData64.GetData(), CompressedData64.Num());
    if (CompressedData.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to compress image data"));
        return false;
    }

    // 保存文件
    if (!FFileHelper::SaveArrayToFile(CompressedData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save file: %s"), *FilePath);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("Successfully saved texture to PNG: %s"), *FilePath);
    return true;
}

bool UImageLoadHelper::ConvertTextureToFloatArray(UTexture2D* Texture, TArray<float>& ImageData, int32& Width, int32& Height)
{
    if (!Texture)
    {
        UE_LOG(LogTemp, Error, TEXT("Texture is null"));
        return false;
    }

    Width = Texture->GetSizeX();
    Height = Texture->GetSizeY();

    // 检查纹理格式
    EPixelFormat Format = Texture->GetPixelFormat();
    if (Format != PF_B8G8R8A8)
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

UTexture2D* UImageLoadHelper::CreateTextureFromFloatArray(const TArray<float>& ImageData, int32 Width, int32 Height)
{
    if (ImageData.Num() != Width * Height * 3)
    {
        UE_LOG(LogTemp, Error, TEXT("Image data size mismatch"));
        return nullptr;
    }

    UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
    if (!NewTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create transient texture"));
        return nullptr;
    }

    FTexture2DMipMap& Mip = NewTexture->GetPlatformData()->Mips[0];
    void* TextureData = Mip.BulkData.Lock(LOCK_WRITE_ONLY);
    uint8* Pixels = static_cast<uint8*>(TextureData);

    for (int32 i = 0; i < Width * Height; ++i)
    {
        int32 FloatIndex = i * 3;
        int32 ByteIndex = i * 4;

        // 转换回[0,255]范围的字节值，并转换为BGRA格式
        Pixels[ByteIndex + 0] = FMath::Clamp(static_cast<int32>(ImageData[FloatIndex + 2] * 255.0f), 0, 255); // B
        Pixels[ByteIndex + 1] = FMath::Clamp(static_cast<int32>(ImageData[FloatIndex + 1] * 255.0f), 0, 255); // G
        Pixels[ByteIndex + 2] = FMath::Clamp(static_cast<int32>(ImageData[FloatIndex + 0] * 255.0f), 0, 255); // R
        Pixels[ByteIndex + 3] = 255; // A
    }

    Mip.BulkData.Unlock();
    NewTexture->UpdateResource();

    return NewTexture;
}

UTexture2D* UImageLoadHelper::CreateTextureFromByteArray(const TArray<uint8>& ImageData, int32 Width, int32 Height)
{
    if (ImageData.Num() != Width * Height)
    {
        UE_LOG(LogTemp, Error, TEXT("Byte data size mismatch"));
        return nullptr;
    }

    UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PF_G8);
    if (!NewTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create transient texture"));
        return nullptr;
    }

    FTexture2DMipMap& Mip = NewTexture->GetPlatformData()->Mips[0];
    void* TextureData = Mip.BulkData.Lock(LOCK_WRITE_ONLY);
    FMemory::Memcpy(TextureData, ImageData.GetData(), ImageData.Num());
    Mip.BulkData.Unlock();
    NewTexture->UpdateResource();

    return NewTexture;
}

UTexture2D* UImageLoadHelper::FindAndLoadTest01Image()
{
    FString ProjectDir = FPaths::ProjectDir();

    // 首先尝试加载已存在的PNG文件
    TArray<FString> PossibleFiles;
    PossibleFiles.Add(FPaths::Combine(ProjectDir, TEXT("Content"), TEXT("Dataset"), TEXT("Test01.png")));
    PossibleFiles.Add(FPaths::Combine(ProjectDir, TEXT("Content"), TEXT("Dataset"), TEXT("Test01.jpg")));
    PossibleFiles.Add(FPaths::Combine(ProjectDir, TEXT("Content"), TEXT("Dataset"), TEXT("Test01.jpeg")));

    for (const FString& FilePath : PossibleFiles)
    {
        if (FPaths::FileExists(FilePath))
        {
            UTexture2D* LoadedTexture = LoadTextureFromFile(FilePath);
            if (LoadedTexture)
            {
                UE_LOG(LogTemp, Log, TEXT("Found and loaded Test01 image: %s"), *FilePath);
                return LoadedTexture;
            }
        }
    }

    // 尝试从资产路径加载
    TArray<FString> AssetPaths;
    AssetPaths.Add(TEXT("Dataset/Test01"));
    AssetPaths.Add(TEXT("/Game/Dataset/Test01"));

    for (const FString& AssetPath : AssetPaths)
    {
        UTexture2D* LoadedTexture = LoadTextureFromAssetPath(AssetPath);
        if (LoadedTexture)
        {
            UE_LOG(LogTemp, Log, TEXT("Found and loaded Test01 asset: %s"), *AssetPath);

            // 自动导出为PNG以便下次使用
            FString ExportPath = FPaths::Combine(ProjectDir, TEXT("Content"), TEXT("Dataset"), TEXT("Test01_exported.png"));
            if (SaveTextureAsPNG(LoadedTexture, ExportPath))
            {
                UE_LOG(LogTemp, Log, TEXT("Exported Test01 asset to PNG: %s"), *ExportPath);
            }

            return LoadedTexture;
        }
    }

    UE_LOG(LogTemp, Error, TEXT("Test01 image not found in any format"));
    return nullptr;
}

bool UImageLoadHelper::ExportTest01ToPNG()
{
    UTexture2D* Test01Texture = FindAndLoadTest01Image();
    if (!Test01Texture)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot find Test01 texture to export"));
        return false;
    }

    FString ProjectDir = FPaths::ProjectDir();
    FString ExportPath = FPaths::Combine(ProjectDir, TEXT("Content"), TEXT("Dataset"), TEXT("Test01.png"));

    return SaveTextureAsPNG(Test01Texture, ExportPath);
}