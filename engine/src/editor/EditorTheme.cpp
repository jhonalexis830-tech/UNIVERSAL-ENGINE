#include "ue/editor/EditorTheme.hpp"

#include <imgui.h>

namespace ue::editor {

namespace {
    ImVec4 cv(float rgb[4]) { return ImVec4(rgb[0], rgb[1], rgb[2], rgb[3]); }
}

const EditorTheme::Colors& EditorTheme::colors() {
    static const Colors c;
    return c;
}

void EditorTheme::applyStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    const Colors& c = colors();

    style.WindowRounding    = 6.0f;
    style.ChildRounding     = 4.0f;
    style.FrameRounding     = 4.0f;
    style.PopupRounding     = 6.0f;
    style.GrabRounding      = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.TabRounding       = 4.0f;
    style.WindowPadding     = ImVec2(10, 10);
    style.FramePadding      = ImVec2(6, 4);
    style.ItemSpacing       = ImVec2(8, 6);
    style.WindowBorderSize  = 1.0f;
    style.FrameBorderSize   = 0.0f;

    ImVec4* col = style.Colors;
    col[ImGuiCol_WindowBg]          = cv(c.panelBackground);
    col[ImGuiCol_ChildBg]           = cv(c.panelDark);
    col[ImGuiCol_PopupBg]           = cv(c.deepBlack);
    col[ImGuiCol_Border]            = ImVec4(0.12f, 0.20f, 0.34f, 0.9f);
    col[ImGuiCol_BorderShadow]      = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    col[ImGuiCol_TitleBg]           = cv(c.panelDark);
    col[ImGuiCol_TitleBgActive]     = cv(c.metallicBlue);
    col[ImGuiCol_TitleBgCollapsed]  = cv(c.panelDark);
    col[ImGuiCol_MenuBarBg]         = cv(c.deepBlack);
    col[ImGuiCol_Text]              = cv(c.text);
    col[ImGuiCol_TextDisabled]      = cv(c.textMuted);
    col[ImGuiCol_CheckMark]         = cv(c.brightBlue);
    col[ImGuiCol_Header]            = cv(c.selection);
    col[ImGuiCol_HeaderHovered]     = ImVec4(0.25f, 0.42f, 0.70f, 0.45f);
    col[ImGuiCol_HeaderActive]      = cv(c.metallicBlue);
    col[ImGuiCol_FrameBg]           = cv(c.frameBackground);
    col[ImGuiCol_FrameBgHovered]    = ImVec4(0.12f, 0.18f, 0.28f, 1.0f);
    col[ImGuiCol_FrameBgActive]     = ImVec4(0.16f, 0.24f, 0.36f, 1.0f);
    col[ImGuiCol_Button]            = cv(c.metallicBlue);
    col[ImGuiCol_ButtonHovered]     = cv(c.steelBlue);
    col[ImGuiCol_ButtonActive]      = cv(c.brightBlue);
    col[ImGuiCol_SliderGrab]        = cv(c.steelBlue);
    col[ImGuiCol_SliderGrabActive]  = cv(c.brightBlue);
    col[ImGuiCol_Separator]         = ImVec4(0.13f, 0.21f, 0.36f, 0.8f);
    col[ImGuiCol_SeparatorHovered]  = cv(c.brightBlue);
    col[ImGuiCol_SeparatorActive]   = cv(c.brightBlue);
    col[ImGuiCol_Tab]               = cv(c.panelDark);
    col[ImGuiCol_TabHovered]        = cv(c.steelBlue);
    col[ImGuiCol_TabActive]         = cv(c.metallicBlue);
    col[ImGuiCol_TabUnfocused]      = cv(c.panelDark);
    col[ImGuiCol_TabUnfocusedActive]= cv(c.metallicBlue);
    col[ImGuiCol_ScrollbarBg]       = cv(c.deepBlack);
    col[ImGuiCol_ScrollbarGrab]     = cv(c.metallicBlue);
    col[ImGuiCol_ScrollbarGrabHovered] = cv(c.steelBlue);
    col[ImGuiCol_ScrollbarGrabActive]  = cv(c.brightBlue);
}

void EditorTheme::loadDefaultFont() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    const std::vector<std::pair<const char*, float>> candidates = {
        { "C:/Windows/Fonts/segoeui.ttf", 16.0f },
        { "C:/Windows/Fonts/arial.ttf", 16.0f },
        { "C:/Windows/Fonts/tahoma.ttf", 16.0f },
    };
    for (const auto& [path, size] : candidates) {
        if (io.Fonts->AddFontFromFileTTF(path, size) != nullptr) {
            break;
        }
    }
}

} // namespace ue::editor