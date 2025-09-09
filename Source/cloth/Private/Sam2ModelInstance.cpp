// Sam2ModelInstance.cpp

#include "Sam2ModelInstance.h"
#include "HAL/PlatformFilemanager.h"
#include "Interfaces/IPluginManager.h"

// 包含ONNX Runtime的实现头文件
#if PLATFORM_WINDOWS && PLATFORM_64BITS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif
#include "onnxruntime_cxx_api.h"
#if PLATFORM_WINDOWS && PLATFORM_64BITS
#include "Windows/HideWindowsPlatformTypes.h"
#endif

FSam2ModelInstance::FSam2ModelInstance(const FString& EncoderPath, const FString& DecoderPath)
    : EncoderModelPath(EncoderPath)
    , DecoderModelPath(DecoderPath)
    , bIsInitialized(false)
    , bHasCachedFeatures(false)
{
    UE_LOG(LogTemp, Log, TEXT("Creating FSam2ModelInstance..."));
    UE_LOG(LogTemp, Log, TEXT("Encoder Path: %s"), *EncoderPath);
    UE_LOG(LogTemp, Log, TEXT("Decoder Path: %s"), *DecoderPath);

    try
    {
        // 创建ONNX环境
        Env = MakeUnique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "SAM2Model");
        UE_LOG(LogTemp, Log, TEXT("ONNX Environment created"));

        // 初始化编码器和解码器
        if (InitializeEncoder() && InitializeDecoder())
        {
            bIsInitialized = true;
            UE_LOG(LogTemp, Log, TEXT("SAM2 Model Instance initialized successfully"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to initialize SAM2 models"));
        }
    }
    catch (const Ort::Exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("ONNX Runtime error in SAM2 constructor: %s"), UTF8_TO_TCHAR(e.what()));
        bIsInitialized = false;
    }
    catch (const std::exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("Standard exception in SAM2 constructor: %s"), UTF8_TO_TCHAR(e.what()));
        bIsInitialized = false;
    }
}

FSam2ModelInstance::~FSam2ModelInstance()
{
    UE_LOG(LogTemp, Log, TEXT("Destroying FSam2ModelInstance"));
}

bool FSam2ModelInstance::IsInitialized() const
{
    return bIsInitialized;
}

bool FSam2ModelInstance::InitializeEncoder()
{
    try
    {
        if (!FPaths::FileExists(EncoderModelPath))
        {
            UE_LOG(LogTemp, Error, TEXT("Encoder model file not found: %s"), *EncoderModelPath);
            return false;
        }

        // 创建会话选项
        Ort::SessionOptions sessionOptions;
        sessionOptions.SetIntraOpNumThreads(1);

        // 创建编码器会话
        EncoderSession = MakeUnique<Ort::Session>(*Env, *EncoderModelPath, sessionOptions);
        
        UE_LOG(LogTemp, Log, TEXT("Encoder session created successfully"));

        // 获取编码器输入输出信息
        size_t numInputNodes = EncoderSession->GetInputCount();
        size_t numOutputNodes = EncoderSession->GetOutputCount();

        UE_LOG(LogTemp, Log, TEXT("Encoder - Inputs: %d, Outputs: %d"), numInputNodes, numOutputNodes);

        // 记录输入输出名称
        Ort::AllocatorWithDefaultOptions allocator;
        for (size_t i = 0; i < numInputNodes; ++i)
        {
            auto inputName = EncoderSession->GetInputNameAllocated(i, allocator);
            UE_LOG(LogTemp, Log, TEXT("Encoder Input %d: %s"), i, UTF8_TO_TCHAR(inputName.get()));
        }

        for (size_t i = 0; i < numOutputNodes; ++i)
        {
            auto outputName = EncoderSession->GetOutputNameAllocated(i, allocator);
            UE_LOG(LogTemp, Log, TEXT("Encoder Output %d: %s"), i, UTF8_TO_TCHAR(outputName.get()));
        }

        return true;
    }
    catch (const Ort::Exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize encoder: %s"), UTF8_TO_TCHAR(e.what()));
        return false;
    }
}

