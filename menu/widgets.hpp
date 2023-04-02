#pragma once

#include "ImGui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui_internal.h"
#include "menu.h"

namespace widgets {
    void begin_child(const char* name, ImVec2 size);
    void end_child();
    IMGUI_API bool Checkbox(const char* label, bool* v);
    void AddCircleImageFilled(ImTextureID user_texture_id, const ImVec2& centre, float radius, ImU32 col, int num_segments);
}

struct checkbox_animation {
    float animation;
};