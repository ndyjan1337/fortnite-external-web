#pragma once
#include "imgui/imgui.h"
#include <dxgi.h>
#include <string>

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

namespace Globals
{
	inline bool showMenu = false;
	inline Present oPresent;
	inline ImGuiStyle* style;
	inline std::string window = "main";
}

namespace Config
{
	inline bool checkbox_State = false;
	inline int slider_Int = 2;
	inline float slider_Float = 1.5f;
	inline int slider_TripleInt[3] = { 1, 2, 3 };
	inline std::string dropdown_Selection = "0";
	inline std::string text_Input;
}