bool FSam2ModelInstance::InitializeDecoder()
{
    try
    {
        if (!FPaths::FileExists(DecoderModelPath))
        {
            UE_LOG(LogTemp, Error, TEXT("Decoder model file not found: %s"), *DecoderModelPath);
            return false;
        }

        // 创建会话选项
        Ort::SessionOptions sessionOptions;
        sessionOptions.SetIntraOpNumThreads(1);

        // 创建解码器会话
        DecoderSession = MakeUnique<Ort::Session>(*Env, *DecoderModelPath, sessionOptions);
        
        UE_LOG(LogTemp, Log, TEXT("Decoder session created successfully"));

        // 获取解码器输入输出信息
        size_t numInputNodes = DecoderSession->GetInputCount();
        size_t numOutputNodes = DecoderSession->GetOutputCount();

        UE_LOG(LogTemp, Log, TEXT("Decoder - Inputs: %d, Outputs: %d"), numInputNodes, numOutputNodes);

        // 记录输入输出名称
        Ort::AllocatorWithDefaultOptions allocator;
        for (size_t i = 0; i < numInputNodes; ++i)
        {
            auto inputName = DecoderSession->GetInputNameAllocated(i, allocator);
            UE_LOG(LogTemp, Log, TEXT("Decoder Input %d: %s"), i, UTF8_TO_TCHAR(inputName.get()));
        }

        for (size_t i = 0; i < numOutputNodes; ++i)
        {
            auto outputName = DecoderSession->GetOutputNameAllocated(i, allocator);
            UE_LOG(LogTemp, Log, TEXT("Decoder Output %d: %s"), i, UTF8_TO_TCHAR(outputName.get()));
        }

        return true;
    }
    catch (const Ort::Exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize decoder: %s"), UTF8_TO_TCHAR(e.what()));
        return false;
    }
}

bool FSam2ModelInstance::RunInference(const FSam2Input& Input, FSam2Output& Output)
{
    if (!bIsInitialized)
    {
        UE_LOG(LogTemp, Error, TEXT("SAM2 model not initialized"));
        return false;
    }

    if (Input.ImageData.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Input image data is empty"));
        return false;
    }

    if (Input.PromptPoints.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No prompt points provided"));
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("Running SAM2 inference with %d prompt points"), Input.PromptPoints.Num());

    try
    {
        // 步骤1: 预处理图像
        TArray<float> ProcessedImageData;
        float Scale;
        int32 XOffset, YOffset;

        if (!PreprocessImage(Input.ImageData, Input.ImageWidth, Input.ImageHeight,
                           ProcessedImageData, Scale, XOffset, YOffset))
        {
            UE_LOG(LogTemp, Error, TEXT("Image preprocessing failed"));
            return false;
        }

        // 步骤2: 运行编码器
        if (!RunEncoder(ProcessedImageData))
        {
            UE_LOG(LogTemp, Error, TEXT("Encoder inference failed"));
            return false;
        }

        // 步骤3: 设置输出参数用于后处理
        Output.OriginalWidth = Input.ImageWidth;
        Output.OriginalHeight = Input.ImageHeight;
        Output.Scale = Scale;
        Output.XOffset = XOffset;
        Output.YOffset = YOffset;

        // 步骤4: 运行解码器
        if (!RunDecoder(Input, Output))
        {
            UE_LOG(LogTemp, Error, TEXT("Decoder inference failed"));
            return false;
        }

        UE_LOG(LogTemp, Log, TEXT("SAM2 inference completed successfully"));
        return true;
    }
    catch (const Ort::Exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("ONNX Runtime error during inference: %s"), UTF8_TO_TCHAR(e.what()));
        return false;
    }
    catch (const std::exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("Standard exception during inference: %s"), UTF8_TO_TCHAR(e.what()));
        return false;
    }
}

