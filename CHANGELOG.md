# Changelog

All notable changes to the UE5 ONNX Runtime Plugin will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-01-09

### 🎉 Initial Release

#### Added
- **Generic ONNX Model Support**
  - `UONNXComponent` base class for any ONNX model
  - Blueprint and C++ integration
  - Dynamic model loading from file paths or assets
  - Automatic memory management

- **SAM2 Image Segmentation**  
  - `USam2Component` specialized for SAM2 models
  - Interactive point-based prompting
  - Real-time mask generation and visualization
  - Overlay textures with prompt point indicators

- **Testing Framework**
  - `AGenericOnnxTestActor` for model validation
  - `ASam2TestActor` for SAM2 functionality testing
  - Comprehensive logging and debugging tools

- **ONNX Runtime Integration**
  - ONNX Runtime 1.20.0 bundled
  - Windows x64 platform support
  - CPU and GPU execution providers
  - Optimized for UE5.4+

- **Extensible Architecture**
  - Clean separation between generic and specialized components
  - Virtual methods for easy model type extension
  - Blueprint-friendly interfaces
  - Memory-safe smart pointer usage

- **Developer Tools**
  - Comprehensive API documentation
  - Multiple usage examples
  - Interactive tutorials
  - Performance optimization guides

#### Technical Details
- **Supported UE5 Versions**: 5.4.0+
- **Platform Support**: Windows x64
- **ONNX Runtime Version**: 1.20.0
- **Memory Management**: TUniquePtr-based automatic cleanup
- **Thread Safety**: Safe for concurrent use with separate instances

#### Known Limitations
- Windows x64 only (Linux/Mac support planned)
- CPU inference primarily (GPU optimization in progress)
- SAM2-tiny models only (other variants planned)
- Single-threaded inference per component

### 📚 Documentation
- Complete API reference
- Quick start guide
- Advanced usage examples
- Contribution guidelines
- Performance optimization tips

### 🧪 Examples Included
- Basic ONNX model inference
- Interactive SAM2 segmentation
- Custom model integration patterns
- Blueprint workflow examples
- C++ implementation samples

---

## [Unreleased]

### Planned Features
- **Platform Expansion**
  - Linux x64 support
  - macOS support  
  - Mobile platform investigation

- **Performance Enhancements**
  - GPU acceleration optimization
  - Multi-threaded inference
  - Model quantization support
  - Async processing patterns

- **Model Support**
  - YOLO object detection component
  - ResNet classification component
  - StyleGAN image generation
  - Additional SAM2 model variants

- **Advanced Features**
  - Video processing pipeline
  - Real-time camera integration
  - Model ensemble support
  - Custom operator registration

### Known Issues
- Occasional memory spikes with large models (>1GB)
- Hot-reload compatibility with specialized components
- Blueprint compilation warnings in some UE5 versions

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for information on how to contribute to this changelog and the project.

## License

This project is licensed under the MIT License - see [LICENSE](LICENSE) for details.