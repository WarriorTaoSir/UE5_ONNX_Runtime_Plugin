// ImageLoadHelper.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ImageLoadHelper.generated.h"

/**
 * UImageLoadHelper
 * 图像加载辅助工具类，提供各种图像加载和保存功能
 */
UCLASS()
class CLOTH_API UImageLoadHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // 从资产引用路径加载纹理
    UFUNCTION(BlueprintCallable, Category = "Image Helper")
    static UTexture2D* LoadTextureFromAssetPath(const FString& AssetPath);

    // 从文件路径加载纹理（支持PNG、JPG等格式）
    UFUNCTION(BlueprintCallable, Category = "Image Helper")
    static UTexture2D* LoadTextureFromFile(const FString& FilePath);

    // 将纹理保存为PNG文件
    UFUNCTION(BlueprintCallable, Category = "Image Helper")
    static bool SaveTextureAsPNG(UTexture2D* Texture, const FString& FilePath);

    // 将纹理转换为浮点数组（用于SAM2输入）
    UFUNCTION(BlueprintCallable, Category = "Image Helper")
    static bool ConvertTextureToFloatArray(UTexture2D* Texture, TArray<float>& ImageData, int32& Width, int32& Height);

    // 从浮点数组创建纹理
    UFUNCTION(BlueprintCallable, Category = "Image Helper")
    static UTexture2D* CreateTextureFromFloatArray(const TArray<float>& ImageData, int32 Width, int32 Height);

    // 从字节数组创建纹理（用于掩码）
    UFUNCTION(BlueprintCallable, Category = "Image Helper")
    static UTexture2D* CreateTextureFromByteArray(const TArray<uint8>& ImageData, int32 Width, int32 Height);

    // 查找Content/Dataset中的Test01图像
    UFUNCTION(BlueprintCallable, Category = "Image Helper")
    static UTexture2D* FindAndLoadTest01Image();

    // 将Test01.uasset导出为PNG
    UFUNCTION(BlueprintCallable, Category = "Image Helper")
    static bool ExportTest01ToPNG();
};