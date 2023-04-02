#include "widgets.hpp"

#include <string>
#include <map>

using namespace ImGui;

void widgets::begin_child(const char* name, ImVec2 size) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    ImVec2 p = window->DC.CursorPos;

    BeginChild(std::string(name).append("main").c_str(), size, true, ImGuiWindowFlags_NoScrollbar);
    GetWindowDrawList()->AddText(c_menu::get().futura_large, 18.f, { p.x + 15, p.y + 15 }, GetColorU32(ImGuiCol_Scheme), name);
    SetCursorPosY(40);
    BeginChild(name, { size.x, size.y == 0 ? size.y : size.y - 40 }, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
    SetCursorPosY(10);
    SetCursorPosX(15);
    BeginGroup();

    PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10, 12 });
}

void widgets::end_child() {
    PopStyleVar();
    EndGroup();
    EndChild();
    EndChild();
}

bool widgets::Checkbox(const char* label, bool* v)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    const float w = 0;
    const float square_sz = 17;
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect frame_bb(pos + ImVec2(w - square_sz - 13, 0), window->DC.CursorPos + ImVec2(w, square_sz - 1));
    const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
    {
        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
        return false;
    }

    bool hovered, held;
    bool pressed = ButtonBehavior(frame_bb, id, &hovered, &held);
    if (pressed)
    {
        *v = !(*v);
        MarkItemEdited(id);
    }

    static std::map <ImGuiID, checkbox_animation> anim;
    auto it_anim = anim.find(id);
    if (it_anim == anim.end())
    {
        anim.insert({ id, { 0.0f } });
        it_anim = anim.find(id);
    }

    it_anim->second.animation = ImLerp(it_anim->second.animation, *v ? 1.0f : 0.0f, 0.12f * (1.0f - ImGui::GetIO().DeltaTime));

    RenderNavHighlight(total_bb, id);

    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_Border), false, 9.0f);
    RenderFrame(frame_bb.Min, frame_bb.Max, ImColor(142 / 255.0f, 169 / 255.0f, 248 / 255.0f, it_anim->second.animation), false, 9.0f);

    window->DrawList->AddCircleFilled(ImVec2(frame_bb.Min.x + 8 + 14 * it_anim->second.animation, frame_bb.Min.y + 8), 5.0f, ImColor(1.0f, 1.0f, 1.0f), 30);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
    return it_anim->second.animation;
}

void widgets::AddCircleImageFilled(ImTextureID user_texture_id, const ImVec2& centre, float radius, ImU32 col, int num_segments)
{
    auto window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    window->DrawList->PathClear();

    if ((col & IM_COL32_A_MASK) == 0 || num_segments <= 2)
        return;

    const bool push_texture_id = window->DrawList->_TextureIdStack.empty() || user_texture_id != window->DrawList->_TextureIdStack.back();
    if (push_texture_id)
        window->DrawList->PushTextureID(user_texture_id);

    int vert_start_idx = window->DrawList->VtxBuffer.Size;
    const float a_max = IM_PI * 2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
    window->DrawList->PathArcTo(centre, radius, 0.0f, a_max, num_segments - 1);
    window->DrawList->PathFillConvex(col);
    int vert_end_idx = window->DrawList->VtxBuffer.Size;

    ImGui::ShadeVertsLinearUV(window->DrawList, vert_start_idx, vert_end_idx, ImVec2(centre.x - radius, centre.y - radius), ImVec2(centre.x + radius, centre.y + radius), ImVec2(0, 0), ImVec2(1, 1), true);

    if (push_texture_id)
        window->DrawList->PopTextureID();
}

bool ImGui::tab(const char* icoid, const char* icon, bool selected, const ImVec2& icon_pos)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(std::string(icoid + std::string(icoid)).c_str());
    const ImVec2 label_size = ImGui::CalcTextSize(icon);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = { 50, 30 };

    const ImRect bb(pos, ImVec2(pos.x + 60, pos.y + 30));
    ImGui::ItemSize(size, 0);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

    if (hovered || held)
        ImGui::SetMouseCursor(7);

    float t = selected ? 1.0f : 0.0f;
    float deltatime = 1.5f * ImGui::GetIO().DeltaTime;
    static std::map<ImGuiID, float> hover_animation;
    auto it_hover = hover_animation.find(id);
    if (it_hover == hover_animation.end())
    {
        hover_animation.insert({ id, 0.f });
        it_hover = hover_animation.find(id);
    }
    it_hover->second = ImClamp(it_hover->second + (0.2f * ImGui::GetIO().DeltaTime * (hovered || IsItemActive() ? 1.f : -1.f)), 0.0f, 0.15f);
    it_hover->second *= min(GetStyle().Alpha * 1.2, 1.f);

    static std::map<ImGuiID, float> filled_animation;
    auto it_filled = filled_animation.find(id);
    if (it_filled == filled_animation.end())
    {
        filled_animation.insert({ id, 0.f });
        it_filled = filled_animation.find(id);
    }
    it_filled->second = ImClamp(it_filled->second + (2.55f * ImGui::GetIO().DeltaTime * (selected ? 1.f : -1.0f)), it_hover->second, 1.f);
    it_filled->second *= min(GetStyle().Alpha * 1.2, 1.f);

    static std::map<ImGuiID, float> fill_animation;
    auto it_fill = fill_animation.find(id);
    if (it_fill == fill_animation.end())
    {
        fill_animation.insert({ id, 0.f });
        it_fill = fill_animation.find(id);
    }
    it_fill->second = ImClamp(it_filled->second + (1.75f * ImGui::GetIO().DeltaTime * (selected ? 1.f : -1.0f)), it_hover->second, 1.f);
    it_fill->second *= min(GetStyle().Alpha * 1.2, 1.f);

    ImVec4 text = ImLerp(ImVec4{ 87 / 255.f, 88 / 255.f, 88 / 255.f, GetStyle().Alpha }, ImVec4{ 200 / 255.f, 200 / 255.f, 200 / 255.f, it_filled->second }, it_filled->second);

    ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(bb.Min.x + 13, bb.Min.y), ImVec2(bb.Min.x + 53, bb.Max.y + 2), ImColor(30, 30, 30, int(255 * it_filled->second)), 5);
    ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(bb.Min.x + 59, bb.Min.y + 3.5), ImVec2(bb.Min.x + 61 + it_filled->second + 1, bb.Max.y - 3.5), ImColor(142, 169, 248, int(255 * it_filled->second)), ImDrawFlags_RoundCornersTop, 5);

    ImGui::GetForegroundDrawList()->AddText(ImVec2(bb.Min.x + 23, bb.Min.y + (15 - label_size.y / 2)) + icon_pos, ImColor(text), icon);

    return pressed;
}