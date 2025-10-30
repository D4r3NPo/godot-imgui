#include "imgui_godot.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/display_server.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void ImGuiGodot::_bind_methods() {
    // Frame control
    ClassDB::bind_method(D_METHOD("begin_frame"), &ImGuiGodot::begin_frame);
    ClassDB::bind_method(D_METHOD("end_frame"), &ImGuiGodot::end_frame);

    // Window functions
    ClassDB::bind_method(D_METHOD("begin", "name", "has_close_button"), &ImGuiGodot::begin, DEFVAL(false));
    ClassDB::bind_method(D_METHOD("end"), &ImGuiGodot::end);

    // Widgets: Text
    ClassDB::bind_method(D_METHOD("text", "text"), &ImGuiGodot::text);
    ClassDB::bind_method(D_METHOD("text_colored", "color", "text"), &ImGuiGodot::text_colored);
    ClassDB::bind_method(D_METHOD("text_disabled", "text"), &ImGuiGodot::text_disabled);
    ClassDB::bind_method(D_METHOD("text_wrapped", "text"), &ImGuiGodot::text_wrapped);
    ClassDB::bind_method(D_METHOD("label_text", "label", "text"), &ImGuiGodot::label_text);
    ClassDB::bind_method(D_METHOD("bullet_text", "text"), &ImGuiGodot::bullet_text);

    // Widgets: Main
    ClassDB::bind_method(D_METHOD("button", "label", "size"), &ImGuiGodot::button, DEFVAL(Vector2(0, 0)));
    ClassDB::bind_method(D_METHOD("small_button", "label"), &ImGuiGodot::small_button);
    ClassDB::bind_method(D_METHOD("checkbox", "label", "value"), &ImGuiGodot::checkbox);
    ClassDB::bind_method(D_METHOD("radio_button", "label", "active"), &ImGuiGodot::radio_button);

    // Widgets: Sliders
    ClassDB::bind_method(D_METHOD("slider_float", "label", "value", "min_value", "max_value"), &ImGuiGodot::slider_float);
    ClassDB::bind_method(D_METHOD("slider_int", "label", "value", "min_value", "max_value"), &ImGuiGodot::slider_int);

    // Widgets: Input
    ClassDB::bind_method(D_METHOD("input_text", "label", "text", "max_length"), &ImGuiGodot::input_text, DEFVAL(256));
    ClassDB::bind_method(D_METHOD("input_float", "label", "value"), &ImGuiGodot::input_float);
    ClassDB::bind_method(D_METHOD("input_int", "label", "value"), &ImGuiGodot::input_int);

    // Widgets: Trees
    ClassDB::bind_method(D_METHOD("tree_node", "label"), &ImGuiGodot::tree_node);
    ClassDB::bind_method(D_METHOD("tree_pop"), &ImGuiGodot::tree_pop);

    // Widgets: Selectables
    ClassDB::bind_method(D_METHOD("selectable", "label", "selected"), &ImGuiGodot::selectable, DEFVAL(false));

    // Layout
    ClassDB::bind_method(D_METHOD("separator"), &ImGuiGodot::separator);
    ClassDB::bind_method(D_METHOD("same_line", "offset_from_start_x", "spacing"), &ImGuiGodot::same_line, DEFVAL(0.0f), DEFVAL(-1.0f));
    ClassDB::bind_method(D_METHOD("new_line"), &ImGuiGodot::new_line);
    ClassDB::bind_method(D_METHOD("spacing"), &ImGuiGodot::spacing);
    ClassDB::bind_method(D_METHOD("indent", "indent_w"), &ImGuiGodot::indent, DEFVAL(0.0f));
    ClassDB::bind_method(D_METHOD("unindent", "indent_w"), &ImGuiGodot::unindent, DEFVAL(0.0f));

    // Cursor / Layout
    ClassDB::bind_method(D_METHOD("get_cursor_pos"), &ImGuiGodot::get_cursor_pos);
    ClassDB::bind_method(D_METHOD("set_cursor_pos", "pos"), &ImGuiGodot::set_cursor_pos);

    // ID stack
    ClassDB::bind_method(D_METHOD("push_id", "id"), &ImGuiGodot::push_id);
    ClassDB::bind_method(D_METHOD("pop_id"), &ImGuiGodot::pop_id);

    // Style
    ClassDB::bind_method(D_METHOD("push_style_color", "idx", "color"), &ImGuiGodot::push_style_color);
    ClassDB::bind_method(D_METHOD("pop_style_color", "count"), &ImGuiGodot::pop_style_color, DEFVAL(1));
}

