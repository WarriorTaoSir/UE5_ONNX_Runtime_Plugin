# API Reference

## 📚 Core Classes

### UONNXComponent

Base component class for generic ONNX model inference.

#### Public Methods

##### `virtual bool Initialize()`
Initializes the ONNX model from ModelAsset or ModelFilePath.

**Returns:** `true` if initialization succeeded, `false` otherwise.

**Blueprint:** Yes

---

##### `virtual bool RunInference(const TArray<float>& InputData, TArray<float>& OutputData)`
Runs inference on the loaded model with input data.

**Parameters:**
- `InputData` - Float array containing model input
- `OutputData` - Output array to store results

**Returns:** `true` if inference succeeded

**Blueprint:** Yes

---

##### `virtual bool IsInitialized() const`
Checks if the component has been successfully initialized.

**Returns:** `true` if ready for inference

**Blueprint:** Yes

---

##### `bool LoadModelFromFile(const FString& FilePath)`
Loads a model from the specified file path.

**Parameters:**
- `FilePath` - Path to the ONNX model file

**Returns:** `true` if loading succeeded

**Blueprint:** Yes

---

##### `TArray<FOnnxTensorInfo> GetModelInputInfo() const`
Gets information about model input tensors.

**Returns:** Array of tensor information structures

**Blueprint:** Yes

---

##### `TArray<FOnnxTensorInfo> GetModelOutputInfo() const`
Gets information about model output tensors.

**Returns:** Array of tensor information structures

**Blueprint:** Yes

---

##### `void Reset()`
Resets the component and cleans up resources.

**Blueprint:** Yes

#### Public Properties

##### `UOnnxModelAsset* ModelAsset`
ONNX model asset to use for inference.

**Category:** ONNX Model
**Blueprint:** ReadWrite

---

##### `FString ModelFilePath`
File path to ONNX model (used when ModelAsset is null).

**Category:** ONNX Model
**Blueprint:** ReadWrite

#### Protected Methods

##### `virtual bool InitializeModel()`
Override this method in derived classes for custom initialization logic.

**Returns:** `true` if initialization succeeded

---

## 🎨 Specialized Classes

### USam2Component : public UONNXComponent

Specialized component for SAM2 image segmentation.

#### Public Methods

##### `bool RunSam2Segmentation(const FSam2Input& Input, FSam2Output& Output)`
Runs SAM2 segmentation inference.

**Parameters:**
- `Input` - SAM2 input structure with image and prompts
- `Output` - SAM2 output structure with masks and scores

**Returns:** `true` if segmentation succeeded

**Blueprint:** Yes

---

##### `bool SetImageFromTexture(UTexture2D* Texture, FSam2Input& Sam2Input)`
Converts UTexture2D to SAM2 input format.

**Parameters:**
- `Texture` - Source texture
- `Sam2Input` - SAM2 input structure to populate

**Returns:** `true` if conversion succeeded

**Blueprint:** Yes

---

##### `void AddPromptPoint(FSam2Input& Sam2Input, FVector2D Point, bool bIsForeground = true)`
Adds a prompt point to the SAM2 input.

**Parameters:**
- `Sam2Input` - SAM2 input structure
- `Point` - Point coordinates (0.0-1.0 relative)
- `bIsForeground` - Whether point is foreground (true) or background (false)

**Blueprint:** Yes

---

##### `void ClearPromptPoints(FSam2Input& Sam2Input)`
Clears all prompt points from the input.

**Parameters:**
- `Sam2Input` - SAM2 input structure to clear

**Blueprint:** Yes

---

##### `UTexture2D* CreateMaskTexture(const FSam2Output& Output, int32 MaskIndex = 0)`
Creates a texture from the segmentation mask.

**Parameters:**
- `Output` - SAM2 output structure
- `MaskIndex` - Index of the mask to convert

**Returns:** Texture2D containing the mask

**Blueprint:** Yes

---

##### `UTexture2D* CreateOverlayTexture(UTexture2D* OriginalImage, const FSam2Input& Input, const FSam2Output& Output, int32 MaskIndex = 0)`
Creates an overlay texture combining the original image, mask, and prompt points.

**Parameters:**
- `OriginalImage` - Original source image
- `Input` - SAM2 input with prompt points
- `Output` - SAM2 output with masks
- `MaskIndex` - Index of mask to overlay

**Returns:** Texture2D with visualization overlay

**Blueprint:** Yes

#### Public Properties

##### `FString Sam2EncoderPath`
Path to the SAM2 encoder ONNX model.

**Default:** `"Content/Model/sam2_hiera_tiny_encoder.onnx"`
**Category:** SAM2 Settings
**Blueprint:** ReadWrite

---

##### `FString Sam2DecoderPath`
Path to the SAM2 decoder ONNX model.

**Default:** `"Content/Model/sam2_hiera_tiny_decoder.onnx"`
**Category:** SAM2 Settings
**Blueprint:** ReadWrite

---

## 📊 Data Structures

### FOnnxTensorInfo

Information about an ONNX tensor.

#### Properties

##### `FString Name`
Name of the tensor.

**Blueprint:** ReadWrite

##### `TArray<int32> Shape`
Shape of the tensor (e.g., [1, 3, 224, 224]).

**Blueprint:** ReadWrite

##### `FString DataType`
Data type of the tensor (currently supports "float32").

**Blueprint:** ReadWrite

---

### FSam2Input

Input structure for SAM2 segmentation.

#### Properties

##### `TArray<float> ImageData`
Image data in HWC format (Height-Width-Channel).

##### `int32 ImageWidth`
Width of the input image.

