extends Node

# Test variables for new widgets
var counter: int = 0
var slider_value: float = 0.5
var slider_vec2: Vector2 = Vector2(0.0, 0.5)
var slider_vec3: Vector3 = Vector3(0.0, 0.5, 1.0)
var checkbox_value: bool = false
var color_rgb: Color = Color(1.0, 0.0, 0.0)
var color_rgba: Color = Color(0.0, 1.0, 0.0, 0.5)
var combo_current: int = 0
var combo_items: PackedStringArray = ["Option 1", "Option 2", "Option 3", "Option 4"]
var drag_float_val: float = 50.0
var drag_int_val: int = 10
var drag_vec2_val: Vector2 = Vector2(1.0, 2.0)
var drag_vec3_val: Vector2 = Vector2(1.0, 2.0)
var text_input: String = "Hello ImGui!"
var show_popup: bool = false
var progress: float = 0.0

var imgui: ImGuiGodot

func _ready() -> void:
	imgui = ImGuiGodot.new()
	imgui.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	add_child(imgui)

func _process(delta):
	if not imgui: return

	# Automatically advance progress
	progress += delta * 0.1
	if progress > 1.0:
		progress = 0.0

	# Main window with menu bar
	if imgui.begin("Advanced ImGui Demo"):

		# Menu bar example
		if imgui.begin_menu_bar():
			if imgui.begin_menu("File"):
				if imgui.menu_item("New", "Ctrl+N"):
					print("New file")
				if imgui.menu_item("Open", "Ctrl+O"):
					print("Open file")
				imgui.separator()
				if imgui.menu_item("Exit"):
					print("Exit")
				imgui.end_menu()

			if imgui.begin_menu("Edit"):
				if imgui.menu_item("Undo", "Ctrl+Z"):
					print("Undo")
				if imgui.menu_item("Redo", "Ctrl+Y"):
					print("Redo")
				imgui.end_menu()

			imgui.end_menu_bar()

		# Tab bar example
		if imgui.begin_tab_bar("MainTabs"):
			if imgui.begin_tab_item("Widgets"):
				imgui.separator_text("Basic Widgets")

				imgui.text("Counter: " + str(counter))
				if imgui.button("Increment"):
					counter += 1
				imgui.same_line()
				if imgui.small_button("Reset"):
					counter = 0

				checkbox_value = imgui.checkbox("Checkbox", checkbox_value)

				imgui.separator_text("Sliders")
				slider_value = imgui.slider_float("Float Slider", slider_value, 0.0, 1.0)
				slider_vec2 = imgui.slider_float2("Vec2 Slider", slider_vec2, 0.0, 1.0)
				slider_vec3 = imgui.slider_float3("Vec3 Slider", slider_vec3, 0.0, 1.0)

				imgui.separator_text("Drag Widgets")
				drag_float_val = imgui.drag_float("Drag Float", drag_float_val, 1.0, 0.0, 100.0)
				drag_int_val = imgui.drag_int("Drag Int", drag_int_val, 1.0, 0, 100)
				drag_vec2_val = imgui.drag_float2("Drag Vec2", drag_vec2_val, 0.1)

				imgui.separator_text("Input Widgets")
				text_input = imgui.input_text("Text Input", text_input)

				imgui.end_tab_item()

			if imgui.begin_tab_item("Colors"):
				imgui.text("Color pickers and editors:")

				color_rgb = imgui.color_edit3("RGB Color", color_rgb)
				color_rgba = imgui.color_edit4("RGBA Color", color_rgba)

				imgui.spacing()
				imgui.text("Color picker:")
				color_rgb = imgui.color_picker3("Pick Color", color_rgb)

				imgui.spacing()
				if imgui.color_button("color_btn", color_rgba, 0, Vector2(50, 50)):
					print("Color button clicked!")

				imgui.end_tab_item()

			if imgui.begin_tab_item("Combos & Lists"):
				imgui.text("Combo box:")
				combo_current = imgui.combo("Combo", combo_current, combo_items)
				imgui.text("Selected: " + combo_items[combo_current])

				imgui.separator()

				# Manual combo with begin_combo/end_combo
				if imgui.begin_combo("Custom Combo", combo_items[combo_current]):
					for i in range(combo_items.size()):
						if imgui.selectable(combo_items[i], i == combo_current):
							combo_current = i
					imgui.end_combo()

				imgui.separator_text("Tree Nodes")
				if imgui.tree_node("Parent Node"):
					imgui.text("Child content 1")
					imgui.text("Child content 2")
					if imgui.tree_node("Nested Node"):
						imgui.text("Deeply nested content")
						imgui.tree_pop()
					imgui.tree_pop()

				if imgui.collapsing_header("Collapsing Header"):
					imgui.text("Content under collapsing header")
					imgui.bullet_text("Bullet point 1")
					imgui.bullet_text("Bullet point 2")

				imgui.end_tab_item()

			if imgui.begin_tab_item("Popups & Tooltips"):
				if imgui.button("Open Popup"):
					imgui.open_popup("test_popup")

				if imgui.begin_popup("test_popup"):
					imgui.text("This is a popup!")
					if imgui.button("Close"):
						imgui.close_current_popup()
					imgui.end_popup()

				imgui.separator()

				imgui.button("Hover for tooltip")
				if imgui.is_item_hovered():
					imgui.set_tooltip("This is a tooltip!")

				imgui.separator()

				if imgui.button("Open Modal"):
					imgui.open_popup("modal_popup")

				if imgui.begin_popup_modal("modal_popup", true):
					imgui.text("This is a modal popup")
					imgui.text("It blocks interaction with the rest of the UI")
					if imgui.button("OK"):
						imgui.close_current_popup()
					imgui.end_popup()

				imgui.end_tab_item()

			if imgui.begin_tab_item("Tables"):
				if imgui.begin_table("table1", 3):
					imgui.table_setup_column("Name")
					imgui.table_setup_column("Age")
					imgui.table_setup_column("City")
					imgui.table_headers_row()

					for i in range(5):
						imgui.table_next_row()
						imgui.table_next_column()
						imgui.text("Person " + str(i + 1))
						imgui.table_next_column()
						imgui.text(str(20 + i * 5))
						imgui.table_next_column()
						imgui.text("City " + str(i + 1))

					imgui.end_table()

				imgui.end_tab_item()

			if imgui.begin_tab_item("Layout"):
				imgui.text("Layout utilities:")

				imgui.button("Button 1")
				imgui.same_line()
				imgui.button("Button 2")
				imgui.same_line()
				imgui.button("Button 3")

				imgui.separator()

				imgui.begin_group()
				imgui.text("Grouped")
				imgui.text("Items")
				imgui.end_group()
				imgui.same_line()
				imgui.button("Next to group")

				imgui.separator()
				imgui.text("Progress bar:")
				imgui.progress_bar(progress, Vector2(-1, 0), str(int(progress * 100)) + "%")

				imgui.separator()
				imgui.text("Available region: " + str(imgui.get_content_region_avail()))
				imgui.text("Cursor pos: " + str(imgui.get_cursor_pos()))
				imgui.text("Window size: " + str(imgui.get_window_size()))

				imgui.end_tab_item()

			if imgui.begin_tab_item("Child Windows"):
				var child_size := Vector2(200, 100)

				imgui.text("Child window example:")
				if imgui.begin_child("child1", child_size, 1):
					imgui.text("Inside child window")
					for i in range(10):
						imgui.text("Scrollable line " + str(i))
				imgui.end_child()

				imgui.end_tab_item()

			imgui.end_tab_bar()

	imgui.end()

	# Second window to demonstrate multiple windows
	imgui.set_next_window_pos(Vector2(650, 50), 1)
	imgui.set_next_window_size(Vector2(300, 200), 1)
	if imgui.begin("Info Window"):
		imgui.text("This demonstrates multiple windows")
		imgui.separator()
		imgui.text("Mouse pos: " + str(imgui.get_mouse_pos()))
		imgui.text("Mouse down: " + str(imgui.is_mouse_down(0)))
		imgui.text("Window focused: " + str(imgui.is_window_focused()))
		imgui.text("Window hovered: " + str(imgui.is_window_hovered()))
	imgui.end()
