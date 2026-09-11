# 🎛️ Modern JUCE Audio Plugin Boilerplate

> **A production-ready, crash-resilient JUCE audio plugin boilerplate for Visual Studio Code & CMake.**  
> Built to eliminate configuration headaches, avoid beginner pitfalls, and provide a fully working, thread-safe DSP starting point.

[![CI Build & Test](https://github.com/juce-framework/JUCE/actions/workflows/build.yml/badge.svg)](https://github.com/)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![JUCE](https://img.shields.io/badge/JUCE-8.0-orange.svg)
![Formats](https://img.shields.io/badge/Formats-VST3%20%7C%20AU%20%7C%20Standalone-green.svg)
![Platforms](https://img.shields.io/badge/Platforms-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey.svg)

---

## 🌟 Why This Exists

Getting started with audio plugin development is intimidating. Beginners often spend days battling:
- Cryptic CMake generator and kit errors.
- Unlinked JUCE modules or missing submodules.
- Thread-safety data races between the audio thread and UI thread.
- Audible "zipper noise" from unsmoothed parameters.
- Missing state persistence (losing all plugin settings whenever a DAW project is reloaded).
- Build failures from permission issues (`COPY_PLUGIN_AFTER_BUILD`).

**This boilerplate solves all of that out of the box.** Download it, open it in VS Code, press **F5**, and start coding your DSP algorithms immediately.

---

## ✨ Features

- ⚡ **Zero-Friction Setup**: Automatic JUCE dependency handling via CMake `FetchContent` with local submodule fallback. Works immediately even if downloaded as a ZIP!
- 🎚️ **Production-Ready Parameter System**: Fully implemented `AudioProcessorValueTreeState` (APVTS) for sample-accurate DAW automation and preset recall.
- 💾 **DAW State Persistence**: Robust XML state saving and loading in `getStateInformation()` and `setStateInformation()`.
- 🎛️ **Smoothed DSP**: Clean gain processing using `juce::dsp::Gain<float>` with ramped smoothing to eliminate zipper noise and clicking.
- 📊 **Real-Time Safe Metering**: Lock-free, atomic RMS output meters (`std::atomic<float>`) polled at 30 FPS by the UI thread without audio thread stalls.
- 🎨 **Modern Dark UI**: Resizable vector interface with dual stereo LED-style RMS level meters.
- 🍏 **True Cross-Platform**: Supports **VST3**, **Standalone**, and **AudioUnit (AU)** on macOS (Logic Pro / GarageBand compatible).
- 🛡️ **Safe Build Defaults**: No Administrator permission crashes—standalone binaries and plugin outputs stay in your project build directory.
- 🐞 **Pre-configured VS Code Debugging**: `.vscode/` includes instant F5 debugging for Standalone and DAW process attachment.
- 🚀 **GitHub Actions CI**: Automated multi-platform build workflow (Windows, macOS, Linux).

---

## 📁 Project Structure

```text
JUCE-Plugin-Boilerplate/
├── .github/
│   └── workflows/
│       └── build.yml             # Automated CI for Windows, macOS, Linux
├── .vscode/
│   ├── extensions.json           # Recommended VS Code extensions
│   ├── settings.json             # CMake & Intellisense configuration
│   └── launch.json               # One-click F5 debug configurations
├── Source/
│   ├── PluginProcessor.h         # Audio processor, APVTS, and DSP declarations
│   ├── PluginProcessor.cpp       # Real-time audio callback & state management
│   ├── PluginEditor.h            # UI components and timer declarations
│   └── PluginEditor.cpp          # Vector UI painting, controls & visual meters
├── assets/                       # (Optional) Drop PNG/WAV/SVG assets here
├── modules/
│   └── JUCE/                     # Local JUCE framework (or auto-fetched)
├── .gitignore                    # Comprehensive audio dev ignore rules
├── CMakeLists.txt                # Modern CMake configuration (C++20)
└── README.md                     # Documentation
```

---

## 🚀 Quick Start (VS Code)

### Prerequisites

Install the following on your system:
1. **[Visual Studio Code](https://code.visualstudio.com/)**
2. **[CMake](https://cmake.org/download/)** (version 3.23 or newer)
3. **C++ Compiler**:
   - **Windows**: Visual Studio 2022 Build Tools ("Desktop development with C++")
   - **macOS**: Xcode Command Line Tools (`xcode-select --install`)
   - **Linux**: `sudo apt install build-essential cmake libasound2-dev libjack-jackd2-dev libgl1-mesa-dev`
4. **VS Code Extensions**:
   - `C/C++` (`ms-vscode.cpptools`)
   - `CMake Tools` (`ms-vscode.cmake-tools`)

---

### Step 1 — Clone the Repository

```bash
git clone --recurse-submodules https://github.com/your-username/JUCE-Plugin-Boilerplate.git
cd JUCE-Plugin-Boilerplate
```
*(Even if you forget `--recurse-submodules`, CMake will automatically fetch JUCE for you!)*

---

### Step 2 — Open in Visual Studio Code

Open the project directory in VS Code:
```bash
code .
```

VS Code will automatically detect CMake and ask you to select a **Kit / Compiler** (e.g., *Visual Studio Community Release - amd64* or *Clang*).

---

### Step 3 — Build and Debug

- **To Build**: Press `F7` or click **Build** in the bottom status bar.
- **To Run & Debug**: Press `F5` to launch the **Standalone Application**.
- **To Test in a DAW**: The `.vst3` bundle will be generated inside:
  ```text
  build/SimpleGainPlugin_artefacts/Debug/VST3/
  ```

---

## 🛠️ How to Add Your Own Parameters

This boilerplate provides a clean pattern for adding automatable parameters using `apvts`:

### 1. Add the Parameter in `PluginProcessor.cpp`

Inside `createParameterLayout()`:
```cpp
// Add a Frequency parameter (20 Hz to 20,000 Hz with logarithmic skew)
params.push_back(std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID{"cutoff", 1},
    "Cutoff",
    juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f), // Skew factor 0.25 for audio log response
    1000.0f, // Default value
    juce::AudioParameterFloatAttributes().withLabel("Hz")
));
```

### 2. Read the Parameter in `processBlock()`

```cpp
const float cutoffHz = apvts.getRawParameterValue("cutoff")->load(std::memory_order_relaxed);
// Update your filter:
// filter.setCutoffFrequency(cutoffHz);
```

### 3. Connect a Slider in `PluginEditor`

In `PluginEditor.h`:
```cpp
juce::Slider cutoffSlider;
std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
```

In `PluginEditor.cpp`:
```cpp
cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
    processorRef.apvts, "cutoff", cutoffSlider);
addAndMakeVisible(cutoffSlider);
```

**That's it!** The parameter is now:
- Automatable in all major DAWs.
- Saved & restored automatically when saving the project.
- Thread-safe and lock-free on the audio thread.

---

## ⚡ Real-Time Audio Golden Rules

When programming audio inside `processBlock()`:

| ❌ NEVER DO THIS IN `processBlock` | ✅ DO THIS INSTEAD |
| :--- | :--- |
| `new`, `malloc`, or `std::vector::push_back` | Pre-allocate all buffers in `prepareToPlay()` |
| `std::mutex::lock()` or `CriticalSection` | Use `std::atomic<float>` or lock-free FIFOs (`juce::AbstractFifo`) |
| `std::cout`, file I/O, or socket calls | Push data to a background queue or timer |
| Raw sudden float multiplications | Use `juce::SmoothedValue` or `juce::dsp::Gain` |

---

## 🔧 Build Configuration Options

In `CMakeLists.txt`, customize the project to your liking:

| Option / Variable | Default | Description |
| :--- | :--- | :--- |
| `PROJECT_NAME` | `"SimpleGainPlugin"` | CMake target name |
| `PRODUCT_NAME` | `"Simple Gain Plugin"` | User-facing plugin name displayed in DAWs |
| `COMPANY_NAME` | `"Archie DSP"` | Manufacturer name |
| `BUNDLE_ID` | `com.archiedsp.SimpleGainPlugin` | Unique bundle identifier |
| `COPY_PLUGIN_TO_SYSTEM` | `OFF` | Set `ON` to auto-copy to system VST3/AU folder |
| `JUCE_WEB_BROWSER` | `0` | Keeps plugin lightweight and prevents Linux WebKit dependencies |

---

## ❓ Frequently Asked Questions & Troubleshooting

### Q: CMake says "No CMAKE_CXX_COMPILER could be found"
**Fix**: Install Visual Studio Build Tools (Windows) with the "Desktop Development with C++" workload checked, or run `xcode-select --install` (macOS).

### Q: My DAW doesn't detect the plugin
1. Verify whether your DAW scans 64-bit VST3 plugins.
2. In your DAW's plugin manager, add the folder:
   `<YourProject>/build/SimpleGainPlugin_artefacts/Debug/VST3`
3. If on macOS using Logic/GarageBand, ensure you built the **AU** target.

### Q: How do I debug inside my DAW?
Open `.vscode/launch.json` and choose **"Attach to DAW"**. Start your DAW, press `F5` in VS Code, and select your DAW process from the prompt. You can now set breakpoints directly inside `processBlock()`!

---

## 📄 License & Credits

- **Boilerplate**: Created by [Archie](https://github.com/). Free to use and modify for learning, academic, and commercial plugin development.
- **JUCE Framework**: JUCE is licensed under the AGPLv3 / Commercial license. See [JUCE License](https://juce.com/legal/juce-8-licence/) for details.