##### `int32 ImageHeight`
Height of the input image.

##### `TArray<FVector2D> PromptPoints`
Array of prompt point coordinates (0.0-1.0 relative).

##### `TArray<int32> PromptLabels`
Labels for prompt points (1 = foreground, 0 = background).

---

### FSam2Output

Output structure for SAM2 segmentation.

#### Properties

##### `TArray<float> MaskData`
Raw mask data from the model.

##### `int32 NumMasks`
Number of generated masks.

##### `TArray<float> IouScores`
Intersection over Union scores for each mask.

##### `int32 OriginalWidth`
Width of the original input image.

##### `int32 OriginalHeight`
Height of the original input image.

##### `float Scale`
Scale factor applied during preprocessing.

##### `float XOffset`
X offset applied during preprocessing.

##### `float YOffset`
Y offset applied during preprocessing.

#### Methods

##### `TArray<float> GetMask(int32 MaskIndex) const`
Gets the mask data for a specific mask index.

**Parameters:**
- `MaskIndex` - Index of the mask to retrieve

**Returns:** Float array containing mask values

---

## 🧪 Testing Classes

### AGenericOnnxTestActor : public AActor

Test actor for generic ONNX models.

#### Public Methods

##### `void RunGenericOnnxTest()`
Runs a test with the configured ONNX model.

**Blueprint:** Yes

##### `void GenerateRandomInputData()`
Generates random input data based on InputShape.

**Blueprint:** Yes

#### Public Properties

##### `UONNXComponent* OnnxComponent`
The ONNX component to test.

**Category:** ONNX Test
**Blueprint:** ReadOnly

##### `FString ModelFilePath`
Path to the model file to test.

**Category:** ONNX Test
**Blueprint:** ReadWrite
**Default:** `"Content/Model/your_model.onnx"`

##### `TArray<int32> InputShape`
Expected input shape for the model.

**Category:** ONNX Test
**Blueprint:** ReadWrite

##### `bool bAutoRunTest`
Whether to automatically run the test on BeginPlay.

**Category:** ONNX Test
**Blueprint:** ReadWrite
**Default:** `true`

---

### ASam2TestActor : public AActor

Test actor specifically for SAM2 models.

#### Public Methods

##### `void RunSAM2Test()`
Runs a complete SAM2 segmentation test.

**Blueprint:** Yes

##### `bool LoadTestImage()`
Loads the test image from the specified path.

**Blueprint:** Yes

##### `bool SaveMaskResult(UTexture2D* MaskTexture)`
Saves the mask result to a PNG file.

**Blueprint:** Yes

##### `bool SaveOverlayResult(UTexture2D* OverlayTexture)`
Saves the overlay result to a PNG file.

**Blueprint:** Yes

#### Public Properties

##### `USam2Component* Sam2Component`
The SAM2 component to test.

**Category:** SAM2 Test
**Blueprint:** ReadOnly

##### `UTexture2D* TestImage`
Test image asset to use for segmentation.

**Category:** SAM2 Test
**Blueprint:** ReadWrite

##### `FVector2D PromptPoint`
Prompt point coordinates for testing.

**Category:** SAM2 Test
**Blueprint:** ReadWrite
**Default:** `(0.5, 0.5)`

##### `bool bIsForegroundPoint`
Whether the prompt point is foreground.

**Category:** SAM2 Test
**Blueprint:** ReadWrite
**Default:** `true`

##### `bool bAutoRunTest`
Whether to automatically run the test on BeginPlay.

**Category:** SAM2 Test
**Blueprint:** ReadWrite
**Default:** `true`

---

## 🛠️ Usage Examples

### Basic ONNX Inference

```cpp
// Load and run a generic model
UONNXComponent* Component = CreateDefaultSubobject<UONNXComponent>(TEXT("ONNX"));
if (Component->LoadModelFromFile(TEXT("Content/Models/classifier.onnx")))
{
    TArray<float> Input = {1.0f, 2.0f, 3.0f};
    TArray<float> Output;
    Component->RunInference(Input, Output);
}
```

### SAM2 Segmentation

```cpp
// Run SAM2 segmentation
USam2Component* Sam2 = CreateDefaultSubobject<USam2Component>(TEXT("SAM2"));
FSam2Input Input;
FSam2Output Output;

Sam2->SetImageFromTexture(MyTexture, Input);
Sam2->AddPromptPoint(Input, FVector2D(0.5f, 0.5f), true);
Sam2->RunSam2Segmentation(Input, Output);

UTexture2D* Mask = Sam2->CreateMaskTexture(Output);
```

### Custom Model Integration

```cpp
// Create custom component
class MYPLUGIN_API UMyModelComponent : public UONNXComponent
{
protected:
    virtual bool InitializeModel() override
    {
        // Custom initialization logic
        return LoadMySpecificModel();
    }
};
```

## 📋 Error Codes

### Common Return Values
- `true` - Operation succeeded
- `false` - Operation failed (check logs for details)

### Log Categories
- `LogTemp` - General plugin messages
- `LogOnnx` - ONNX Runtime specific messages  
- `LogSam2` - SAM2 specific messages

## 🔧 Performance Notes

### Memory Usage
- Models are loaded into GPU memory when available
- Use `Reset()` to free memory when switching models
- Consider model quantization for large models

### Threading
- All inference operations are thread-safe
- Avoid concurrent inference on the same component
- Use separate components for parallel processing

### Optimization Tips
- Pre-allocate input/output arrays when possible
- Cache model instances for repeated use
- Use appropriate input resolutions for your use case