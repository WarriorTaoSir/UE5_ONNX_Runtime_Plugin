// OnnxModelInstance.h

#pragma once

#include "CoreMinimal.h"

// 包含ONNX Runtime的实现头文件
#if PLATFORM_WINDOWS && PLATFORM_64BITS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif
#include "onnxruntime_cxx_api.h"
#if PLATFORM_WINDOWS && PLATFORM_64BITS
#include "Windows/HideWindowsPlatformTypes.h"
#endif
// Forward-declare our asset class
class UOnnxModelAsset;

/**
 * FOnnxModelInstance
 * 一个非UObject的C++类，用于封装ONNX Runtime会话。
 * 这是核心逻辑层，负责创建Ort::Session、管理其生命周期并执行推理。它由UOnnxModelAsset创建。
 */
class CLOTH_API FOnnxModelInstance
{
public:
	// 构造函数：从给定的资产创建实例。
	FOnnxModelInstance(UOnnxModelAsset* InModelAsset);

	// 析构函数：清理Ort::Session和Ort::Env。
	~FOnnxModelInstance();

	
	// 检查底层的Ort::Session是否已成功初始化。
	bool IsInitialized() const;

	// 对提供的输入数据运行推理。
	// 注意：为简单起见，此示例假定单个浮点张量输入/输出。
	// 在实际使用中，您需要将其扩展以使其更通用。
	bool Run(const TArray<float>& InputData, TArray<float>& OutputData);


private:
	
	// 禁用复制以防止TUniquePtr的所有权问题。
	FOnnxModelInstance(const FOnnxModelInstance&) = delete;
	FOnnxModelInstance& operator=(const FOnnxModelInstance&) = delete;
	
	// ONNX运行时环境。
	TUniquePtr<Ort::Env> env_{nullptr};

	// ONNX运行时会话，代表加载的模型。
	TUniquePtr<Ort::Session> session_{nullptr};

	// 从资产中缓存的模型元数据，以便快速访问。
	FString inputNodeName_;
	FString outputNodeName_;
	TArray<int64> inputNodeDims_;
	
	// 用于指示初始化是否成功的标志。
	bool bIsInitialized_ = false;
};