bool FSam2ModelInstance::PreprocessImage(const TArray<float>& InputImageData, int32 InputWidth, int32 InputHeight,
                                        TArray<float>& ProcessedImageData, float& OutScale, int32& OutXOffset, int32& OutYOffset)
{
    if (InputImageData.Num() != InputWidth * InputHeight * 3)
    {
        UE_LOG(LogTemp, Error, TEXT("Input image data size mismatch: expected %d, got %d"), 
               InputWidth * InputHeight * 3, InputImageData.Num());
        return false;
    }

    const int32 TargetSize = 1024;
    
    // 计算缩放比例，保持宽高比
    float ScaleW = float(TargetSize) / float(InputWidth);
    float ScaleH = float(TargetSize) / float(InputHeight);
    OutScale = FMath::Min(ScaleW, ScaleH);
    
    int32 NewWidth = FMath::RoundToInt(InputWidth * OutScale);
    int32 NewHeight = FMath::RoundToInt(InputHeight * OutScale);
    
    // 计算居中偏移
    OutXOffset = (TargetSize - NewWidth) / 2;
    OutYOffset = (TargetSize - NewHeight) / 2;

    UE_LOG(LogTemp, Log, TEXT("Image preprocessing: %dx%d -> %dx%d, scale=%.3f, offset=(%d,%d)"), 
           InputWidth, InputHeight, NewWidth, NewHeight, OutScale, OutXOffset, OutYOffset);

    // 创建1024x1024的画布
    ProcessedImageData.Empty(TargetSize * TargetSize * 3);
    ProcessedImageData.AddZeroed(TargetSize * TargetSize * 3);

    // 双线性插值缩放并放置到画布中央
    for (int32 y = 0; y < NewHeight; ++y)
    {
        for (int32 x = 0; x < NewWidth; ++x)
        {
            // 源坐标
            float SrcX = x / OutScale;
            float SrcY = y / OutScale;

            int32 SrcX0 = FMath::FloorToInt(SrcX);
            int32 SrcY0 = FMath::FloorToInt(SrcY);
            int32 SrcX1 = FMath::Min(SrcX0 + 1, InputWidth - 1);
            int32 SrcY1 = FMath::Min(SrcY0 + 1, InputHeight - 1);

            float WeightX = SrcX - SrcX0;
            float WeightY = SrcY - SrcY0;

            // 目标坐标
            int32 DstX = x + OutXOffset;
            int32 DstY = y + OutYOffset;

            if (DstX >= 0 && DstX < TargetSize && DstY >= 0 && DstY < TargetSize)
            {
                int32 DstIndex = (DstY * TargetSize + DstX) * 3;

                for (int32 c = 0; c < 3; ++c)
                {
                    // 双线性插值
                    float P00 = InputImageData[(SrcY0 * InputWidth + SrcX0) * 3 + c];
                    float P01 = InputImageData[(SrcY0 * InputWidth + SrcX1) * 3 + c];
                    float P10 = InputImageData[(SrcY1 * InputWidth + SrcX0) * 3 + c];
                    float P11 = InputImageData[(SrcY1 * InputWidth + SrcX1) * 3 + c];

                    float InterpolatedValue = P00 * (1 - WeightX) * (1 - WeightY) +
                                            P01 * WeightX * (1 - WeightY) +
                                            P10 * (1 - WeightX) * WeightY +
                                            P11 * WeightX * WeightY;

                    ProcessedImageData[DstIndex + c] = InterpolatedValue;
                }
            }
        }
    }

    // ImageNet标准化
    const float Mean[3] = {0.485f, 0.456f, 0.406f};
    const float Std[3] = {0.229f, 0.224f, 0.225f};

    for (int32 i = 0; i < TargetSize * TargetSize; ++i)
    {
        for (int32 c = 0; c < 3; ++c)
        {
            int32 Index = i * 3 + c;
            ProcessedImageData[Index] = (ProcessedImageData[Index] - Mean[c]) / Std[c];
        }
    }

    // 转换为NCHW格式（ONNX期望的格式）
    TArray<float> NCHWData;
    NCHWData.Empty(TargetSize * TargetSize * 3);
    NCHWData.AddUninitialized(TargetSize * TargetSize * 3);

    for (int32 c = 0; c < 3; ++c)
    {
        for (int32 h = 0; h < TargetSize; ++h)
        {
            for (int32 w = 0; w < TargetSize; ++w)
            {
                int32 HWCIndex = (h * TargetSize + w) * 3 + c;
                int32 NCHWIndex = c * TargetSize * TargetSize + h * TargetSize + w;
                NCHWData[NCHWIndex] = ProcessedImageData[HWCIndex];
            }
        }
    }

    ProcessedImageData = MoveTemp(NCHWData);
    return true;
}

