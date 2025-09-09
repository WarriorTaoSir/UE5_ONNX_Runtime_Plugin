// Sam2ModelInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"

// 包含ONNX Runtime的实现头文件
#if PLATFORM_WINDOWS && PLATFORM_64BITS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif
#include "onnxruntime_cxx_api.h"
#if PLATFORM_WINDOWS && PLATFORM_64BITS
#include "Windows/HideWindowsPlatformTypes.h"
#endif

#include "Sam2ModelInstance.generated.h"

/**
 * SAM2输入数据结构
 */
USTRUCT(BlueprintType)
struct CLOTH_API FSam2Input
{
	GENERATED_BODY()

	// 图像数据，HWC格式 [Height * Width * 3]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Input")
	TArray<float> ImageData;

	// 图像尺寸
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Input")
	int32 ImageWidth = 1024;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Input")
	int32 ImageHeight = 1024;

	// 提示点坐标 (在1024x1024画布坐标系中)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Input")
	TArray<FVector2D> PromptPoints;

	// 提示点标签 (1=前景点, 0=背景点)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Input")
	TArray<int32> PromptLabels;

	FSam2Input()
	{
		// 预分配1024x1024 RGB图像空间
		ImageData.Empty(1024 * 1024 * 3);
		ImageData.AddZeroed(1024 * 1024 * 3);
	}
};

/**
 * SAM2输出数据结构
 */
USTRUCT(BlueprintType)
struct CLOTH_API FSam2Output
{
	GENERATED_BODY()

	// 掩码数据，每个掩码为1024x1024
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Output")
	TArray<float> MaskData;

	// IoU预测分数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Output")
	TArray<float> IouScores;

	// 掩码数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Output")
	int32 NumMasks = 0;

	// 掩码尺寸
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Output")
	int32 MaskWidth = 1024;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Output")
	int32 MaskHeight = 1024;

	// 原始图像尺寸（用于后处理）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Output")
	int32 OriginalWidth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Output")
	int32 OriginalHeight = 0;

	// 缩放和偏移参数（用于后处理）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Output")
	float Scale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Output")
	int32 XOffset = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SAM2 Output")
	int32 YOffset = 0;

	// 获取指定索引的掩码数据
	TArray<float> GetMask(int32 MaskIndex) const
	{
		if (MaskIndex < 0 || MaskIndex >= NumMasks)
		{
			return TArray<float>();
		}

		int32 MaskSize = MaskWidth * MaskHeight;
		int32 StartIndex = MaskIndex * MaskSize;

		TArray<float> SingleMask;
		SingleMask.Empty(MaskSize);
		
		for (int32 i = 0; i < MaskSize && (StartIndex + i) < MaskData.Num(); ++i)
		{
			SingleMask.Add(MaskData[StartIndex + i]);
		}

		return SingleMask;
	}
};

/**
 * FSam2ModelInstance
 * SAM2模型的核心推理类，管理encoder-decoder架构的ONNX Runtime会话
 */
class CLOTH_API FSam2ModelInstance
{
public:
	// 构造函数：从给定的encoder和decoder模型路径创建实例
	FSam2ModelInstance(const FString& EncoderPath, const FString& DecoderPath);

	// 析构函数：清理ONNX Runtime会话和环境
	~FSam2ModelInstance();

	// 检查SAM2模型是否已成功初始化
	bool IsInitialized() const;

	// 运行SAM2推理
	bool RunInference(const FSam2Input& Input, FSam2Output& Output);

	// 图像预处理：将任意尺寸图像转换为1024x1024标准化格式
	bool PreprocessImage(const TArray<float>& InputImageData, int32 InputWidth, int32 InputHeight,
						 TArray<float>& ProcessedImageData, float& OutScale, int32& OutXOffset, int32& OutYOffset);

	// 掩码后处理：将1024x1024掩码转换回原始图像尺寸
	bool PostprocessMask(const TArray<float>& MaskData, int32 OriginalWidth, int32 OriginalHeight,
						 float Scale, int32 XOffset, int32 YOffset, TArray<uint8>& FinalMask);

private:
	// 禁用复制
	FSam2ModelInstance(const FSam2ModelInstance&) = delete;
	FSam2ModelInstance& operator=(const FSam2ModelInstance&) = delete;

	// ONNX Runtime环境
	TUniquePtr<Ort::Env> Env;

	// Encoder会话
	TUniquePtr<Ort::Session> EncoderSession;

	// Decoder会话
	TUniquePtr<Ort::Session> DecoderSession;

	// 模型路径
	FString EncoderModelPath;
	FString DecoderModelPath;

	// 初始化标志
	bool bIsInitialized = false;

	// 缓存的编码器输出（用于同一图像的多次推理）
	TArray<float> CachedImageEmbed;
	TArray<float> CachedHighResFeats0;
	TArray<float> CachedHighResFeats1;
	bool bHasCachedFeatures = false;

	// 内部初始化函数
	bool InitializeEncoder();
	bool InitializeDecoder();

	// 运行编码器
	bool RunEncoder(const TArray<float>& ImageData);

	// 运行解码器
	bool RunDecoder(const FSam2Input& Input, FSam2Output& Output);

	// 创建ONNX Runtime张量
	Ort::Value CreateTensor(const TArray<float>& Data, const std::vector<int64_t>& Shape);
	Ort::Value CreateTensor(const TArray<int32>& Data, const std::vector<int64_t>& Shape);

	// 辅助函数：坐标转换
	void TransformPromptPoints(const TArray<FVector2D>& InputPoints, int32 InputWidth, int32 InputHeight,
							   float Scale, int32 XOffset, int32 YOffset, TArray<float>& OutputCoords);

	// 辅助函数：应用sigmoid
	void ApplySigmoid(TArray<float>& Data);
};