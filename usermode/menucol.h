#pragma once
#include "ImGui/imgui.h"

struct Color
{
    float r, g, b, a;
    Color()
    {
        r = 1.f, g = 1.f, b = 1.f, a = 1.f;
    }
    Color(float _r, float _g, float _b, float _a)
    {
        this->r = _r;
        this->g = _g;
        this->b = _b;
        this->a = _a;
    }
    ImVec4 x()
    {
        return ImVec4(r, g, b, a);
    }
    ImU32 c()
    {
        return IM_COL32(r * 255.f, g * 255.f, b * 255.f, a * 255.f);
    }
    ImU32 c(int m)
    {
        return IM_COL32((r * 255.f) - m, (g * 255.f) - m, (b * 255.f) - m, (a * 255.f) - m);
    }
};

Color AccentColor = Color{ 209 / 255.f, 165 / 255.f, 255 / 255.f, 1.f };
Color AccentColor2 = Color{ 255 / 255.f, 0 / 255.f, 255 / 255.f, 1.f };


Color SecondAccentColor = Color{ 69 / 255.f, 140 / 255.f, 247 / 255.f, 1.f };

ImVec2 window_pos{ 0, 0 };
ImVec2 window_size{ 550, 430 };



DWORD window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize;
DWORD Settings_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize;