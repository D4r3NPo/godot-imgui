extends Node

## Example showing how to use ImGui's built-in demo window
## The demo window is an excellent way to learn all ImGui features!

var imgui: ImGuiGodot
var show_demo: bool = true
var show_metrics: bool = false
var show_about: bool = false
var show_style_editor: bool = false

func _ready() -> void:
	imgui = ImGuiGodot.new()
	imgui.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	add_child(imgui)

func _process(_delta):
	if not imgui:
		return

	# Show a simple control window
	if imgui.begin("ImGui Demo Controls"):
		imgui.text("Toggle ImGui's built-in windows:")
		imgui.separator()

		show_demo = imgui.checkbox("Show Demo Window", show_demo)
		imgui.separator_text("Debug Windows")
		show_metrics = imgui.checkbox("Show Metrics/Debugger", show_metrics)
		show_about = imgui.checkbox("Show About Window", show_about)
		show_style_editor = imgui.checkbox("Show Style Editor", show_style_editor)

		imgui.separator()
		imgui.text_wrapped("The Demo Window showcases all ImGui features and is the best way to learn the API!")

	imgui.end()

	# Show the ImGui demo window - this is a comprehensive showcase of all ImGui features
	if show_demo:
		imgui.show_demo_window(true)

	# Show metrics/debugger window - useful for debugging ImGui itself
	if show_metrics:
		imgui.show_metrics_window(true)

	# Show about window - displays ImGui version and credits
	if show_about:
		imgui.show_about_window(true)

	# Show style editor - allows live editing of ImGui's visual style
	if show_style_editor:
		if imgui.begin("Style Editor"):
			imgui.show_style_editor()
		imgui.end()