ImGuiGodot::ImGuiGodot() {
    initialized = false;
    imgui_context = nullptr;
    time = 0.0;
    mouse_pos = Vector2(0, 0);

    for (int i = 0; i < 5; i++) {
        mouse_buttons[i] = false;
    }
}

ImGuiGodot::~ImGuiGodot() {
    if (imgui_context) {
        ImGui::DestroyContext(imgui_context);
        imgui_context = nullptr;
    }
}

void ImGuiGodot::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Create ImGui context
    imgui_context = ImGui::CreateContext();
    ImGui::SetCurrentContext(imgui_context);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup display size
    Viewport* viewport = get_viewport();
    if (viewport) {
        Rect2 rect = viewport->get_visible_rect();
        io.DisplaySize = ImVec2(rect.size.x, rect.size.y);
    }

    setup_imgui_style();
    create_fonts_texture();

    // Prepare a new frame before any node try use it
    get_tree()->connect("process_frame", Callable(this, "begin_frame"));

    // Execute process later to call end_frame() after all nodes
    set_process_priority(100);

    UtilityFunctions::print("ImGui-Godot initialized");
    initialized = true;
}

void ImGuiGodot::_process(double delta) {
    if (!initialized || Engine::get_singleton()->is_editor_hint()) { return; }

    time += delta;

    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = (float)delta;

    // Update display size
    Viewport* viewport = get_viewport();
    if (viewport) {
        Rect2 rect = viewport->get_visible_rect();
        io.DisplaySize = ImVec2(rect.size.x, rect.size.y);
    }

    // Update mouse position
    io.MousePos = ImVec2(mouse_pos.x, mouse_pos.y);

    // Update mouse buttons
    for (int i = 0; i < 5; i++) {
        io.MouseDown[i] = mouse_buttons[i];
    }

   	if (has_pending_frame) {
	 	end_frame();
		has_pending_frame = false;
	}
}

void ImGuiGodot::_input(const Ref<InputEvent> &event) {
    if (!initialized || Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

    // Mouse motion
    Ref<InputEventMouseMotion> mouse_motion = event;
    if (mouse_motion.is_valid()) {
        mouse_pos = mouse_motion->get_position();
    }

    // Mouse buttons
    Ref<InputEventMouseButton> mouse_button = event;
    if (mouse_button.is_valid()) {
        int button_index = (int)mouse_button->get_button_index();
        if (button_index >= 1 && button_index <= 5) {
            mouse_buttons[button_index - 1] = mouse_button->is_pressed();
        }

        // Mouse wheel
        if (button_index == (int)MouseButton::MOUSE_BUTTON_WHEEL_UP) {
            io.MouseWheel += 1.0f;
        } else if (button_index == (int)MouseButton::MOUSE_BUTTON_WHEEL_DOWN) {
            io.MouseWheel -= 1.0f;
        }
    }

    // Keyboard
    Ref<InputEventKey> key_event = event;
    if (key_event.is_valid()) {
        // Update modifier keys
        io.AddKeyEvent(ImGuiMod_Ctrl, key_event->is_ctrl_pressed());
        io.AddKeyEvent(ImGuiMod_Shift, key_event->is_shift_pressed());
        io.AddKeyEvent(ImGuiMod_Alt, key_event->is_alt_pressed());
        io.AddKeyEvent(ImGuiMod_Super, key_event->is_meta_pressed());

        // Map Godot keys to ImGui keys
        ImGuiKey imgui_key = ImGuiKey_None;
        Key godot_key = key_event->get_keycode();

        // Basic mapping - a complete implementation would map all keys
        switch (godot_key) {
            case Key::KEY_TAB: imgui_key = ImGuiKey_Tab; break;
            case Key::KEY_LEFT: imgui_key = ImGuiKey_LeftArrow; break;
            case Key::KEY_RIGHT: imgui_key = ImGuiKey_RightArrow; break;
            case Key::KEY_UP: imgui_key = ImGuiKey_UpArrow; break;
            case Key::KEY_DOWN: imgui_key = ImGuiKey_DownArrow; break;
            case Key::KEY_PAGEUP: imgui_key = ImGuiKey_PageUp; break;
            case Key::KEY_PAGEDOWN: imgui_key = ImGuiKey_PageDown; break;
            case Key::KEY_HOME: imgui_key = ImGuiKey_Home; break;
            case Key::KEY_END: imgui_key = ImGuiKey_End; break;
            case Key::KEY_INSERT: imgui_key = ImGuiKey_Insert; break;
            case Key::KEY_DELETE: imgui_key = ImGuiKey_Delete; break;
            case Key::KEY_BACKSPACE: imgui_key = ImGuiKey_Backspace; break;
            case Key::KEY_SPACE: imgui_key = ImGuiKey_Space; break;
            case Key::KEY_ENTER: imgui_key = ImGuiKey_Enter; break;
            case Key::KEY_ESCAPE: imgui_key = ImGuiKey_Escape; break;
            case Key::KEY_A: imgui_key = ImGuiKey_A; break;
            case Key::KEY_C: imgui_key = ImGuiKey_C; break;
            case Key::KEY_V: imgui_key = ImGuiKey_V; break;
            case Key::KEY_X: imgui_key = ImGuiKey_X; break;
            case Key::KEY_Y: imgui_key = ImGuiKey_Y; break;
            case Key::KEY_Z: imgui_key = ImGuiKey_Z; break;
            default: break;
        }

        if (imgui_key != ImGuiKey_None) {
            io.AddKeyEvent(imgui_key, key_event->is_pressed());
        }

        // Add text input
        if (key_event->is_pressed() && key_event->get_unicode() != 0) {
            char32_t unicode = key_event->get_unicode();
            if (unicode < 0x10000) {
                io.AddInputCharacter((unsigned short)unicode);
            }
        }
    }
}

void ImGuiGodot::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_DRAW: {
            if (initialized && !Engine::get_singleton()->is_editor_hint()) {
                // Render ImGui frame
                ImGui::Render();
                render_draw_data();
            }
            break;
        }
    }
}

