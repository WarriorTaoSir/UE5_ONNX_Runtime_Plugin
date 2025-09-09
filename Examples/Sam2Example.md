# SAM2 Image Segmentation Example

Complete example showing how to implement interactive image segmentation using the SAM2 (Segment Anything Model 2) component.

## 🎯 What You'll Learn

- Set up SAM2 for interactive segmentation
- Handle mouse input for point prompts
- Create real-time mask visualization
- Save segmentation results

## 📋 Prerequisites

### Models Required
Download SAM2-tiny models and place in `Content/Model/`:
- `sam2_hiera_tiny_encoder.onnx` 
- `sam2_hiera_tiny_decoder.onnx`

### Test Image
Any image file in `Content/Images/` (PNG, JPG supported)

## 🎮 Complete Implementation

### Header File (InteractiveSegmentationActor.h)

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Texture2D.h"
#include "Sam2Component.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "InteractiveSegmentationActor.generated.h"

UCLASS()
class MYGAME_API AInteractiveSegmentationActor : public AActor
{
    GENERATED_BODY()
    
public:    
    AInteractiveSegmentationActor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Input handling
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

public:
    // === Components ===
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ImageDisplay;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    USam2Component* Sam2Component;

    // === Configuration ===
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Segmentation")
    UTexture2D* SourceImage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Segmentation")
    UMaterial* DisplayMaterial;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Segmentation")
    bool bShowPromptPoints = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Segmentation")
    bool bAutoSegmentOnClick = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Segmentation")
    FLinearColor ForegroundPointColor = FLinearColor::Blue;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Segmentation")
    FLinearColor BackgroundPointColor = FLinearColor::Red;

    // === Public Interface ===
    
    UFUNCTION(BlueprintCallable, Category = "Segmentation")
    void LoadNewImage(UTexture2D* NewImage);
    
    UFUNCTION(BlueprintCallable, Category = "Segmentation")
    void AddPromptPoint(FVector2D ScreenPosition, bool bIsForeground = true);
    
    UFUNCTION(BlueprintCallable, Category = "Segmentation")
    void RunSegmentation();
    
    UFUNCTION(BlueprintCallable, Category = "Segmentation")
    void ClearAllPoints();
    
    UFUNCTION(BlueprintCallable, Category = "Segmentation")
    void SaveCurrentMask(const FString& OutputPath);

private:
    // === Internal State ===
    
    FSam2Input CurrentInput;
    FSam2Output LastOutput;
    
    UPROPERTY()
    UMaterialInstanceDynamic* DynamicMaterial;
    
    UPROPERTY()
    UTexture2D* CurrentOverlay;
    
    TArray<FVector2D> PromptPoints;
    TArray<bool> PromptLabels;
    
    bool bIsInitialized = false;
    bool bSegmentationInProgress = false;

    // === Helper Functions ===
    
    void InitializeDisplay();
    void UpdateDisplayTexture();
    FVector2D ScreenToImageCoordinates(FVector2D ScreenPos);
    void CreatePromptPointVisuals();
};
```

### Implementation File (InteractiveSegmentationActor.cpp)

```cpp
#include "InteractiveSegmentationActor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

AInteractiveSegmentationActor::AInteractiveSegmentationActor()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Create mesh component for image display
    ImageDisplay = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ImageDisplay"));
    RootComponent = ImageDisplay;
    
    // Set up default plane mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane"));
    if (PlaneMesh.Succeeded())
    {
        ImageDisplay->SetStaticMesh(PlaneMesh.Object);
    }
    
    // Create SAM2 component
    Sam2Component = CreateDefaultSubobject<USam2Component>(TEXT("Sam2Component"));
    
    // Enable input
    SetActorEnableCollision(true);
    ImageDisplay->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ImageDisplay->SetCollisionResponseToAllChannels(ECR_Block);
}

