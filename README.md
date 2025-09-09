# UE5 ONNX Runtime Plugin

[![UE Version](https://img.shields.io/badge/UE5-5.4%2B-blue)](https://www.unrealengine.com/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20x64-lightgrey)](https://www.microsoft.com/windows)
[![ONNX Runtime](https://img.shields.io/badge/ONNX%20Runtime-1.20.0-green)](https://onnxruntime.ai/)
[![License](https://img.shields.io/badge/License-MIT-orange)](LICENSE)

A comprehensive **ONNX Runtime integration plugin** for Unreal Engine 5, providing seamless AI model inference capabilities with an extensible architecture for custom ONNX models.

## ✨ Features

### 🎯 Core Capabilities
- **Generic ONNX Model Support** - Load and run any ONNX model
- **Blueprint & C++ Integration** - Full support for both Blueprint and C++ workflows
- **Extensible Architecture** - Easily add support for new model types
- **Memory Efficient** - Smart memory management with automatic cleanup
- **Thread Safe** - Designed for production use

### 🎨 Specialized Models
- **SAM2 Image Segmentation** - Ready-to-use SAM2 (Segment Anything Model 2) integration
- **Interactive Segmentation** - Point-based prompting with visual feedback
- **Real-time Processing** - Optimized for interactive applications

### 🛠️ Developer Tools
- **Testing Actors** - Built-in test actors for validation
- **Debug Logging** - Comprehensive logging for troubleshooting
- **Visual Helpers** - Overlay textures and mask visualization

## 🚀 Quick Start

### Prerequisites
- **Unreal Engine 5.4+**
- **Windows 64-bit**
- **Visual Studio 2022** (for C++ development)

### Installation

1. **Download the Plugin**
   ```bash
   git clone https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin.git
   ```

2. **Copy to Your Project**
   ```
   YourProject/
   └── Plugins/
       └── UE5OnnxRuntime/    # Copy plugin here
   ```

3. **Enable the Plugin**
   - Open your UE5 project
   - Go to `Edit → Plugins`
   - Search for "UE5 ONNX Runtime Plugin"
   - Enable the plugin and restart the editor

## 📖 Usage Guide

### Generic ONNX Models

#### Blueprint Usage
1. Add **ONNX Component** to your Actor
2. Set **Model File Path** (e.g., `Content/Models/resnet50.onnx`)
3. Call **Initialize** to load the model
4. Use **Run Inference** with input data

#### C++ Usage
```cpp
#include "OnnxComponent.h"

// In your Actor class
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
UONNXComponent* OnnxComponent;

// In constructor
OnnxComponent = CreateDefaultSubobject<UONNXComponent>(TEXT("OnnxComponent"));

// Load and run model
void AMyActor::RunInference()
{
    if (OnnxComponent->LoadModelFromFile(TEXT("Content/Models/my_model.onnx")))
    {
        TArray<float> InputData = {/* your input */};
        TArray<float> OutputData;
        
        if (OnnxComponent->RunInference(InputData, OutputData))
        {
            // Process results
            UE_LOG(LogTemp, Log, TEXT("Inference completed: %d outputs"), OutputData.Num());
        }
    }
}
```

### SAM2 Image Segmentation

#### Blueprint Workflow
1. Add **Sam2 Component** to your Actor
2. Set encoder/decoder model paths
3. Use **Set Image From Texture** to input image
4. Add prompt points with **Add Prompt Point**
5. Call **Run Sam2 Segmentation**
6. Create visualization with **Create Overlay Texture**

#### C++ Example
```cpp
#include "Sam2Component.h"

USam2Component* Sam2Component;

void AMyActor::RunSegmentation()
{
    FSam2Input Input;
    FSam2Output Output;
    
    // Set image and prompt
    Sam2Component->SetImageFromTexture(MyTexture, Input);
    Sam2Component->AddPromptPoint(Input, FVector2D(0.5f, 0.5f), true);
    
    // Run inference
    if (Sam2Component->RunSam2Segmentation(Input, Output))
    {
        // Create mask texture
        UTexture2D* Mask = Sam2Component->CreateMaskTexture(Output);
        // Use the mask...
    }
}
```

## 🔧 Extending the Plugin

### Adding New Model Types

Create a new component inheriting from `UONNXComponent`:

```cpp
// YoloComponent.h
UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class YOURPLUGIN_API UYoloComponent : public UONNXComponent
{
    GENERATED_BODY()

public:
    // YOLO-specific functions
    UFUNCTION(BlueprintCallable, Category = "YOLO Detection")
    bool RunObjectDetection(const FYoloInput& Input, FYoloOutput& Output);

protected:
    // Override initialization for YOLO-specific setup
    virtual bool InitializeModel() override;
};
```

### Directory Structure for New Models
```
Content/
└── Models/
    ├── Classification/
    │   └── resnet50.onnx
    ├── Detection/
    │   └── yolov8.onnx
    └── Segmentation/
        ├── sam2_encoder.onnx
        └── sam2_decoder.onnx
```

## 📁 Plugin Architecture

```
UE5OnnxRuntime/
├── Source/cloth/
│   ├── Public/
│   │   ├── OnnxComponent.h          # Generic ONNX base class
│   │   ├── Sam2Component.h          # SAM2 specialization
│   │   └── GenericOnnxTestActor.h   # Testing utilities
│   └── Private/
│       ├── OnnxComponent.cpp
│       ├── Sam2Component.cpp
│       └── OnnxModelInstance.cpp    # ONNX Runtime wrapper
├── ThirdParty/OnnxRuntime/          # ONNX Runtime 1.20.0
│   ├── include/                     # Headers
│   └── lib/                         # Libraries & DLLs
└── Docs/
    ├── API_Reference.md
    └── Advanced_Usage.md
```

## 🎮 Example Projects

### Included Examples
- **GenericOnnxTestActor** - Tests any ONNX model with random input
- **Sam2TestActor** - Complete SAM2 segmentation example
- **Sample Models** - Pre-configured test models

### Community Examples
See [Examples Repository](https://github.com/YourUsername/UE5-ONNX-Examples) for:
- Image Classification with ResNet
- Object Detection with YOLO
- Style Transfer models
- Custom model integrations

## 🐛 Troubleshooting

### Common Issues

**"Component not initialized"**
- Ensure model files exist in the specified path
- Check UE5 logs for detailed error messages
- Verify ONNX model compatibility

**Memory issues**
- Use `Reset()` to clean up models when switching
- Monitor memory usage with large models
- Consider model quantization for performance

**Build errors**
- Ensure Visual Studio 2022 is installed
- Check UE5 version compatibility (5.4+)
- Verify plugin dependencies in .uplugin file

### Getting Help
- Check [Documentation](Docs/)
- Browse [Issues](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin/issues)
- Join [Discord Community](#) 

## 🤝 Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development Setup
1. Fork the repository
2. Create feature branch
3. Make changes with tests
4. Submit pull request

## 📄 License

This project is licensed under the MIT License - see [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **ONNX Runtime Team** - For the excellent runtime library
- **Meta AI** - For the SAM2 model architecture  
- **Epic Games** - For Unreal Engine 5
- **Community Contributors** - For feedback and improvements

---

⭐ **Star this repo** if you find it helpful!

📢 **Follow for updates** on new features and model support!