# ImGui-Godot

A Dear ImGui binding for Godot 4.3+ using GDExtension (C++).

## Features

- Native C++ GDExtension implementation
- Direct integration with Godot's input system
- Compatible with Godot 4.3+
- Cross-platform support

## Usage

1. Add an `ImGuiGodot` node to your scene:

```gdscript
extends Node

var imgui: ImGuiGodot

func _ready() -> void:
	imgui = ImGuiGodot.new()
	imgui.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	add_child(imgui)
```

2. Use ImGui in your scripts:

```gdscript

var counter: int = 0
var slider_value: float = 0.5
var checkbox_value: bool = false

func _process(_delta):
	if not imgui: return

	if imgui.begin("ImGui Window !"):
		imgui.text("Welcome to ImGui-Godot!")

		if imgui.button("Button"):
			print("Button clicked from GDScript!")

		imgui.separator()

		imgui.text("Counter: " + str(counter))
		if imgui.button("Increment Counter"):
			counter += 1

		imgui.separator()
		checkbox_value = imgui.checkbox("Test Checkbox", checkbox_value)
		slider_value = imgui.slider_float("Test Slider", slider_value, 0.0, 1.0)

	imgui.end()
```

## Installation

1. Download `godot-imgui-addon.zip`
2. Extract the `addons` folder
3. Copy the `addons/godot-imgui` folder into your Godot project's `addons` directory
4. Restart Godot or reload your project

## Building

### Prerequisites

- Python 3.6+
- SCons build system
- C++17 compatible compiler
  - macOS: Xcode Command Line Tools
  - Windows: Visual Studio 2019 or newer
  - Linux: GCC 9+ or Clang 10+
- Git (for cloning with submodules)

### Build Instructions

1. Clone the repository with submodules:

```bash
git clone --recurse-submodules https://github.com/D4r3NPo/godot-imgui.git
cd imgui-godot
```

**If you already cloned without submodules:**

```bash
git submodule update --init --recursive
```

2. Build for your platform:

```bash
scons platform=<macos|windows|linux> target=<template_debug|template_release>
```

3. The compiled library will be placed in `bin/`

### Testing

Open the `demo` folder in the Godot editor.

## License

This project integrates:

- Dear ImGui (MIT License)
- godot-cpp (MIT License)
