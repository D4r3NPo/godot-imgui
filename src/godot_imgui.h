#ifndef IMGUI_GODOT_H
#define IMGUI_GODOT_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <../imgui/imgui.h>

namespace godot {

class ImGuiGodot : public Control {
    GDCLASS(ImGuiGodot, Control)

private:
    bool initialized;
    bool has_pending_frame;
    ImGuiContext* imgui_context;
    double time;

    // Rendering
    RID canvas_item;
    Ref<ImageTexture> font_texture;

    // Input state
    Vector2 mouse_pos;
    bool mouse_buttons[5];

    void setup_imgui_style();
    void create_fonts_texture();
    void render_draw_data();
    void update_mouse_cursor();
    void frame();

protected:
    static void _bind_methods();

public:
    ImGuiGodot();
    ~ImGuiGodot();

    void _ready() override;
    void _process(double delta) override;
    void _input(const Ref<InputEvent> &event) override;
    void _notification(int p_what);

    // Frame control (manual mode)
    void begin_frame();
    void end_frame();

    // Window functions
    bool begin(const String &name, bool has_close_button = false);
    void end();

    // Widgets: Text
    void text(const String &text);
    void text_colored(const Color &color, const String &text);
    void text_disabled(const String &text);
    void text_wrapped(const String &text);
    void label_text(const String &label, const String &text);
    void bullet_text(const String &text);

    // Widgets: Main
    bool button(const String &label, const Vector2 &size = Vector2(0, 0));
    bool small_button(const String &label);
    bool checkbox(const String &label, bool value);
    bool radio_button(const String &label, bool active);

    // Widgets: Sliders
    float slider_float(const String &label, float value, float min_value, float max_value);
    int slider_int(const String &label, int value, int min_value, int max_value);

    // Widgets: Input
    String input_text(const String &label, const String &text, int max_length = 256);
    float input_float(const String &label, float value);
    int input_int(const String &label, int value);

    // Widgets: Trees
    bool tree_node(const String &label);
    void tree_pop();

    // Widgets: Selectables
    bool selectable(const String &label, bool selected = false);

    // Layout
    void separator();
    void same_line(float offset_from_start_x = 0.0f, float spacing = -1.0f);
    void new_line();
    void spacing();
    void indent(float indent_w = 0.0f);
    void unindent(float indent_w = 0.0f);

    // Cursor / Layout
    Vector2 get_cursor_pos();
    void set_cursor_pos(const Vector2 &pos);

    // ID stack
    void push_id(const String &id);
    void pop_id();

    // Style
    void push_style_color(int idx, const Color &color);
    void pop_style_color(int count = 1);

    // Internal - not exposed to GDScript
    ImGuiContext* get_context_internal() { return imgui_context; }
};

} // namespace godot

#endif // IMGUI_GODOT_H