void AInteractiveSegmentationActor::BeginPlay()
{
    Super::BeginPlay();
    
    InitializeDisplay();
    
    if (SourceImage)
    {
        LoadNewImage(SourceImage);
    }
    
    // Enable input for this actor
    EnableInput(GetWorld()->GetFirstPlayerController());
}

void AInteractiveSegmentationActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Update any ongoing animations or effects here
}

void AInteractiveSegmentationActor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Bind input actions here if needed
}

void AInteractiveSegmentationActor::NotifyActorOnClicked(FKey ButtonPressed)
{
    if (ButtonPressed == EKeys::LeftMouseButton && bAutoSegmentOnClick)
    {
        // Get mouse position and convert to image coordinates
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            float MouseX, MouseY;
            PC->GetMousePosition(MouseX, MouseY);
            
            // Convert screen to image coordinates (this is simplified)
            FVector2D ImageCoords = ScreenToImageCoordinates(FVector2D(MouseX, MouseY));
            
            // Add prompt point
            bool bIsForeground = !ButtonPressed.IsShiftDown(); // Shift+Click for background
            AddPromptPoint(ImageCoords, bIsForeground);
            
            // Run segmentation
            RunSegmentation();
        }
    }
    
    Super::NotifyActorOnClicked(ButtonPressed);
}

void AInteractiveSegmentationActor::LoadNewImage(UTexture2D* NewImage)
{
    if (!NewImage || !Sam2Component)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot load image: null image or SAM2 component"));
        return;
    }
    
    SourceImage = NewImage;
    
    // Clear previous state
    ClearAllPoints();
    
    // Set up SAM2 input
    if (Sam2Component->SetImageFromTexture(SourceImage, CurrentInput))
    {
        bIsInitialized = true;
        UpdateDisplayTexture();
        UE_LOG(LogTemp, Log, TEXT("Loaded image: %dx%d"), 
               SourceImage->GetSizeX(), SourceImage->GetSizeY());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to set image for SAM2"));
        bIsInitialized = false;
    }
}

void AInteractiveSegmentationActor::AddPromptPoint(FVector2D ScreenPosition, bool bIsForeground)
{
    if (!bIsInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot add prompt: not initialized"));
        return;
    }
    
    // Convert screen position to normalized image coordinates (0-1)
    FVector2D ImageCoords = ScreenToImageCoordinates(ScreenPosition);
    
    // Clamp to valid range
    ImageCoords.X = FMath::Clamp(ImageCoords.X, 0.0f, 1.0f);
    ImageCoords.Y = FMath::Clamp(ImageCoords.Y, 0.0f, 1.0f);
    
    // Add to SAM2 input
    Sam2Component->AddPromptPoint(CurrentInput, ImageCoords, bIsForeground);
    
    // Store for visualization
    PromptPoints.Add(ImageCoords);
    PromptLabels.Add(bIsForeground);
    
    UE_LOG(LogTemp, Log, TEXT("Added %s point at (%.3f, %.3f)"), 
           bIsForeground ? TEXT("foreground") : TEXT("background"),
           ImageCoords.X, ImageCoords.Y);
    
    CreatePromptPointVisuals();
}