void ImGuiGodot::setup_imgui_style() {
    ImGui::StyleColorsDark();
}

void ImGuiGodot::create_fonts_texture() {
    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Create Godot Image from ImGui font texture data
    PackedByteArray data;
    data.resize(width * height * 4);
    memcpy(data.ptrw(), pixels, width * height * 4);

    Ref<Image> image = Image::create_from_data(width, height, false, Image::FORMAT_RGBA8, data);

    // Create ImageTexture from Image
    font_texture = ImageTexture::create_from_image(image);

    // Store texture ID in ImGui
    io.Fonts->TexID = (ImTextureID)(uintptr_t)font_texture->get_rid().get_id();
}

void ImGuiGodot::render_draw_data() {
    ImDrawData* draw_data = ImGui::GetDrawData();

    if (!draw_data || draw_data->CmdListsCount == 0) {
        return;
    }

    // ImGui uses top-left origin, Godot uses top-left too, so no Y-flip needed
    Vector2 clip_off = Vector2(draw_data->DisplayPos.x, draw_data->DisplayPos.y);
    Vector2 clip_scale = Vector2(draw_data->FramebufferScale.x, draw_data->FramebufferScale.y);

    // Render command lists
    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
        const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

            if (pcmd->UserCallback) {
                pcmd->UserCallback(cmd_list, pcmd);
            } else {
                // Calculate clip rectangle
                Vector2 clip_min(
                    (pcmd->ClipRect.x - clip_off.x) * clip_scale.x,
                    (pcmd->ClipRect.y - clip_off.y) * clip_scale.y
                );
                Vector2 clip_max(
                    (pcmd->ClipRect.z - clip_off.x) * clip_scale.x,
                    (pcmd->ClipRect.w - clip_off.y) * clip_scale.y
                );

                // Skip drawing if clipped
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y) {
                    continue;
                }

                // Draw triangles
                for (unsigned int i = 0; i < pcmd->ElemCount; i += 3) {
                    // Get the three vertices for this triangle
                    const ImDrawVert& v0 = vtx_buffer[idx_buffer[pcmd->IdxOffset + i + 0]];
                    const ImDrawVert& v1 = vtx_buffer[idx_buffer[pcmd->IdxOffset + i + 1]];
                    const ImDrawVert& v2 = vtx_buffer[idx_buffer[pcmd->IdxOffset + i + 2]];

                    // Convert to Godot format
                    Vector2 p0(v0.pos.x, v0.pos.y);
                    Vector2 p1(v1.pos.x, v1.pos.y);
                    Vector2 p2(v2.pos.x, v2.pos.y);

                    // Convert colors from RGBA packed to Godot Color
                    Color c0(
                        ((v0.col >> IM_COL32_R_SHIFT) & 0xFF) / 255.0f,
                        ((v0.col >> IM_COL32_G_SHIFT) & 0xFF) / 255.0f,
                        ((v0.col >> IM_COL32_B_SHIFT) & 0xFF) / 255.0f,
                        ((v0.col >> IM_COL32_A_SHIFT) & 0xFF) / 255.0f
                    );
                    Color c1(
                        ((v1.col >> IM_COL32_R_SHIFT) & 0xFF) / 255.0f,
                        ((v1.col >> IM_COL32_G_SHIFT) & 0xFF) / 255.0f,
                        ((v1.col >> IM_COL32_B_SHIFT) & 0xFF) / 255.0f,
                        ((v1.col >> IM_COL32_A_SHIFT) & 0xFF) / 255.0f
                    );
                    Color c2(
                        ((v2.col >> IM_COL32_R_SHIFT) & 0xFF) / 255.0f,
                        ((v2.col >> IM_COL32_G_SHIFT) & 0xFF) / 255.0f,
                        ((v2.col >> IM_COL32_B_SHIFT) & 0xFF) / 255.0f,
                        ((v2.col >> IM_COL32_A_SHIFT) & 0xFF) / 255.0f
                    );

                    // UV coordinates
                    Vector2 uv0(v0.uv.x, v0.uv.y);
                    Vector2 uv1(v1.uv.x, v1.uv.y);
                    Vector2 uv2(v2.uv.x, v2.uv.y);

                    // Prepare arrays for draw_primitive
                    PackedVector2Array points;
                    points.push_back(p0);
                    points.push_back(p1);
                    points.push_back(p2);

                    PackedColorArray colors;
                    colors.push_back(c0);
                    colors.push_back(c1);
                    colors.push_back(c2);

                    PackedVector2Array uvs;
                    uvs.push_back(uv0);
                    uvs.push_back(uv1);
                    uvs.push_back(uv2);

                    // Get texture if available
                    Ref<Texture2D> texture;
                    ImTextureID tex_id = pcmd->GetTexID();
                    if (tex_id) {
                        // Use font texture if texture ID matches
                        if (font_texture.is_valid() &&
                            tex_id == (ImTextureID)(uintptr_t)font_texture->get_rid().get_id()) {
                            texture = font_texture;
                        }
                    }

                    // Draw the triangle
                    draw_primitive(points, colors, uvs, texture);
                }
            }
        }
    }
}

