# Contributing to UE5 ONNX Runtime Plugin

Thank you for your interest in contributing! This document provides guidelines for contributing to the project.

## 🤝 How to Contribute

### Reporting Issues
- Use the [GitHub Issues](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin/issues) page
- Provide detailed reproduction steps
- Include UE5 version, OS, and plugin version
- Attach relevant logs and screenshots

### Feature Requests
- Open an issue with the "enhancement" label
- Describe the use case and expected behavior
- Consider providing a mockup or example code

## 🛠️ Development Workflow

### Setting Up Development Environment
1. Fork the repository
2. Clone your fork:
   ```bash
   git clone https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin.git
   ```
3. Create a new UE5 project and copy the plugin
4. Generate project files and build

### Making Changes
1. Create a feature branch:
   ```bash
   git checkout -b feature/your-feature-name
   ```
2. Make your changes
3. Test thoroughly
4. Commit with clear messages

### Code Style Guidelines
- Follow UE5 coding conventions
- Use descriptive variable and function names
- Add comments for complex logic
- Include Blueprint-callable functions where appropriate

### Testing
- Test with multiple UE5 versions when possible
- Verify both Blueprint and C++ workflows
- Include test cases for new model types
- Test memory management and cleanup

## 📝 Pull Request Process

### Before Submitting
- [ ] Code compiles without warnings
- [ ] All tests pass
- [ ] Documentation updated if needed
- [ ] No merge conflicts with main branch

### PR Description Template
```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
- [ ] Tested in UE5 Editor
- [ ] Tested with Blueprint
- [ ] Tested with C++
- [ ] Added/updated tests

## Screenshots/Videos
(If applicable)
```

### Review Process
1. Maintainer review (usually within 48 hours)
2. Address feedback if needed
3. Approval and merge

## 🎯 Areas for Contribution

### High Priority
- **New Model Types** - YOLO, ResNet, etc.
- **Platform Support** - Linux, Mac support
- **Performance Optimization** - GPU acceleration
- **Documentation** - Tutorials, examples

### Medium Priority
- **Blueprint Helpers** - Visual scripting aids
- **Model Validation** - ONNX compatibility checking
- **Memory Optimization** - Reduced footprint
- **Error Handling** - Better error messages

### Good First Issues
- **Example Projects** - Simple demo implementations
- **Documentation** - API reference improvements
- **Testing** - Unit test coverage
- **Code Cleanup** - Refactoring opportunities

## 🔧 Adding New Model Support

### 1. Create Component Class
```cpp
// YourModelComponent.h
UCLASS(ClassGroup = (AI), meta = (BlueprintSpawnableComponent))
class CLOTH_API UYourModelComponent : public UONNXComponent
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable, Category = "Your Model")
    bool RunYourModelInference(const FYourInput& Input, FYourOutput& Output);
    
protected:
    virtual bool InitializeModel() override;
};
```

### 2. Define Data Structures
```cpp
USTRUCT(BlueprintType)
struct CLOTH_API FYourInput
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    TArray<float> InputData;
    
    // Add your specific input fields
};
```

### 3. Add Test Actor
```cpp
UCLASS()
class CLOTH_API AYourModelTestActor : public AActor
{
    // Implementation for testing your model
};
```

### 4. Documentation
- Update README.md with usage examples
- Add to API documentation
- Include in example projects

## 📚 Documentation Standards

### Code Comments
- Document all public functions
- Explain complex algorithms
- Include usage examples in comments

### API Documentation
- Use Doxygen-style comments
- Include parameter descriptions
- Provide return value information

### User Documentation
- Step-by-step tutorials
- Common use cases
- Troubleshooting guides

## ❗ Important Notes

### Plugin Architecture
- Maintain separation between generic ONNX and specialized models
- Follow the established inheritance patterns
- Keep backward compatibility when possible

### Performance Considerations
- Profile memory usage with large models
- Optimize for real-time applications
- Consider async processing for heavy operations

### Platform Compatibility
- Test on target platforms
- Use platform-specific optimizations carefully
- Document platform limitations

## 🐛 Bug Reports

### Information to Include
- UE5 version and build
- Operating system
- Plugin version
- ONNX model details
- Full error messages and logs
- Minimal reproduction steps

### Example Bug Report
```markdown
**Environment:**
- UE5: 5.4.0
- OS: Windows 11 x64
- Plugin: v1.0.0

**Issue:**
Sam2Component crashes when loading large images

**Steps to Reproduce:**
1. Create Sam2Component
2. Load 4K texture
3. Call RunSam2Segmentation

**Expected:** Successful inference
**Actual:** Editor crash

**Logs:**
[Include relevant log output]
```

## 📞 Getting Help

### Community Channels
- GitHub Discussions for questions
- Issues for bug reports
- Pull Requests for contributions

### Response Times
- Issues: Within 48 hours
- Pull Requests: Within 1 week
- Security Issues: Within 24 hours

Thank you for contributing to the UE5 ONNX Runtime Plugin! 🎉