void AInteractiveSegmentationActor::RunSegmentation()
{
    if (!bIsInitialized || bSegmentationInProgress)
    {
        return;
    }
    
    if (PromptPoints.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No prompt points added"));
        return;
    }
    
    bSegmentationInProgress = true;
    
    UE_LOG(LogTemp, Log, TEXT("Running SAM2 segmentation with %d prompt points..."), 
           PromptPoints.Num());
    
    // Run SAM2 inference
    if (Sam2Component->RunSam2Segmentation(CurrentInput, LastOutput))
    {
        UE_LOG(LogTemp, Log, TEXT("Segmentation completed: %d masks generated"), 
               LastOutput.NumMasks);
        
        // Create overlay visualization
        CurrentOverlay = Sam2Component->CreateOverlayTexture(
            SourceImage, CurrentInput, LastOutput, 0
        );
        
        if (CurrentOverlay)
        {
            UpdateDisplayTexture();
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SAM2 segmentation failed"));
    }
    
    bSegmentationInProgress = false;
}

void AInteractiveSegmentationActor::ClearAllPoints()
{
    if (Sam2Component)
    {
        Sam2Component->ClearPromptPoints(CurrentInput);
    }
    
    PromptPoints.Empty();
    PromptLabels.Empty();
    
    // Reset display to original image
    if (SourceImage)
    {
        UpdateDisplayTexture();
    }
    
    UE_LOG(LogTemp, Log, TEXT("Cleared all prompt points"));
}

void AInteractiveSegmentationActor::SaveCurrentMask(const FString& OutputPath)
{
    if (!CurrentOverlay)
    {
        UE_LOG(LogTemp, Warning, TEXT("No mask to save"));
        return;
    }
    
    // This would require additional image saving functionality
    // For now, just log the request
    UE_LOG(LogTemp, Log, TEXT("Mask save requested to: %s"), *OutputPath);
    
    // You could implement PNG saving here using the ImageWrapper module
}

void AInteractiveSegmentationActor::InitializeDisplay()
{
    if (DisplayMaterial && ImageDisplay)
    {
        DynamicMaterial = UMaterialInstanceDynamic::Create(DisplayMaterial, this);
        ImageDisplay->SetMaterial(0, DynamicMaterial);
    }
}

void AInteractiveSegmentationActor::UpdateDisplayTexture()
{
    if (DynamicMaterial)
    {
        UTexture2D* TextureToShow = CurrentOverlay ? CurrentOverlay : SourceImage;
        if (TextureToShow)
        {
            DynamicMaterial->SetTextureParameterValue(TEXT("BaseTexture"), TextureToShow);
        }
    }
}

FVector2D AInteractiveSegmentationActor::ScreenToImageCoordinates(FVector2D ScreenPos)
{
    // This is a simplified conversion - you might need more sophisticated
    // world-to-local coordinate transformation based on your setup
    
    // For now, return a relative position
    FVector2D ViewportSize;
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
        
        return FVector2D(
            ScreenPos.X / ViewportSize.X,
            ScreenPos.Y / ViewportSize.Y
        );
    }
    
    return FVector2D(0.5f, 0.5f); // Default to center
}

void AInteractiveSegmentationActor::CreatePromptPointVisuals()
{
    // Here you could create visual indicators for prompt points
    // For example, spawn small sphere actors or draw debug points
    
    for (int32 i = 0; i < PromptPoints.Num(); ++i)
    {
        FVector2D Point = PromptPoints[i];
        bool bIsForeground = PromptLabels[i];
        
        FLinearColor PointColor = bIsForeground ? ForegroundPointColor : BackgroundPointColor;
        
        // Convert normalized coordinates to world position on the mesh
        // This is simplified - you'd need proper coordinate transformation
        FVector WorldPos = GetActorLocation() + FVector(
            (Point.X - 0.5f) * 100.0f,  // Scale and center
            (Point.Y - 0.5f) * 100.0f,
            10.0f  // Slightly above the mesh
        );
        
        // Draw debug point (you might want to create actual mesh components)
        DrawDebugSphere(GetWorld(), WorldPos, 2.0f, 8, PointColor.ToFColor(true), false, 5.0f);
    }
}
```

## 🎮 Blueprint Setup Alternative

For designers who prefer Blueprint workflow:

### 1. Create Blueprint Class
- Parent Class: **Actor**
- Add **Sam2 Component**
- Add **Static Mesh Component** (using Plane mesh)

### 2. Set Up Material
Create a material with:
- **Texture Sample** node connected to **Base Color**
- **Texture Parameter** named "BaseTexture"

### 3. Blueprint Graph
```
Event BeginPlay
│
├─ Create Dynamic Material Instance
│  ├─ Source Material: [Your image material]
│  └─ Store in variable: DynamicMaterial
│
├─ Set Material (Static Mesh)
│  ├─ Target: Static Mesh Component
│  └─ Material: DynamicMaterial
│
└─ Sam2Component → Set Image From Texture
   ├─ Texture: [Your test image]
   └─ Sam2Input: [Create variable]

