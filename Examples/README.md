# Examples

This directory contains practical examples showing how to use the UE5 ONNX Runtime Plugin for various AI tasks.

## 📁 Available Examples

### 🎯 Basic Examples

#### [Generic ONNX Model](./GenericOnnxExample.md)
- Load any ONNX model
- Run inference with custom input
- Handle different tensor shapes
- **Use Case**: Image classification, regression models

#### [SAM2 Image Segmentation](./Sam2Example.md) 
- Interactive image segmentation
- Point-based prompting
- Mask visualization and overlay
- **Use Case**: Object selection, image editing

### 🚀 Advanced Examples

#### [Custom Model Integration](./CustomModelExample.md)
- Create specialized component classes
- Custom data structures
- Model-specific preprocessing
- **Use Case**: YOLO detection, custom architectures

#### [Performance Optimization](./PerformanceExample.md)
- Memory management best practices
- Async processing patterns
- GPU acceleration setup
- **Use Case**: Real-time applications

#### [Blueprint Integration](./BlueprintExample.md)
- Complete Blueprint workflows
- Custom Blueprint nodes
- Visual scripting patterns
- **Use Case**: Designer-friendly AI integration

## 🎮 Interactive Demos

### SAM2 Interactive Segmentation
```cpp
// Click anywhere in the viewport to segment that object
class MYGAME_API AInteractiveSegmentationActor : public AActor
{
    // Responds to mouse clicks and segments clicked objects
    // Shows real-time mask overlay
    // Supports multiple selection modes
};
```

### Real-time Object Detection
```cpp
// YOLO-based object detection on camera feed
class MYGAME_API AObjectDetectionActor : public AActor
{
    // Processes camera input in real-time
    // Draws bounding boxes around detected objects
    // Supports multiple object classes
};
```

### AI-Powered Image Filters
```cpp
// Style transfer and image enhancement
class MYGAME_API AImageFilterActor : public AActor
{
    // Applies neural style transfer
    // Real-time image enhancement
    // Multiple artistic styles
};
```

## 🛠️ How to Use Examples

### 1. Copy Example Code
Each example includes complete source code that you can copy into your project:
- Header files (`.h`)
- Implementation files (`.cpp`)
- Blueprint assets (when applicable)

### 2. Adapt to Your Needs
Examples are designed to be:
- **Modular** - Easy to integrate into existing projects
- **Configurable** - Adjustable parameters and settings
- **Extensible** - Build upon the provided foundation

### 3. Test and Iterate
All examples include:
- Test data and models
- Validation procedures
- Performance benchmarks
- Troubleshooting guides

## 📋 Example Template

### Creating Your Own Example

Follow this structure when contributing new examples:

```cpp
// ExampleComponent.h
#pragma once

#include "CoreMinimal.h"
#include "OnnxComponent.h"
#include "ExampleComponent.generated.h"

UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class MYPLUGIN_API UExampleComponent : public UONNXComponent
{
    GENERATED_BODY()

public:
    UExampleComponent();

    // Your custom inference function
    UFUNCTION(BlueprintCallable, Category = "Example")
    bool RunExampleInference(const FExampleInput& Input, FExampleOutput& Output);

protected:
    virtual bool InitializeModel() override;

private:
    // Your model-specific logic
    bool ProcessExampleData(const TArray<float>& Input, TArray<float>& Output);
};
```

### Documentation Template

Each example should include:

1. **Purpose** - What does this example demonstrate?
2. **Prerequisites** - Required models, data, or setup
3. **Usage** - Step-by-step instructions
4. **Code Explanation** - Key concepts and implementation details
5. **Customization** - How to adapt for different use cases
6. **Performance Notes** - Memory usage, speed considerations

## 🎯 Use Case Categories

### Image Processing
- **Classification** - Classify image contents
- **Segmentation** - Separate objects from background
- **Enhancement** - Improve image quality
- **Style Transfer** - Apply artistic styles

### Object Detection
- **Bounding Boxes** - Locate objects in images
- **Instance Segmentation** - Precise object boundaries
- **Tracking** - Follow objects across frames
- **3D Detection** - Spatial object recognition

### Natural Language
- **Text Classification** - Categorize text content
- **Sentiment Analysis** - Understand text emotion
- **Language Translation** - Convert between languages
- **Text Generation** - Create natural text

### Audio Processing
- **Speech Recognition** - Convert speech to text
- **Audio Classification** - Identify audio types
- **Noise Reduction** - Clean audio signals
- **Music Generation** - Create musical content

## 📚 Learning Path

### Beginner
1. Start with **Generic ONNX Example**
2. Try **SAM2 Image Segmentation**
3. Explore **Blueprint Integration**

### Intermediate
1. **Custom Model Integration**
2. **Performance Optimization**
3. Create your own specialized component

### Advanced
1. Multi-model pipelines
2. Custom ONNX operators
3. GPU optimization
4. Production deployment

## 🤝 Contributing Examples

We welcome community examples! To contribute:

1. **Fork** the repository
2. **Create** your example following the template
3. **Test** thoroughly across different scenarios
4. **Document** clearly with use cases and limitations
5. **Submit** a pull request

### Example Criteria
- **Practical** - Solves real-world problems
- **Well-documented** - Clear explanations and comments
- **Tested** - Works reliably across different setups
- **Maintainable** - Clean, readable code

## 🔗 External Resources

### Model Sources
- **ONNX Model Zoo** - [Pre-trained models](https://github.com/onnx/models)
- **Hugging Face** - [Transformer models](https://huggingface.co/models)
- **PyTorch Hub** - [Research models](https://pytorch.org/hub/)
- **TensorFlow Hub** - [Google models](https://tfhub.dev/)

### Conversion Tools
- **PyTorch** → ONNX: `torch.onnx.export()`
- **TensorFlow** → ONNX: `tf2onnx`
- **Keras** → ONNX: `keras2onnx`
- **Scikit-learn** → ONNX: `skl2onnx`

### Optimization Tools
- **ONNX Runtime** - Built-in optimizations
- **ONNX Simplifier** - Graph simplification
- **Quantization Tools** - Reduce model size
- **TensorRT** - NVIDIA GPU acceleration

---

🚀 **Ready to explore?** Pick an example that matches your use case and start building amazing AI-powered UE5 applications!