bool FSam2ModelInstance::RunEncoder(const TArray<float>& ImageData)
{
    try
    {
        Ort::AllocatorWithDefaultOptions allocator;

        // 创建输入张量 [1, 3, 1024, 1024]
        std::vector<int64_t> inputShape = {1, 3, 1024, 1024};
        Ort::Value inputTensor = CreateTensor(ImageData, inputShape);

        // 输入名称
        const char* inputNames[] = {"image"};

        // 输出名称
        const char* outputNames[] = {"high_res_feats_0", "high_res_feats_1", "image_embed"};

        // 运行推理
        auto outputs = EncoderSession->Run(Ort::RunOptions{nullptr}, inputNames, &inputTensor, 1, outputNames, 3);

        if (outputs.size() != 3)
        {
            UE_LOG(LogTemp, Error, TEXT("Encoder returned unexpected number of outputs: %d"), outputs.size());
            return false;
        }

        // 缓存编码器输出
        // high_res_feats_0: [1, 32, 256, 256]
        auto& feats0Tensor = outputs[0];
        auto feats0Shape = feats0Tensor.GetTensorTypeAndShapeInfo();
        float* feats0Data = feats0Tensor.GetTensorMutableData<float>();
        size_t feats0Size = feats0Shape.GetElementCount();

        CachedHighResFeats0.Empty(feats0Size);
        CachedHighResFeats0.AddUninitialized(feats0Size);
        FMemory::Memcpy(CachedHighResFeats0.GetData(), feats0Data, feats0Size * sizeof(float));

        // high_res_feats_1: [1, 64, 128, 128]
        auto& feats1Tensor = outputs[1];
        auto feats1Shape = feats1Tensor.GetTensorTypeAndShapeInfo();
        float* feats1Data = feats1Tensor.GetTensorMutableData<float>();
        size_t feats1Size = feats1Shape.GetElementCount();

        CachedHighResFeats1.Empty(feats1Size);
        CachedHighResFeats1.AddUninitialized(feats1Size);
        FMemory::Memcpy(CachedHighResFeats1.GetData(), feats1Data, feats1Size * sizeof(float));

        // image_embed: [1, 256, 64, 64]
        auto& embedTensor = outputs[2];
        auto embedShape = embedTensor.GetTensorTypeAndShapeInfo();
        float* embedData = embedTensor.GetTensorMutableData<float>();
        size_t embedSize = embedShape.GetElementCount();

        CachedImageEmbed.Empty(embedSize);
        CachedImageEmbed.AddUninitialized(embedSize);
        FMemory::Memcpy(CachedImageEmbed.GetData(), embedData, embedSize * sizeof(float));

        bHasCachedFeatures = true;

        UE_LOG(LogTemp, Log, TEXT("Encoder inference completed, cached features: feats0=%d, feats1=%d, embed=%d"), 
               CachedHighResFeats0.Num(), CachedHighResFeats1.Num(), CachedImageEmbed.Num());

        return true;
    }
    catch (const Ort::Exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("Encoder inference error: %s"), UTF8_TO_TCHAR(e.what()));
        return false;
    }
}

