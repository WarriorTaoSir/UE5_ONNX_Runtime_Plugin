# ONNX Runtime Libraries

This directory contains the essential ONNX Runtime libraries for the UE5 plugin.

## Included Files

### ✅ Core Libraries (Included)
- `onnxruntime.dll` (12MB) - Main ONNX Runtime library
- `onnxruntime.lib` - Import library for linking
- `onnxruntime_providers_shared.dll` (22KB) - Shared providers

### 🔽 Optional GPU Acceleration (Download Separately)

For GPU acceleration, you can download these optional providers:

#### CUDA Provider (642MB)
- **File**: `onnxruntime_providers_cuda.dll`
- **Purpose**: NVIDIA CUDA GPU acceleration
- **Download**: [ONNX Runtime Releases](https://github.com/microsoft/onnxruntime/releases/tag/v1.20.0)
- **Requirements**: NVIDIA GPU with CUDA 11.8+ support

#### TensorRT Provider (742KB)  
- **File**: `onnxruntime_providers_tensorrt.dll`
- **Purpose**: NVIDIA TensorRT optimized inference
- **Download**: [ONNX Runtime Releases](https://github.com/microsoft/onnxruntime/releases/tag/v1.20.0)
- **Requirements**: NVIDIA GPU with TensorRT 8.5+ installed

## Installation Instructions

### For CPU-Only Usage (Default)
No additional setup required. The plugin works out of the box with CPU inference.

### For GPU Acceleration
1. Download the ONNX Runtime 1.20.0 release for Windows x64
2. Extract the GPU provider DLLs
3. Copy them to this directory:
   ```
   ThirdParty/OnnxRuntime/lib/
   ├── onnxruntime_providers_cuda.dll     (optional)
   └── onnxruntime_providers_tensorrt.dll (optional)
   ```
4. The plugin will automatically detect and use GPU providers if available

## Performance Notes

- **CPU Mode**: Works on all Windows systems, moderate performance
- **CUDA Mode**: 2-10x faster inference on NVIDIA GPUs
- **TensorRT Mode**: Maximum performance with model optimization

## Troubleshooting

### "Failed to load CUDA provider"
- Ensure NVIDIA GPU drivers are up to date
- Verify CUDA 11.8+ is installed
- Check that `onnxruntime_providers_cuda.dll` is in this directory

### "TensorRT provider not found"
- Install NVIDIA TensorRT 8.5+
- Ensure `onnxruntime_providers_tensorrt.dll` is present
- Verify GPU supports TensorRT

## Version Compatibility

- **ONNX Runtime**: 1.20.0
- **CUDA**: 11.8 or later
- **TensorRT**: 8.5 or later
- **Windows**: 10/11 x64

For more information, see the [ONNX Runtime documentation](https://onnxruntime.ai/docs/).