Event ActorOnClicked
│
├─ Get Player Controller → Get Mouse Position
│
├─ Convert to Image Coordinates (Custom Function)
│
├─ Sam2Component → Add Prompt Point
│  ├─ Point: [Converted coordinates]
│  └─ Is Foreground: True
│
└─ Sam2Component → Run Sam2 Segmentation
   ├─ Input: Sam2Input
   ├─ Output: [Sam2Output variable]
   │
   └─ On Success:
       └─ Sam2Component → Create Overlay Texture
           ├─ Original Image: [Source texture]
           ├─ Input: Sam2Input  
           ├─ Output: Sam2Output
           └─ Set Texture Parameter Value
               ├─ Target: DynamicMaterial
               ├─ Parameter: "BaseTexture"
               └─ Value: [Overlay texture]
```

## 🎯 Usage Instructions

### 1. Setup
1. Place the actor in your level
2. Set **Source Image** to your test image
3. Assign a material with texture parameter

### 2. Interactive Segmentation
1. **Left Click** - Add foreground point and segment
2. **Shift+Left Click** - Add background point and segment  
3. **Multiple clicks** accumulate points for better results

### 3. Controls
- `ClearAllPoints()` - Reset to start over
- `LoadNewImage()` - Switch to different image
- `SaveCurrentMask()` - Export segmentation result

## 📊 Performance Tips

### Optimization
```cpp
// Cache frequently used textures
UPROPERTY()
TArray<UTexture2D*> CachedOverlays;

// Limit inference frequency
float LastInferenceTime = 0.0f;
const float MinInferenceInterval = 0.1f; // 100ms minimum between runs

bool ShouldRunInference()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    return (CurrentTime - LastInferenceTime) > MinInferenceInterval;
}
```

### Memory Management
```cpp
void AInteractiveSegmentationActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Clean up cached textures
    for (UTexture2D* Texture : CachedOverlays)
    {
        if (Texture && IsValid(Texture))
        {
            Texture->ConditionalBeginDestroy();
        }
    }
    CachedOverlays.Empty();
    
    Super::EndPlay(EndPlayReason);
}
```

## 🐛 Troubleshooting

### Common Issues

| Problem | Solution |
|---------|----------|
| No segmentation output | Check if SAM2 models are loaded correctly |
| Click detection not working | Verify collision settings on mesh component |
| Coordinates out of bounds | Implement proper screen-to-world conversion |
| Performance issues | Limit inference frequency, use smaller images |

### Debug Logging
```cpp
// Add comprehensive logging
UE_LOG(LogTemp, VeryVerbose, TEXT("Mouse at (%.1f, %.1f) -> Image (%.3f, %.3f)"), 
       MouseX, MouseY, ImageCoords.X, ImageCoords.Y);

UE_LOG(LogTemp, Log, TEXT("Segmentation: %d points, %d masks, %.3f IoU"), 
       CurrentInput.PromptPoints.Num(), LastOutput.NumMasks, 
       LastOutput.IouScores.Num() > 0 ? LastOutput.IouScores[0] : 0.0f);
```

## 🚀 Extensions

### Multi-Object Segmentation
- Track multiple objects with different colors
- Support object persistence across frames
- Implement object ID management

### Real-time Video
- Process camera input or video files
- Temporal consistency between frames
- Performance optimization for 30+ FPS

### Advanced Interaction
- Brush-based prompting instead of points
- Negative selection (subtract from mask)
- Mask editing and refinement tools

---

This example provides a complete foundation for interactive image segmentation in UE5. Customize it for your specific use case!