bool FSam2ModelInstance::RunDecoder(const FSam2Input& Input, FSam2Output& Output)
{
    if (!bHasCachedFeatures)
    {
        UE_LOG(LogTemp, Error, TEXT("No cached encoder features available"));
        return false;
    }

    try
    {
        Ort::AllocatorWithDefaultOptions allocator;

        // 准备解码器输入数据
        
        // 1. image_embed [1, 256, 64, 64]
        std::vector<int64_t> embedShape = {1, 256, 64, 64};
        Ort::Value embedTensor = CreateTensor(CachedImageEmbed, embedShape);

        // 2. high_res_feats_0 [1, 32, 256, 256]
        std::vector<int64_t> feats0Shape = {1, 32, 256, 256};
        Ort::Value feats0Tensor = CreateTensor(CachedHighResFeats0, feats0Shape);

        // 3. high_res_feats_1 [1, 64, 128, 128]
        std::vector<int64_t> feats1Shape = {1, 64, 128, 128};
        Ort::Value feats1Tensor = CreateTensor(CachedHighResFeats1, feats1Shape);

        // 4. point_coords [1, N, 2] - 转换提示点坐标
        TArray<float> PointCoords;
        TransformPromptPoints(Input.PromptPoints, Input.ImageWidth, Input.ImageHeight, 
                             Output.Scale, Output.XOffset, Output.YOffset, PointCoords);
        
        std::vector<int64_t> coordsShape = {1, static_cast<int64_t>(Input.PromptPoints.Num()), 2};
        Ort::Value coordsTensor = CreateTensor(PointCoords, coordsShape);

        // 5. point_labels [1, N]
        TArray<float> PointLabels;
        for (int32 Label : Input.PromptLabels)
        {
            PointLabels.Add(static_cast<float>(Label));
        }
        std::vector<int64_t> labelsShape = {1, static_cast<int64_t>(Input.PromptLabels.Num())};
        Ort::Value labelsTensor = CreateTensor(PointLabels, labelsShape);

        // 6. mask_input [1, 1, 256, 256] - 全零
        TArray<float> MaskInput;
        MaskInput.AddZeroed(1 * 1 * 256 * 256);
        std::vector<int64_t> maskShape = {1, 1, 256, 256};
        Ort::Value maskTensor = CreateTensor(MaskInput, maskShape);

        // 7. has_mask_input [1] - 值为0
        TArray<float> HasMaskInput = {0.0f};
        std::vector<int64_t> hasMaskShape = {1};
        Ort::Value hasMaskTensor = CreateTensor(HasMaskInput, hasMaskShape);

        // 8. orig_im_size [2] - 强制为[1024, 1024]
        TArray<int32> OrigImSize = {1024, 1024};
        std::vector<int64_t> origSizeShape = {2};
        Ort::Value origSizeTensor = CreateTensor(OrigImSize, origSizeShape);

        // 准备输入数组
        std::vector<Ort::Value> inputs;
        inputs.push_back(std::move(embedTensor));
        inputs.push_back(std::move(feats0Tensor));
        inputs.push_back(std::move(feats1Tensor));
        inputs.push_back(std::move(coordsTensor));
        inputs.push_back(std::move(labelsTensor));
        inputs.push_back(std::move(maskTensor));
        inputs.push_back(std::move(hasMaskTensor));
        inputs.push_back(std::move(origSizeTensor));

        // 输入名称
        const char* inputNames[] = {
            "image_embed", "high_res_feats_0", "high_res_feats_1", "point_coords",
            "point_labels", "mask_input", "has_mask_input", "orig_im_size"
        };

        // 输出名称
        const char* outputNames[] = {"masks", "iou_predictions"};

        // 运行推理
        auto outputs = DecoderSession->Run(Ort::RunOptions{nullptr}, inputNames, inputs.data(), 8, outputNames, 2);

        if (outputs.size() != 2)
        {
            UE_LOG(LogTemp, Error, TEXT("Decoder returned unexpected number of outputs: %d"), outputs.size());
            return false;
        }

        // 处理掩码输出 [1, 1, 1024, 1024]
        auto& masksTensor = outputs[0];
        auto masksShape = masksTensor.GetTensorTypeAndShapeInfo();
        float* masksData = masksTensor.GetTensorMutableData<float>();
        size_t masksSize = masksShape.GetElementCount();

        Output.MaskData.Empty(masksSize);
        Output.MaskData.AddUninitialized(masksSize);
        FMemory::Memcpy(Output.MaskData.GetData(), masksData, masksSize * sizeof(float));

        // 应用sigmoid激活
        ApplySigmoid(Output.MaskData);

        // 处理IoU输出 [1, 1]
        auto& iouTensor = outputs[1];
        auto iouShape = iouTensor.GetTensorTypeAndShapeInfo();
        float* iouData = iouTensor.GetTensorMutableData<float>();
        size_t iouSize = iouShape.GetElementCount();

        Output.IouScores.Empty(iouSize);
        Output.IouScores.AddUninitialized(iouSize);
        FMemory::Memcpy(Output.IouScores.GetData(), iouData, iouSize * sizeof(float));

        // 设置输出元数据
        Output.NumMasks = 1;  // SAM2通常输出1个掩码
        Output.MaskWidth = 1024;
        Output.MaskHeight = 1024;

        UE_LOG(LogTemp, Log, TEXT("Decoder inference completed, mask size=%d, IoU=%.3f"), 
               Output.MaskData.Num(), Output.IouScores.Num() > 0 ? Output.IouScores[0] : 0.0f);

        return true;
    }
    catch (const Ort::Exception& e)
    {
        UE_LOG(LogTemp, Error, TEXT("Decoder inference error: %s"), UTF8_TO_TCHAR(e.what()));
        return false;
    }
}

Ort::Value FSam2ModelInstance::CreateTensor(const TArray<float>& Data, const std::vector<int64_t>& Shape)
{
    Ort::MemoryInfo memInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    return Ort::Value::CreateTensor<float>(memInfo, const_cast<float*>(Data.GetData()), Data.Num(), Shape.data(), Shape.size());
}

