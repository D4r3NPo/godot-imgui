extends Node

var counter: int = 0
var slider_value: float = 0.5
var checkbox_value: bool = false
var imgui: ImGuiGodot

func _ready() -> void:
	imgui = ImGuiGodot.new()
	imgui.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	add_child(imgui)

func _process(_delta):
	if not imgui: return

	if imgui.begin("GDScript Window"):
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