void ImGuiGodot::update_mouse_cursor() {
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) {
        return;
    }

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    // Map ImGui cursor to Godot cursor
}

void ImGuiGodot::begin_frame() {
    if (!initialized || has_pending_frame) { return; }
    ImGui::NewFrame();
    has_pending_frame = true;
}

void ImGuiGodot::end_frame() {
    if (!initialized) { return; }
    ImGui::Render();
    queue_redraw();
}

// ============================================================================
// GDScript API Wrappers
// ============================================================================

// Window functions
bool ImGuiGodot::begin(const String &name, bool has_close_button) {
    if (!initialized) return false;

    if (has_close_button) {
        bool open = true;
        return ImGui::Begin(name.utf8().get_data(), &open);
    } else {
        return ImGui::Begin(name.utf8().get_data(), nullptr);
    }
}

void ImGuiGodot::end() {
    if (!initialized) return;
    ImGui::End();
}

// Widgets: Text
void ImGuiGodot::text(const String &text) {
    if (!initialized) return;
    ImGui::TextUnformatted(text.utf8().get_data());
}

void ImGuiGodot::text_colored(const Color &color, const String &text) {
    if (!initialized) return;
    ImGui::TextColored(ImVec4(color.r, color.g, color.b, color.a), "%s", text.utf8().get_data());
}

void ImGuiGodot::text_disabled(const String &text) {
    if (!initialized) return;
    ImGui::TextDisabled("%s", text.utf8().get_data());
}

void ImGuiGodot::text_wrapped(const String &text) {
    if (!initialized) return;
    ImGui::TextWrapped("%s", text.utf8().get_data());
}

void ImGuiGodot::label_text(const String &label, const String &text) {
    if (!initialized) return;
    ImGui::LabelText(label.utf8().get_data(), "%s", text.utf8().get_data());
}