Ort::Value FSam2ModelInstance::CreateTensor(const TArray<int32>& Data, const std::vector<int64_t>& Shape)
{
    Ort::MemoryInfo memInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    return Ort::Value::CreateTensor<int32_t>(memInfo, const_cast<int32_t*>(Data.GetData()), Data.Num(), Shape.data(), Shape.size());
}

void FSam2ModelInstance::TransformPromptPoints(const TArray<FVector2D>& InputPoints, int32 InputWidth, int32 InputHeight,
                                              float Scale, int32 XOffset, int32 YOffset, TArray<float>& OutputCoords)
{
    OutputCoords.Empty(InputPoints.Num() * 2);

    for (const FVector2D& Point : InputPoints)
    {
        // 将相对坐标转换为画布坐标
        float CanvasX = XOffset + Point.X * InputWidth * Scale;
        float CanvasY = YOffset + Point.Y * InputHeight * Scale;

        // 确保坐标在有效范围内
        CanvasX = FMath::Clamp(CanvasX, 0.0f, 1024.0f);
        CanvasY = FMath::Clamp(CanvasY, 0.0f, 1024.0f);

        OutputCoords.Add(CanvasX);
        OutputCoords.Add(CanvasY);

        UE_LOG(LogTemp, Log, TEXT("Transformed point (%.3f, %.3f) -> (%.3f, %.3f)"), 
               Point.X, Point.Y, CanvasX, CanvasY);
    }
}

void FSam2ModelInstance::ApplySigmoid(TArray<float>& Data)
{
    for (float& Value : Data)
    {
        Value = 1.0f / (1.0f + FMath::Exp(-Value));
    }
}

bool FSam2ModelInstance::PostprocessMask(const TArray<float>& MaskData, int32 OriginalWidth, int32 OriginalHeight,
                                        float Scale, int32 XOffset, int32 YOffset, TArray<uint8>& FinalMask)
{
    if (MaskData.Num() != 1024 * 1024)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid mask data size: %d"), MaskData.Num());
        return false;
    }

    // 二值化掩码
    TArray<uint8> BinaryMask;
    BinaryMask.Empty(MaskData.Num());
    for (float Value : MaskData)
    {
        BinaryMask.Add(Value > 0.5f ? 255 : 0);
    }

    // 提取有效区域
    int32 ScaledWidth = FMath::RoundToInt(OriginalWidth * Scale);
    int32 ScaledHeight = FMath::RoundToInt(OriginalHeight * Scale);

    TArray<uint8> RegionMask;
    RegionMask.Empty(ScaledWidth * ScaledHeight);

    for (int32 y = 0; y < ScaledHeight; ++y)
    {
        for (int32 x = 0; x < ScaledWidth; ++x)
        {
            int32 CanvasX = x + XOffset;
            int32 CanvasY = y + YOffset;

            if (CanvasX >= 0 && CanvasX < 1024 && CanvasY >= 0 && CanvasY < 1024)
            {
                RegionMask.Add(BinaryMask[CanvasY * 1024 + CanvasX]);
            }
            else
            {
                RegionMask.Add(0);
            }
        }
    }

    // 缩放回原始尺寸（使用最近邻插值）
    FinalMask.Empty(OriginalWidth * OriginalHeight);
    FinalMask.AddZeroed(OriginalWidth * OriginalHeight);

    float InvScaleX = float(ScaledWidth) / float(OriginalWidth);
    float InvScaleY = float(ScaledHeight) / float(OriginalHeight);

    for (int32 y = 0; y < OriginalHeight; ++y)
    {
        for (int32 x = 0; x < OriginalWidth; ++x)
        {
            int32 SrcX = FMath::Min(FMath::RoundToInt(x * InvScaleX), ScaledWidth - 1);
            int32 SrcY = FMath::Min(FMath::RoundToInt(y * InvScaleY), ScaledHeight - 1);

            FinalMask[y * OriginalWidth + x] = RegionMask[SrcY * ScaledWidth + SrcX];
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Mask postprocessing completed: %dx%d -> %dx%d -> %dx%d"), 
           1024, 1024, ScaledWidth, ScaledHeight, OriginalWidth, OriginalHeight);

    return true;
}