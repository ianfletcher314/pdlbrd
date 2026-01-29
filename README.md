# PDLBRD

**PDLBRD** (Pedalboard) is a professional-grade, native AU/VST3 audio plugin for macOS that brings the full Compstortion web app experience to your DAW. Built with the JUCE framework, PDLBRD delivers six studio-quality effects in a gorgeous, skeuomorphic interface inspired by real guitar pedals.

---

## Table of Contents

1. [Overview](#overview)
2. [Features](#features)
3. [Effects & Parameters](#effects--parameters)
4. [System Requirements](#system-requirements)
5. [Building from Source](#building-from-source)
6. [Installation](#installation)
7. [Usage Guide](#usage-guide)
8. [Preset System](#preset-system)
9. [Signal Chain & Reordering](#signal-chain--reordering)
10. [Project Structure](#project-structure)
11. [Technical Details](#technical-details)
12. [Troubleshooting](#troubleshooting)
13. [License](#license)
14. [Credits](#credits)

---

## Overview

PDLBRD is the native desktop version of [Compstortion](https://github.com/ianfletcher314/compstortion), a web-based guitar effects processor. While the web version runs in a browser using the Web Audio API, PDLBRD is a compiled Audio Unit (AU) and VST3 plugin that runs natively in any compatible DAW (Logic Pro, Ableton Live, GarageBand, Reaper, etc.).

### Why Native?

- **Lower latency**: Direct hardware access without browser overhead
- **Better CPU efficiency**: Optimized C++ code
- **DAW integration**: Full automation support, preset management, and seamless workflow
- **Professional features**: Sample-accurate processing, proper gain staging, and studio-grade DSP

---

## Features

### Six Professional Effects

1. **Compressor (x2)** - Two independent compressors for parallel compression, sidechain-style effects, or serial compression
2. **Distortion** - Five classic distortion algorithms (TS9, RAT, Blues, Fuzz, Muff)
3. **Amp Simulator** - Five amp voicings with full 3-band EQ and parametric mid
4. **Modulation** - Five modulation types (Phaser, Flanger, Chorus, Tremolo, Vibrato)
5. **Reverb** - Three reverb algorithms (Spring, Plate, Hall)

### Stunning Visual Design

- **Skeuomorphic pedal design** with brushed metal textures
- **Earth-tone color scheme** matching classic guitar pedals
- **Realistic knobs** with knurled edges and smooth rotation (7 o'clock to 5 o'clock sweep)
- **Hex bolt screws** in pedal corners for authentic look
- **Stomp-style footswitch buttons** with 3D press animation
- **Segmented LED gain reduction meters** on compressors (green/yellow/red)
- **Real-time level meters** for input and output monitoring
- **Vertical pedal titles** for compact, readable layout

### Advanced Functionality

- **Drag-and-drop signal chain reordering** - Rearrange effects in any order
- **Scrollable interface** - All pedals accessible in a compact window
- **Preset system** - Save, load, and manage your favorite settings
- **Full DAW automation** - Every parameter is automatable
- **Real-time metering** - Input/output levels and compressor gain reduction

---

## Effects & Parameters

### Compressor (Both Compressor 1 and Compressor 2)

The compressors use a feedforward design with smooth envelope detection and parallel compression capability.

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| **THRESH** | -60 to 0 dB | -20 dB (Comp1), -12 dB (Comp2) | Threshold level where compression begins |
| **RATIO** | 1:1 to 20:1 | 4:1 (Comp1), 2:1 (Comp2) | Compression ratio (higher = more compression) |
| **ATTACK** | 0.1 to 100 ms | 10 ms (Comp1), 20 ms (Comp2) | How quickly compression engages |
| **RELEASE** | 10 to 1000 ms | 100 ms (Comp1), 200 ms (Comp2) | How quickly compression releases |
| **MAKEUP** | 0 to 24 dB | 0 dB | Gain compensation after compression |
| **BLEND** | 0 to 100% | 100% | Parallel compression mix (dry/wet) |

**Gain Reduction Meter**: 12-segment LED display showing real-time compression amount
- Green (6 segments): 0-6 dB reduction
- Yellow (3 segments): 6-12 dB reduction
- Red (3 segments): 12-18 dB reduction

---

### Distortion

Five classic distortion circuits, each with its own character.

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| **DRIVE** | 0 to 100% | 50% | Amount of distortion/saturation |
| **TONE** | 0 to 100% | 50% | Brightness control (low-pass filter) |
| **LEVEL** | 0 to 100% | 50% | Output volume |
| **Type** | TS9, RAT, Blues, Fuzz, Muff | TS9 | Distortion algorithm |

**Distortion Types:**
- **TS9**: Tube Screamer-style smooth overdrive with mid hump
- **RAT**: Aggressive, gritty distortion with tight low end
- **Blues**: Warm, amp-like breakup for blues and rock
- **Fuzz**: Vintage fuzz with sustain and harmonic richness
- **Muff**: Big, creamy sustain with scooped mids

---

### Amp Simulator

Full amp modeling with 3-band EQ and selectable voicings.

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| **GAIN** | 0 to 100% | 30% | Preamp gain/drive |
| **BASS** | 0 to 100% | 50% | Low frequency EQ |
| **MID** | 0 to 100% | 50% | Mid frequency EQ |
| **MID FRQ** | 0 to 100% | 50% | Parametric mid frequency sweep |
| **TREBLE** | 0 to 100% | 50% | High frequency EQ |
| **MASTER** | 0 to 100% | 70% | Output volume |
| **Type** | Clean, Crunch, Lead, Modern, Vintage | Clean | Amp voicing |

**Amp Types:**
- **Clean**: Pristine clean tone with headroom
- **Crunch**: Edge-of-breakup tone, touch-sensitive
- **Lead**: Saturated lead tone with sustain
- **Modern**: High-gain modern metal voicing
- **Vintage**: Classic rock amp character

---

### Modulation

Five classic modulation effects with intuitive controls.

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| **RATE** | 0 to 100% | 50% | Modulation speed (LFO rate) |
| **DEPTH** | 0 to 100% | 50% | Modulation intensity |
| **BLEND** | 0 to 100% | 50% | Dry/wet mix |
| **Type** | Phaser, Flanger, Chorus, Tremolo, Vibrato | Chorus | Modulation algorithm |

**Modulation Types:**
- **Phaser**: Classic phase shifting with sweeping notches
- **Flanger**: Jet-like swooshing effect
- **Chorus**: Lush doubling and widening
- **Tremolo**: Volume modulation (amplitude tremolo)
- **Vibrato**: Pitch modulation for vintage character

---

### Reverb

Three reverb algorithms from subtle ambience to massive spaces.

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| **DECAY** | 0 to 100% | 50% | Reverb tail length |
| **TONE** | 0 to 100% | 50% | Reverb brightness |
| **BLEND** | 0 to 100% | 30% | Dry/wet mix |
| **Type** | Spring, Plate, Hall | Spring | Reverb algorithm |

**Reverb Types:**
- **Spring**: Classic guitar amp spring reverb with "drip"
- **Plate**: Smooth, dense plate reverb for vocals and instruments
- **Hall**: Large space reverb for epic sounds

---

## System Requirements

### Minimum Requirements

- **Operating System**: macOS 10.13 (High Sierra) or later
- **Processor**: Intel or Apple Silicon (Universal Binary)
- **RAM**: 4 GB minimum
- **Disk Space**: ~50 MB for the plugin

### Build Requirements

- **Xcode**: Version 12.0 or later (with Command Line Tools)
- **JUCE Framework**: Version 7.x or later
- **macOS SDK**: 10.13 or later

### Supported Plugin Formats

- **Audio Unit (AU)**: For Logic Pro, GarageBand, and other AU hosts
- **VST3**: For Ableton Live, Reaper, Cubase, and other VST3 hosts

---

## Building from Source

### Step 1: Install Prerequisites

#### Install Xcode

1. Open the **App Store** on your Mac
2. Search for **Xcode** and install it
3. Open Xcode once to accept the license agreement
4. Install Command Line Tools:
   ```bash
   xcode-select --install
   ```

#### Install JUCE

1. Download JUCE from https://juce.com/get-juce/download
2. Extract the downloaded archive
3. Move the JUCE folder to a permanent location (e.g., `~/JUCE` or `/Users/YourName/JUCE`)
4. Note the path - you'll need it for building

### Step 2: Clone the Repository

```bash
git clone https://github.com/ianfletcher314/pdlbrd.git
cd pdlbrd
```

### Step 3: Configure JUCE Path (if needed)

If your JUCE installation is not at `~/JUCE`, you'll need to update the project:

1. Open `PDLBRD.jucer` in the Projucer application (found in JUCE/extras/Projucer)
2. Update the JUCE modules path to your JUCE location
3. Save the project (this regenerates the Xcode project)

### Step 4: Build with Xcode

#### Option A: Command Line Build (Recommended)

```bash
# Navigate to the project directory
cd /path/to/pdlbrd

# Build the Audio Unit (Release configuration)
xcodebuild -project Builds/MacOSX/PDLBRD.xcodeproj \
           -configuration Release \
           -target "PDLBRD - AU"

# Build the VST3 (Release configuration)
xcodebuild -project Builds/MacOSX/PDLBRD.xcodeproj \
           -configuration Release \
           -target "PDLBRD - VST3"
```

#### Option B: Build with Xcode GUI

1. Open `Builds/MacOSX/PDLBRD.xcodeproj` in Xcode
2. Select the scheme you want to build:
   - `PDLBRD - AU` for Audio Unit
   - `PDLBRD - VST3` for VST3
3. Select **Product > Build** (or press `Cmd+B`)
4. Wait for the build to complete

### Step 5: Verify the Build

After a successful build, the plugin will be automatically copied to:

- **Audio Unit**: `~/Library/Audio/Plug-Ins/Components/PDLBRD.component`
- **VST3**: `~/Library/Audio/Plug-Ins/VST3/PDLBRD.vst3`

---

## Installation

### Automatic Installation (via Build)

The build process automatically installs the plugin to the correct location. Simply build the project and the plugin will be ready to use.

### Manual Installation

If you have pre-built binaries:

1. **Audio Unit (.component)**:
   ```bash
   cp -R PDLBRD.component ~/Library/Audio/Plug-Ins/Components/
   ```

2. **VST3 (.vst3)**:
   ```bash
   cp -R PDLBRD.vst3 ~/Library/Audio/Plug-Ins/VST3/
   ```

### Verifying Installation

#### For Audio Units:

```bash
# List all installed Audio Units
auval -a | grep PDLBRD

# Validate the Audio Unit
auval -v aufx PDLB Comp
```

#### For VST3:

Check that the file exists:
```bash
ls -la ~/Library/Audio/Plug-Ins/VST3/PDLBRD.vst3
```

### DAW-Specific Notes

#### Logic Pro
1. Open Logic Pro
2. Go to **Logic Pro > Settings > Plug-in Manager**
3. Click **Reset & Rescan Selection** if PDLBRD doesn't appear
4. Find PDLBRD under **Audio Units > Compstortion**

#### Ableton Live
1. Open Ableton Live
2. Go to **Preferences > Plug-ins**
3. Ensure "Use VST3 Plug-in System Folder" is enabled
4. Click "Rescan"
5. Find PDLBRD in the VST3 browser

#### GarageBand
1. Open GarageBand
2. PDLBRD should appear automatically in the Audio Unit effects list
3. If not, restart GarageBand

---

## Usage Guide

### Basic Operation

1. **Insert PDLBRD** on an audio track in your DAW
2. **Enable effects** by clicking their footswitch buttons
3. **Adjust parameters** by clicking and dragging the knobs
4. **Monitor levels** using the IN/OUT meters in the header
5. **Save presets** using the preset controls

### Knob Operation

- **Click and drag** vertically to adjust values
- **Scroll** over a knob for fine adjustment (if supported by your DAW)
- **Double-click** to reset to default value (DAW-dependent)

### Footswitch Operation

- **Click** the footswitch to toggle the effect on/off
- **LED indicator** (or gain reduction meter for compressors) shows active state
- When off, the effect is completely bypassed (no CPU usage for that effect)

### Level Metering

- **IN meter**: Shows input signal level before any processing
- **OUT meter**: Shows output signal level after all processing
- Color coding:
  - Green: Safe levels (-∞ to -6 dB)
  - Yellow: Moderate levels (-6 to -3 dB)
  - Red: Hot levels (above -3 dB)

---

## Preset System

### Preset Location

Presets are stored as `.pdlbrd` files in:
```
~/Library/Audio/Presets/PDLBRD/
```

### Saving a Preset

1. Configure all effects to your liking
2. Click the **Save** button in the header
3. Enter a name for your preset
4. Click Save in the dialog

### Loading a Preset

#### From the Dropdown:
1. Click the preset dropdown in the header
2. Select a preset from the list
3. All settings will be instantly applied

#### From a File:
1. Click the **Load** button in the header
2. Navigate to your preset file
3. Select the `.pdlbrd` file and click Open

### Preset File Format

Presets are stored as JSON files containing:
- All parameter values for all effects
- Effect bypass states
- Signal chain order

Example preset structure:
```json
{
  "comp1Threshold": -20.0,
  "comp1Ratio": 4.0,
  "comp1Attack": 10.0,
  "comp1Release": 100.0,
  "comp1Makeup": 0.0,
  "comp1Blend": 100.0,
  "comp1Bypass": true,
  ...
  "effectOrder": [0, 1, 2, 3, 4, 5]
}
```

---

## Signal Chain & Reordering

### Default Signal Chain

By default, the signal flows through effects in this order:
1. Compressor 1
2. Distortion
3. Amp Simulator
4. Modulation
5. Reverb
6. Compressor 2

### Reordering Effects

PDLBRD allows you to reorder effects via drag-and-drop:

1. **Click and hold** on any pedal (not on a knob or control)
2. **Drag** the pedal to a new position
3. **Drop** it on another pedal to swap positions
4. The signal chain updates immediately

### Common Signal Chain Configurations

#### Classic Guitar Rig
1. Compressor 1 (light compression for sustain)
2. Distortion
3. Amp Simulator
4. Modulation
5. Reverb
6. Compressor 2 (off or light limiting)

#### Modern High-Gain
1. Compressor 1 (tight compression)
2. Distortion (boost/overdrive into amp)
3. Amp Simulator (high gain)
4. Compressor 2 (limiting)
5. Modulation (subtle chorus)
6. Reverb (short room)

#### Ambient/Post-Rock
1. Compressor 1 (sustain)
2. Amp Simulator (clean)
3. Modulation (lush chorus/vibrato)
4. Distortion (light drive)
5. Reverb (large hall, high mix)
6. Compressor 2 (glue compression)

---

## Project Structure

```
pdlbrd/
├── README.md                    # This file
├── PDLBRD.jucer                 # JUCE project configuration
├── .gitignore                   # Git ignore rules
│
├── Source/                      # Source code
│   ├── PluginProcessor.h        # Audio processor header
│   ├── PluginProcessor.cpp      # Audio processor implementation
│   ├── PluginEditor.h           # UI editor header
│   ├── PluginEditor.cpp         # UI editor implementation
│   │
│   └── DSP/                     # Digital Signal Processing modules
│       ├── EffectModule.h       # Base class for all effects
│       ├── DSPUtils.h           # Common DSP utilities
│       ├── Compressor.h         # Compressor header
│       ├── Compressor.cpp       # Compressor implementation
│       ├── Distortion.h         # Distortion header
│       ├── Distortion.cpp       # Distortion implementation
│       ├── AmpSim.h             # Amp simulator header
│       ├── AmpSim.cpp           # Amp simulator implementation
│       ├── Modulation.h         # Modulation header
│       ├── Modulation.cpp       # Modulation implementation
│       ├── Reverb.h             # Reverb header
│       └── Reverb.cpp           # Reverb implementation
│
├── Builds/                      # Platform-specific build files
│   └── MacOSX/
│       └── PDLBRD.xcodeproj     # Xcode project
│
└── JuceLibraryCode/             # JUCE library wrapper code
    ├── JuceHeader.h
    └── ...
```

---

## Technical Details

### Audio Processing

- **Sample Rate**: Supports 44.1kHz, 48kHz, 88.2kHz, 96kHz, and higher
- **Bit Depth**: 32-bit floating-point internal processing
- **Buffer Size**: Adapts to host buffer size (tested with 32 to 2048 samples)
- **Latency**: Zero latency (no lookahead processing)
- **Channels**: Stereo processing (mono-compatible)

### DSP Architecture

#### Compressor
- Feedforward topology with RMS envelope detection
- Soft-knee compression curve
- Automatic attack/release coefficient calculation
- True parallel compression with wet/dry blend

#### Distortion
- Waveshaping algorithms with anti-aliasing
- Pre/post filtering for each distortion type
- Asymmetric clipping options for tube-like harmonics

#### Amp Simulator
- Cascaded gain stages
- Baxandall-style tone stack
- Parametric mid frequency control
- Cabinet simulation filtering

#### Modulation
- High-precision LFO with multiple waveforms
- All-pass filter network for phasing
- Delay-based chorus and flanger
- Interpolated delay lines for smooth modulation

#### Reverb
- Feedback delay network (FDN) architecture
- Schroeder all-pass diffusion
- Damping filters for natural decay
- Early reflection modeling for space definition

### UI Framework

- **Framework**: JUCE Graphics
- **Rendering**: Hardware-accelerated Core Graphics
- **Refresh Rate**: 30 Hz for meters and animations
- **Resolution**: Retina-ready vector graphics

### Parameter Management

- All parameters use JUCE's `AudioProcessorValueTreeState`
- Full automation support in all DAWs
- Thread-safe parameter access
- Undo/redo support (DAW-dependent)

---

## Troubleshooting

### Plugin Doesn't Appear in DAW

1. **Verify installation location**:
   ```bash
   ls -la ~/Library/Audio/Plug-Ins/Components/PDLBRD.component
   ```

2. **Rescan plugins** in your DAW's preferences

3. **Check plugin validation**:
   ```bash
   auval -v aufx PDLB Comp
   ```

4. **Check Console for errors**:
   - Open Console.app
   - Filter for your DAW's name
   - Look for PDLBRD-related errors

### Audio Glitches or Dropouts

1. **Increase buffer size** in your DAW's audio settings
2. **Disable unused effects** (bypassed effects use minimal CPU)
3. **Check CPU usage** in your DAW's performance meter
4. **Close other applications** using audio

### UI Issues

1. **Window too small**: The UI is scrollable - scroll down to see all pedals
2. **Knobs not responding**: Make sure you're clicking on the knob, not the label
3. **Drag-and-drop not working**: Click on the pedal body, not on controls

### Preset Issues

1. **Presets not saving**: Check write permissions for:
   ```bash
   ls -la ~/Library/Audio/Presets/PDLBRD/
   ```

2. **Create the directory if missing**:
   ```bash
   mkdir -p ~/Library/Audio/Presets/PDLBRD/
   ```

### Build Errors

1. **"JUCE modules not found"**: Update the JUCE path in the `.jucer` file
2. **Signing errors**: Build with "Sign to Run Locally" in Xcode
3. **SDK errors**: Ensure Xcode Command Line Tools are installed

---

## License

This project is provided as-is for educational and personal use. See the LICENSE file for details.

---

## Credits

### Development
- **Ian Fletcher** - Creator and lead developer
- **Claude (Anthropic)** - AI pair programming assistant

### Technologies
- **[JUCE Framework](https://juce.com/)** - Cross-platform audio application framework
- **[Xcode](https://developer.apple.com/xcode/)** - Apple's integrated development environment

### Inspiration
- **Compstortion Web App** - The original browser-based version
- **Classic guitar pedals** - For the visual design language
- **Professional audio plugins** - For UI/UX patterns

---

## Version History

### v1.0.0 (Current)
- Initial release
- Six effects: 2x Compressor, Distortion, Amp Sim, Modulation, Reverb
- Drag-and-drop signal chain reordering
- Preset save/load system
- Compstortion-style visual design
- Segmented LED gain reduction meters
- Full DAW automation support

---

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests on GitHub.

### Development Setup

1. Fork the repository
2. Clone your fork
3. Create a feature branch
4. Make your changes
5. Test thoroughly in multiple DAWs
6. Submit a pull request

---

## Contact

- **GitHub**: [@ianfletcher314](https://github.com/ianfletcher314)
- **Project Repository**: https://github.com/ianfletcher314/pdlbrd

---

*Built with love, coffee, and way too many late nights tweaking knob graphics.*
