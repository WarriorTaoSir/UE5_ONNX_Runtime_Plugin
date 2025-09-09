# Release Checklist

Use this checklist before publishing the plugin to GitHub or any distribution platform.

## 🔍 Pre-Release Verification

### Code Quality
- [ ] All code compiles without warnings
- [ ] No memory leaks detected
- [ ] All virtual functions properly overridden
- [ ] Blueprint nodes work correctly
- [ ] Hot reload functionality tested
- [ ] Exception handling in place for all ONNX operations

### Testing
- [ ] **Generic ONNX**: Basic inference works with sample models
- [ ] **SAM2**: Segmentation produces valid masks
- [ ] **Memory Management**: Components clean up properly on destruction
- [ ] **File Loading**: Models load from both assets and file paths
- [ ] **Error Handling**: Graceful failure with informative logs
- [ ] **Performance**: No significant frame drops during inference

### Documentation
- [ ] README.md is complete and accurate
- [ ] API Reference covers all public methods
- [ ] Quick Start guide tested by new user
- [ ] Examples run successfully
- [ ] Changelog updated with release notes
- [ ] License file present and correct

### Plugin Configuration
- [ ] `.uplugin` file has correct version number
- [ ] Engine version compatibility specified
- [ ] Platform support accurately listed
- [ ] Dependencies properly declared
- [ ] Module loading configuration correct

### File Structure
- [ ] All source files in correct directories
- [ ] ThirdParty libraries properly organized
- [ ] No development-only files included
- [ ] Examples are self-contained
- [ ] Documentation is well-structured

## 📁 Files to Include

### Essential Files
```
UE5OnnxRuntime/
├── ✅ README.md
├── ✅ LICENSE
├── ✅ CHANGELOG.md
├── ✅ CONTRIBUTING.md
├── ✅ .gitignore
├── ✅ UE5OnnxRuntime.uplugin
└── ✅ RELEASE_CHECKLIST.md (this file)
```

### Source Code
```
Source/cloth/
├── Public/
│   ├── ✅ OnnxComponent.h
│   ├── ✅ Sam2Component.h
│   ├── ✅ OnnxModelInstance.h
│   ├── ✅ Sam2ModelInstance.h
│   ├── ✅ GenericOnnxTestActor.h
│   ├── ✅ Sam2TestActor.h
│   ├── ✅ OnnxModelAsset.h
│   ├── ✅ ImageLoadHelper.h
│   └── ✅ Cloth.h
└── Private/
    ├── ✅ [Corresponding .cpp files]
    └── ✅ cloth.Build.cs
```

### Documentation
```
Docs/
├── ✅ API_Reference.md
├── ✅ Quick_Start.md
├── ✅ Advanced_Usage.md
└── ✅ Installation_Guide.md
```

### Examples
```
Examples/
├── ✅ README.md
├── ✅ Sam2Example.md
└── ✅ [Additional example files]
```

### Third-Party Libraries
```
ThirdParty/OnnxRuntime/
├── include/          # ONNX Runtime headers
├── lib/             # Libraries and DLLs
│   ├── ✅ onnxruntime.lib
│   ├── ✅ onnxruntime.dll
│   └── ✅ onnxruntime_providers_shared.dll
└── README.md        # ONNX Runtime license and info
```

## 🚫 Files to Exclude

### Development Files
- [ ] ❌ `.vs/` folder
- [ ] ❌ `Binaries/` folder (except essential DLLs)
- [ ] ❌ `Intermediate/` folder
- [ ] ❌ `*.sln`, `*.vcxproj` files
- [ ] ❌ `.claude/` settings
- [ ] ❌ Personal test assets

### Temporary Files
- [ ] ❌ `*.tmp`, `*.temp` files
- [ ] ❌ Log files (`*.log`)
- [ ] ❌ Debug databases (`*.pdb`, `*.idb`)
- [ ] ❌ Patch files (`*.patch_*`)

### Large Files
- [ ] ❌ Test models >100MB (provide download links instead)
- [ ] ❌ High-resolution test images
- [ ] ❌ Video files or large datasets

## 🏷️ Version Management

### Version Numbering
- [ ] **Major.Minor.Patch** format (e.g., 1.0.0)
- [ ] Increment **Major** for breaking changes
- [ ] Increment **Minor** for new features
- [ ] Increment **Patch** for bug fixes

### Version Update Locations
- [ ] `UE5OnnxRuntime.uplugin` → `"VersionName": "X.Y.Z"`
- [ ] `README.md` → Badge and compatibility info
- [ ] `CHANGELOG.md` → New release section
- [ ] Git tag → `vX.Y.Z`

## 🚀 GitHub Release Process

### Repository Setup
- [ ] Repository is public (or ready for publication)
- [ ] Default branch is `main` or `master`
- [ ] Branch protection rules configured
- [ ] Issues and Discussions enabled
- [ ] Topics/tags added for discoverability

### Release Creation
1. **Create Git Tag**
   ```bash
   git tag -a v1.0.0 -m "Release version 1.0.0"
   git push origin v1.0.0
   ```

2. **GitHub Release Page**
   - [ ] Release title: "UE5 ONNX Runtime Plugin v1.0.0"
   - [ ] Description from CHANGELOG.md
   - [ ] Attach plugin ZIP file
   - [ ] Mark as "Latest Release"
   - [ ] Include installation instructions

3. **Release Assets**
   - [ ] `UE5OnnxRuntime-v1.0.0.zip` (complete plugin)
   - [ ] `UE5OnnxRuntime-Source-v1.0.0.zip` (source only)
   - [ ] `ONNX-Models-SAM2.zip` (sample models, if size permits)

### Post-Release
- [ ] Update social media / community posts
- [ ] Monitor for initial user feedback
- [ ] Respond to issues promptly
- [ ] Plan next release cycle

## ✅ Quality Gates

### Performance Benchmarks
- [ ] **Generic ONNX**: <100ms inference for typical models
- [ ] **SAM2**: <500ms end-to-end segmentation
- [ ] **Memory**: <2GB peak usage for typical workflows
- [ ] **Startup**: Plugin loads in <5 seconds

### Compatibility Testing
- [ ] **UE5 5.4**: Core functionality verified
- [ ] **UE5 5.5**: Forward compatibility checked
- [ ] **Visual Studio 2022**: Clean compilation
- [ ] **Different Windows versions**: Tested on Win10/Win11

### User Experience
- [ ] **Installation**: Complete in <5 minutes
- [ ] **First Use**: Working example in <10 minutes
- [ ] **Documentation**: Questions answered without external help
- [ ] **Error Messages**: Clear and actionable

## 🆘 Rollback Plan

If critical issues are discovered post-release:

1. **Immediate Response**
   - [ ] Add warning to README
   - [ ] Create pinned issue with workaround
   - [ ] Prepare hotfix if needed

2. **Hotfix Release**
   - [ ] Create `hotfix/v1.0.1` branch
   - [ ] Apply minimal fix
   - [ ] Test thoroughly
   - [ ] Release v1.0.1

3. **Communication**
   - [ ] Update all documentation
   - [ ] Notify community
   - [ ] Learn from the issue

---

## ✅ Final Checklist

Before clicking "Publish Release":

- [ ] All items above completed
- [ ] Plugin tested in fresh UE5 project
- [ ] Documentation reviewed by another person
- [ ] Release notes proofread
- [ ] Backup created of current state
- [ ] Ready to provide user support

**Release Manager:** _________________  
**Date:** _________________  
**Version:** _________________