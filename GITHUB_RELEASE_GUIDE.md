# GitHub Release Guide

Complete step-by-step guide for publishing the UE5 ONNX Runtime Plugin to GitHub.

## 📋 Prerequisites

### 1. Plugin Ready for Release
- [ ] All development completed
- [ ] [RELEASE_CHECKLIST.md](RELEASE_CHECKLIST.md) fully completed
- [ ] Plugin tested and working
- [ ] Documentation finalized

### 2. GitHub Account Setup
- [ ] GitHub account with repository creation permissions
- [ ] Git installed locally
- [ ] SSH keys configured (recommended) or HTTPS authentication ready

### 3. Local Environment
- [ ] Plugin files organized in a clean directory
- [ ] Git repository initialized locally
- [ ] All unnecessary files removed (.gitignore applied)

## 🚀 Step-by-Step Release Process

### Phase 1: Repository Setup

#### 1.1 Create GitHub Repository
1. Go to [GitHub.com](https://github.com)
2. Click **"New"** or **"Create repository"**
3. Configure repository:
   ```
   Repository name: UE5-ONNX-Runtime-Plugin
   Description: Comprehensive ONNX Runtime integration for UE5 with SAM2 support
   ✅ Public repository
   ✅ Add a README file (uncheck - we have our own)
   ❌ Add .gitignore (we have our own)  
   ❌ Choose a license (we have MIT license)
   ```
4. Click **"Create repository"**

#### 1.2 Prepare Local Repository
```bash
# Navigate to your plugin directory
cd E:\Projects\UE5\Test\Plugins\cloth

# Initialize git repository
git init

# Add all files
git add .

# Initial commit
git commit -m "Initial commit: UE5 ONNX Runtime Plugin v1.0.0

- Generic ONNX model inference support
- SAM2 image segmentation specialized component  
- Comprehensive documentation and examples
- ONNX Runtime 1.20.0 integration
- UE5 5.4+ compatibility"

# Add remote origin (replace with your GitHub URL)
git remote add origin https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin.git

# Push to GitHub
git branch -M main
git push -u origin main
```

### Phase 2: Repository Configuration

#### 2.1 Set Up Repository Settings
1. Go to your repository on GitHub
2. Click **"Settings"** tab
3. Configure:

**General Settings:**
- ✅ Enable Wikis
- ✅ Enable Issues  
- ✅ Enable Discussions
- ❌ Enable Projects (optional)

**Features:**
- ✅ Preserve this repository  
- ✅ Include in the GitHub Archive Program

#### 2.2 Add Repository Topics/Tags
In repository main page, click **"⚙️ Settings"** near the description:
```
Topics: 
unreal-engine, ue5, onnx, machine-learning, ai, sam2, 
image-segmentation, plugin, cpp, blueprint, inference
```

#### 2.3 Create Branch Protection Rules (Optional)
1. Go to **Settings → Branches**
2. Add rule for `main` branch:
   - ✅ Require a pull request before merging
   - ✅ Require status checks to pass
   - ✅ Restrict pushes that create files over 100MB

### Phase 3: Documentation Enhancement

#### 3.1 Update README Links
Edit your README.md to replace placeholder URLs:
```markdown
# Change this:
[Documentation](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin)

# To actual URLs:
[Documentation](https://github.com/YourRealUsername/UE5-ONNX-Runtime-Plugin)
[Issues](https://github.com/YourRealUsername/UE5-ONNX-Runtime-Plugin/issues)
```

#### 3.2 Add Repository Links to .uplugin File
Update `UE5OnnxRuntime.uplugin`:
```json
{
    "DocsURL": "https://github.com/YourRealUsername/UE5-ONNX-Runtime-Plugin/blob/main/Docs",
    "SupportURL": "https://github.com/YourRealUsername/UE5-ONNX-Runtime-Plugin/issues"
}
```

Commit these changes:
```bash
git add .
git commit -m "Update repository URLs and documentation links"
git push
```

### Phase 4: Create First Release

#### 4.1 Create Version Tag
```bash
# Create and push version tag
git tag -a v1.0.0 -m "Release v1.0.0: Initial stable release

Features:
- Generic ONNX model inference
- SAM2 image segmentation  
- UE5 5.4+ compatibility
- Comprehensive documentation
- Example implementations"

git push origin v1.0.0
```

#### 4.2 Prepare Release Assets

**Create Plugin ZIP:**
```bash
# Create a clean copy of the plugin for distribution
cd E:\Projects\UE5\Test\Plugins
zip -r UE5OnnxRuntime-v1.0.0.zip cloth/ -x "cloth/Binaries/*" "cloth/Intermediate/*" "cloth/.vs/*"
```

**Create Source-Only ZIP:**  
```bash
# Source code only (no binaries)
zip -r UE5OnnxRuntime-Source-v1.0.0.zip cloth/Source/ cloth/Docs/ cloth/Examples/ cloth/*.md cloth/*.uplugin cloth/LICENSE
```

#### 4.3 Create GitHub Release
1. Go to your repository on GitHub
2. Click **"Releases"** (in right sidebar)
3. Click **"Create a new release"**
4. Fill in release form:

**Release Configuration:**
```
Tag version: v1.0.0
Target: main branch
Release title: UE5 ONNX Runtime Plugin v1.0.0
```

**Release Description:**
```markdown
# 🎉 Initial Release - UE5 ONNX Runtime Plugin v1.0.0

A comprehensive ONNX Runtime integration plugin for Unreal Engine 5, featuring generic ONNX model support and specialized SAM2 image segmentation capabilities.

## ✨ Key Features

### 🎯 Generic ONNX Support  
- Load and run any ONNX model in UE5
- Blueprint and C++ integration
- Memory-safe automatic cleanup
- Dynamic model switching

### 🎨 SAM2 Image Segmentation
- Interactive point-based segmentation
- Real-time mask generation
- Visual overlay with prompt indicators  
- Production-ready performance

### 🛠️ Developer-Friendly
- Extensible architecture for custom models
- Comprehensive documentation
- Interactive examples
- Performance optimization guides

## 📋 System Requirements

- **UE5:** 5.4.0 or later
- **Platform:** Windows x64
- **IDE:** Visual Studio 2022 (for C++ development)
- **ONNX Runtime:** 1.20.0 (included)

## 🚀 Quick Start

1. Download `UE5OnnxRuntime-v1.0.0.zip`
2. Extract to your project's `Plugins/` folder
3. Enable in UE5 Editor: `Edit → Plugins → Search "ONNX"`
4. Follow the [Quick Start Guide](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin/blob/main/Docs/Quick_Start.md)

## 📚 Documentation

- 📖 [API Reference](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin/blob/main/Docs/API_Reference.md)
- 🚀 [Quick Start Guide](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin/blob/main/Docs/Quick_Start.md)  
- 🔧 [Advanced Usage](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin/blob/main/Docs/Advanced_Usage.md)
- 💡 [Examples](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin/tree/main/Examples)

## 🧪 What's Included

### Core Components
- **UONNXComponent** - Generic ONNX inference base class
- **USam2Component** - SAM2 specialized segmentation component
- **Testing Actors** - Validation and example implementations

### Documentation
- Complete API reference
- Step-by-step tutorials
- Performance optimization guides
- Troubleshooting documentation

### Examples  
- Basic ONNX model inference
- Interactive SAM2 segmentation
- Custom model integration patterns
- Blueprint workflow examples

## 🔄 Upgrade Notes

This is the initial release. Future versions will maintain backward compatibility where possible.

## 🐛 Known Issues

- Windows x64 only (Linux/Mac support planned)
- Large models (>1GB) may cause memory spikes
- Hot-reload with specialized components needs editor restart

## 🤝 Contributing

We welcome contributions! See [CONTRIBUTING.md](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin/blob/main/CONTRIBUTING.md) for guidelines.

## 📄 License

MIT License - see [LICENSE](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin/blob/main/LICENSE)

---

⭐ **Star this repo** if you find it helpful!  
🐛 **Report issues** [here](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin/issues)  
💬 **Join discussions** [here](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin/discussions)
```

**Upload Assets:**
- Drag and drop `UE5OnnxRuntime-v1.0.0.zip`
- Drag and drop `UE5OnnxRuntime-Source-v1.0.0.zip`

**Final Settings:**
- ✅ Set as the latest release
- ✅ Create a discussion for this release

5. Click **"Publish release"**

### Phase 5: Post-Release Tasks

#### 5.1 Enable GitHub Features

**Enable Discussions:**
1. Go to **Settings → Features**
2. ✅ Enable Discussions
3. Create welcome post and categories

**Create Issue Templates:**
Create `.github/ISSUE_TEMPLATE/` directory with templates:
- `bug_report.md`
- `feature_request.md` 
- `question.md`

#### 5.2 Add Repository Shields/Badges

Update README.md with dynamic badges:
```markdown
[![GitHub release](https://img.shields.io/github/v/release/YourUsername/UE5-ONNX-Runtime-Plugin)](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin/releases)
[![GitHub stars](https://img.shields.io/github/stars/YourUsername/UE5-ONNX-Runtime-Plugin)](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin/stargazers)
[![GitHub issues](https://img.shields.io/github/issues/YourUsername/UE5-ONNX-Runtime-Plugin)](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin/issues)
[![GitHub license](https://img.shields.io/github/license/YourUsername/UE5-ONNX-Runtime-Plugin)](https://github.com/YourUsername/UE5-ONNX-Runtime-Plugin/blob/main/LICENSE)
```

#### 5.3 Community Outreach

**Share Your Release:**
- UE5 community forums
- Reddit r/unrealengine
- Discord servers
- Twitter/X with hashtags: #UE5 #ONNX #AI #GameDev

**Monitor and Respond:**
- Watch for GitHub notifications
- Respond to issues within 24-48 hours
- Thank users for stars and feedback
- Plan next features based on user requests

## 🎯 Success Metrics

After release, monitor:
- ⭐ **Stars**: Community interest indicator
- 📥 **Downloads**: Actual usage
- 🐛 **Issues**: User pain points  
- 💬 **Discussions**: Community engagement
- 🍴 **Forks**: Developer adoption

## 🔄 Future Releases

For subsequent releases:
1. Update version in all files
2. Add to CHANGELOG.md
3. Create new tag and release
4. Announce updates to community

---

**🎉 Congratulations! Your plugin is now live on GitHub!**

Need help? Check our [troubleshooting guide](RELEASE_CHECKLIST.md) or create an issue.