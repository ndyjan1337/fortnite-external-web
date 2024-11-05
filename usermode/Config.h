#pragma once

namespace ImGui
{
    inline bool custom_UseFontShadow;
    inline unsigned int custom_FontShadowColor;
    inline bool FlaggedItem = false;

    inline static void PushFontShadow(unsigned int col)
    {
        custom_UseFontShadow = true;
        custom_FontShadowColor = col;
    }

    inline static void PopFontShadow(void)
    {
        custom_UseFontShadow = false;
    }

    inline static void PushFlagged()
    {
        FlaggedItem = true;
    }

    inline static void PopFlagged()
    {
        FlaggedItem = false;
    }

}; // namespace ImGui