void ImGuiGodot::bullet_text(const String &text) {
    if (!initialized) return;
    ImGui::BulletText("%s", text.utf8().get_data());
}

// Widgets: Main
bool ImGuiGodot::button(const String &label, const Vector2 &size) {
    if (!initialized) return false;
    return ImGui::Button(label.utf8().get_data(), ImVec2(size.x, size.y));
}

bool ImGuiGodot::small_button(const String &label) {
    if (!initialized) return false;
    return ImGui::SmallButton(label.utf8().get_data());
}

bool ImGuiGodot::checkbox(const String &label, bool value) {
    if (!initialized) return value;
    ImGui::Checkbox(label.utf8().get_data(), &value);
    return value;
}

bool ImGuiGodot::radio_button(const String &label, bool active) {
    if (!initialized) return false;
    return ImGui::RadioButton(label.utf8().get_data(), active);
}

// Widgets: Sliders
float ImGuiGodot::slider_float(const String &label, float value, float min_value, float max_value) {
    if (!initialized) return value;
    ImGui::SliderFloat(label.utf8().get_data(), &value, min_value, max_value);
    return value;
}

int ImGuiGodot::slider_int(const String &label, int value, int min_value, int max_value) {
    if (!initialized) return value;
    ImGui::SliderInt(label.utf8().get_data(), &value, min_value, max_value);
    return value;
}

// Widgets: Input
String ImGuiGodot::input_text(const String &label, const String &text, int max_length) {
    if (!initialized) return text;

    char* buffer = new char[max_length];
    strncpy(buffer, text.utf8().get_data(), max_length - 1);
    buffer[max_length - 1] = '\0';

    ImGui::InputText(label.utf8().get_data(), buffer, max_length);
    String result = String(buffer);
    delete[] buffer;

    return result;
}

float ImGuiGodot::input_float(const String &label, float value) {
    if (!initialized) return value;
    ImGui::InputFloat(label.utf8().get_data(), &value);
    return value;
}

int ImGuiGodot::input_int(const String &label, int value) {
    if (!initialized) return value;
    ImGui::InputInt(label.utf8().get_data(), &value);
    return value;
}

// Widgets: Trees
bool ImGuiGodot::tree_node(const String &label) {
    if (!initialized) return false;
    return ImGui::TreeNode(label.utf8().get_data());
}

void ImGuiGodot::tree_pop() {
    if (!initialized) return;
    ImGui::TreePop();
}

// Widgets: Selectables
bool ImGuiGodot::selectable(const String &label, bool selected) {
    if (!initialized) return selected;
    ImGui::Selectable(label.utf8().get_data(), &selected);
    return selected;
}

// Layout
void ImGuiGodot::separator() {
    if (!initialized) return;
    ImGui::Separator();
}

void ImGuiGodot::same_line(float offset_from_start_x, float spacing) {
    if (!initialized) return;
    ImGui::SameLine(offset_from_start_x, spacing);
}

void ImGuiGodot::new_line() {
    if (!initialized) return;
    ImGui::NewLine();
}

void ImGuiGodot::spacing() {
    if (!initialized) return;
    ImGui::Spacing();
}

void ImGuiGodot::indent(float indent_w) {
    if (!initialized) return;
    ImGui::Indent(indent_w);
}

void ImGuiGodot::unindent(float indent_w) {
    if (!initialized) return;
    ImGui::Unindent(indent_w);
}

// Cursor / Layout
Vector2 ImGuiGodot::get_cursor_pos() {
    if (!initialized) return Vector2(0, 0);
    ImVec2 pos = ImGui::GetCursorPos();
    return Vector2(pos.x, pos.y);
}

void ImGuiGodot::set_cursor_pos(const Vector2 &pos) {
    if (!initialized) return;
    ImGui::SetCursorPos(ImVec2(pos.x, pos.y));
}

// ID stack
void ImGuiGodot::push_id(const String &id) {
    if (!initialized) return;
    ImGui::PushID(id.utf8().get_data());
}

void ImGuiGodot::pop_id() {
    if (!initialized) return;
    ImGui::PopID();
}

// Style
void ImGuiGodot::push_style_color(int idx, const Color &color) {
    if (!initialized) return;
    ImGui::PushStyleColor(idx, ImVec4(color.r, color.g, color.b, color.a));
}

void ImGuiGodot::pop_style_color(int count) {
    if (!initialized) return;
    ImGui::PopStyleColor(count);
}
