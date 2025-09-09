// GenericOnnxTestActor.h - 通用ONNX模型测试Actor

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OnnxComponent.h"
#include "GenericOnnxTestActor.generated.h"

/**
 * AGenericOnnxTestActor
 * 用于测试通用ONNX模型推理功能的Actor
 * 展示如何使用UONNXComponent加载和推理任意ONNX模型
 */
UCLASS()
class CLOTH_API AGenericOnnxTestActor : public AActor
{
    GENERATED_BODY()

public:
    AGenericOnnxTestActor();

protected:
    virtual void BeginPlay() override;

public:
    // 通用ONNX组件
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ONNX Test")
    UONNXComponent* OnnxComponent;

    // 模型文件路径
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ONNX Test")
    FString ModelFilePath = TEXT("Content/Model/your_model.onnx");

    // 测试输入数据
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ONNX Test")
    TArray<float> TestInputData;

    // 输入张量形状（例如：[1, 3, 224, 224] 表示batch=1, channels=3, height=224, width=224）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ONNX Test")
    TArray<int32> InputShape;

    // 是否在开始时自动运行测试
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ONNX Test")
    bool bAutoRunTest = true;

    // 运行通用ONNX推理测试
    UFUNCTION(BlueprintCallable, Category = "ONNX Test")
    void RunGenericOnnxTest();

    // 生成随机测试数据
    UFUNCTION(BlueprintCallable, Category = "ONNX Test")
    void GenerateRandomInputData();

private:
    // 测试是否已经运行
    bool bTestCompleted = false;
};