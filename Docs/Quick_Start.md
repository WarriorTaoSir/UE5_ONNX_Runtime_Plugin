# Quick Start Guide

Get up and running with the UE5 ONNX Runtime Plugin in minutes!

## 🚀 5-Minute Setup

### Step 1: Install the Plugin

1. **Download** the plugin from GitHub
2. **Copy** to your project's `Plugins/` folder:
   ```
   MyProject/
   └── Plugins/
       └── UE5OnnxRuntime/
   ```
3. **Enable** in UE5 Editor: `Edit → Plugins → Search "ONNX"`
4. **Restart** the editor when prompted

### Step 2: Test Generic ONNX (Optional)

Add a test actor to verify the plugin works:

1. In your level, **Add Actor** → **Generic Onnx Test Actor**
2. Set **Model File Path** to any `.onnx` file
3. **Play** the game and check the output logs

## 🎨 SAM2 Image Segmentation Tutorial

### Prerequisites
- Download SAM2 models from [Meta's repository](https://github.com/facebookresearch/sam2)
- Place models in `Content/Model/`:
  - `sam2_hiera_tiny_encoder.onnx`
  - `sam2_hiera_tiny_decoder.onnx`

### Blueprint Workflow (Recommended)

#### 1. Create SAM2 Actor

1. **Create new Blueprint** (inherit from Actor)
2. **Add Component** → **Sam2 Component**
3. **Compile and save**

#### 2. Set Up Image Input

```
Event BeginPlay
│
├─ Load Image (Texture2D)
│
└─ Sam2Component → Set Image From Texture
   ├─ Target: Sam2Component
   ├─ Texture: [Your Image]
   └─ Sam2Input: [Create variable]
```

#### 3. Add Prompt Points

```
Sam2Component → Add Prompt Point
├─ Target: Sam2Component  
├─ Sam2Input: [From previous step]
├─ Point: X=0.5, Y=0.5 (center of image)
└─ Is Foreground: True
```

#### 4. Run Segmentation

```
Sam2Component → Run Sam2 Segmentation
├─ Target: Sam2Component
├─ Input: [Sam2Input from above]
└─ Output: [Create Sam2Output variable]
```

#### 5. Create Visualization

```
Sam2Component → Create Overlay Texture
├─ Target: Sam2Component
├─ Original Image: [Your original texture]
├─ Input: [Sam2Input]
├─ Output: [Sam2Output]
└─ Mask Index: 0
```

### C++ Workflow (Advanced)

#### Header File (MyActor.h)
```cpp
#include "Sam2Component.h"

UCLASS()
class MYGAME_API AMyActor : public AActor
{
    GENERATED_BODY()

public:
    AMyActor();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    USam2Component* Sam2Component;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test")
    UTexture2D* TestImage;

    virtual void BeginPlay() override;

private:
    void RunSegmentation();
};
```

#### Implementation (MyActor.cpp)
```cpp
#include "MyActor.h"

AMyActor::AMyActor()
{
    PrimaryActorTick.bCanEverTick = false;
    
    Sam2Component = CreateDefaultSubobject<USam2Component>(TEXT("Sam2Component"));
}

void AMyActor::BeginPlay()
{
    Super::BeginPlay();
    
    if (TestImage)
    {
        RunSegmentation();
    }
}

void AMyActor::RunSegmentation()
{
    FSam2Input Input;
    FSam2Output Output;
    
    // Set image
    if (!Sam2Component->SetImageFromTexture(TestImage, Input))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to set image"));
        return;
    }
    
    // Add prompt point at center
    Sam2Component->AddPromptPoint(Input, FVector2D(0.5f, 0.5f), true);
    
    // Run segmentation
    if (Sam2Component->RunSam2Segmentation(Input, Output))
    {
        // Create mask texture
        UTexture2D* MaskTexture = Sam2Component->CreateMaskTexture(Output);
        
        // Create overlay
        UTexture2D* OverlayTexture = Sam2Component->CreateOverlayTexture(
            TestImage, Input, Output, 0
        );
        
        UE_LOG(LogTemp, Log, TEXT("Segmentation completed! Generated %d masks"), 
               Output.NumMasks);
    }
}
```

## 🧪 Testing Your Setup

### Quick Validation

1. **Check Logs** during play:
   ```
   LogTemp: SAM2 Component initialized successfully
   LogTemp: Segmentation completed! Generated 1 masks
   ```

2. **Expected Behavior**:
   - No error messages
   - Mask textures created successfully
   - Overlay shows segmented regions in green

### Common Issues & Solutions

| Issue | Solution |
|-------|----------|
| "Component not initialized" | Check model file paths exist |
| "Failed to load model" | Verify ONNX model compatibility |
| "Out of memory" | Use smaller images or quantized models |
| Black mask output | Check prompt points are within image bounds |

## 📁 Project Structure

After setup, your project should look like:

```
MyProject/
├── Content/
│   └── Model/                    # Your ONNX models
│       ├── sam2_encoder.onnx
│       └── sam2_decoder.onnx
├── Plugins/
│   └── UE5OnnxRuntime/          # Plugin files
│       ├── Source/
│       ├── ThirdParty/
│       └── Docs/
└── Source/                      # Your game code
    └── MyGame/
        ├── MyActor.h           # Your custom actors
        └── MyActor.cpp
```

## 🎯 Next Steps

### Learn More
- 📖 [Advanced Usage Guide](Advanced_Usage.md) - Comprehensive features
- 🔧 [API Reference](API_Reference.md) - Complete function documentation
- 🤝 [Contributing Guide](../CONTRIBUTING.md) - How to contribute

### Extend the Plugin
- **Add New Models**: Create custom components for YOLO, ResNet, etc.
- **Custom Processing**: Extend post-processing for your use case
- **Performance Tuning**: Optimize for your specific requirements

### Community
- ⭐ **Star** the repository if you find it useful
- 🐛 **Report issues** on GitHub
- 💡 **Share** your use cases and improvements

---

**Need help?** Check our [troubleshooting guide](Advanced_Usage.md#常见问题解答) or open an issue on GitHub!