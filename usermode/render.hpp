
#include "cache.hpp"
#include <D3D11.h>
#include <D3DX11core.h>
#include <D3DX11.h>
#include <D3DX11tex.h>
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_internal.h"
#include "ImGui.hpp"

#include "font.hpp"
#include "fortnitefont.h"
#include "lightfont.h"
#include "particles.hpp"
#include "menucol.h"
#include "Config.h"
#include "crypter.h"
#include "lazyyyy.hpp"
#include "fontawesome.h"
#include "driver.h"
#include "spoofing.hpp"
#include "fadeinout.hpp"
#include "hijack.hpp"

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.f);
HWND window_handle;
HWND windows = NULL;
HWND hwnd = NULL;
RECT GameRect = { NULL };

IDirect3D9Ex* p_Object = NULL;
static LPDIRECT3DDEVICE9 D3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9 TriBuf = NULL;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD ScreenCenterX;
DWORD ScreenCenterY;

ID3D11Device* d3d_device;
ID3D11DeviceContext* d3d_device_ctx;
IDXGISwapChain* d3d_swap_chain;
ID3D11RenderTargetView* d3d_render_target;
D3DPRESENT_PARAMETERS d3d_present_params;

void SetWindowToTarget()
{
	while (true)
	{
		if (windows)
		{
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetWindowRect(windows, &GameRect);
			screen_width = GameRect.right - GameRect.left;
			screen_height = GameRect.bottom - GameRect.top;
			DWORD dwStyle = GetWindowLong(windows, GWL_STYLE);

			if (dwStyle & WS_BORDER)
			{
				GameRect.top += 32;
				screen_height -= 39;
			}
			ScreenCenterX = screen_width / 2;
			ScreenCenterY = screen_height / 2;
			MoveWindow(window_handle, GameRect.left, GameRect.top, screen_width, screen_height, true);
		}
		else
		{
			exit(0);
		}
	}
}

class GradientLine {
public:

	static bool Render(ImVec2 size)
	{
		static ImColor gradient_colors[] =
		{
			//https://txwes.libguides.com/c.php?g=978475&p=7075536

			//Red
			ImColor(255, 0, 0),
			//Yellow
			ImColor(255,255,0),
			//Lime
			ImColor(0, 255, 0),
			//Cyan / Aqua
			ImColor(0, 255, 255),
			//Blue
			ImColor(0, 0, 255),
			//Magenta / Fuchsia
			ImColor(255, 0, 255),
			//Red
			ImColor(255, 0, 0)
		};

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2      screen_pos = ImGui::GetCursorScreenPos();

		static int pos = 0;

		if (size.x - pos == 0)
			pos = 0;
		else
			pos++;

		for (int i = 0; i < 6; ++i)
		{
			ImVec2 item_spacing = ImGui::GetStyle().ItemSpacing;

			auto render = [&](int displacement)
				{
					draw_list->AddRectFilledMultiColor
					(
						ImVec2((screen_pos.x - item_spacing.x - displacement) + (i) * (size.x / 6), (screen_pos.y - item_spacing.y)),
						ImVec2((screen_pos.x - item_spacing.x + (item_spacing.x * 2) - displacement) + (i + 1) * (size.x / 6), (screen_pos.y - item_spacing.y) + (size.y)),

						gradient_colors[i], gradient_colors[i + 1], gradient_colors[i + 1], gradient_colors[i]
					);
				};

			render((pos)); render((pos - size.x));
		}
		return true;
	}

	static bool RenderRB(ImVec2 size)
	{
		static ImColor gradient_colors[] =
		{
			ImColor(255, 0, 0),
			ImColor(20, 0, 0),
			ImColor(255, 0, 0),
		};

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2      screen_pos = ImGui::GetCursorScreenPos();

		static int pos = 0;

		if (size.x - pos == 0)
			pos = 0;
		else
			pos++;

		for (int i = 0; i < 4; ++i)
		{
			ImVec2 item_spacing = ImGui::GetStyle().ItemSpacing;

			auto render = [&](int displacement)
				{
					draw_list->AddRectFilledMultiColor
					(
						ImVec2((screen_pos.x - item_spacing.x - displacement) + (i) * (size.x / 2), (screen_pos.y - item_spacing.y)),
						ImVec2((screen_pos.x - item_spacing.x + (item_spacing.x * 2) - displacement) + (i + 1) * (size.x / 2), (screen_pos.y - item_spacing.y) + (size.y)),

						gradient_colors[i], gradient_colors[i + 1], gradient_colors[i + 1], gradient_colors[i]
					);
				};

			render((pos)); render((pos - size.x));
		}
		return true;
	}
};


void embraceTheDarkness()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Button] = ImVec4(0.2117647081613541f, 0.2117647081613541f, 0.2117647081613541f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.3991416096687317f, 0.3991376161575317f, 0.3991376161575317f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); 

    colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 0.9399999976158142f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
    colors[ImGuiCol_Border] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.1759656667709351f, 0.1759639084339142f, 0.1759639084339142f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(9.999899930335232e-07f, 9.999946541938698e-07f, 9.999999974752427e-07f, 0.4000000059604645f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.3862661123275757f, 0.3846083283424377f, 0.3846083283424377f, 1.0f);
    colors[ImGuiCol_TitleBg] = ImVec4(9.999999974752427e-07f, 9.999899930335232e-07f, 9.999936310123303e-07f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(9.999999974752427e-07f, 9.999899930335232e-07f, 9.99994995254383e-07f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.9999899864196777f, 0.999994695186615f, 1.0f, 1.0f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.2117647081613541f, 0.2117647081613541f, 0.2117647081613541f, 1.0f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.4034335017204285f, 0.4034294486045837f, 0.4034294486045837f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.2117647081613541f, 0.2117647081613541f, 0.2117647081613541f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.3991416096687317f, 0.3991376161575317f, 0.3991376161575317f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(255.0f, 255.0f, 255.0f, 1.0f);
    colors[ImGuiCol_Header] = ImVec4(0.4334720373153687f, 0.4334741532802582f, 0.4334763884544373f, 0.3100000023841858f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(9.999899930335232e-07f, 9.999946541938698e-07f, 9.999999974752427e-07f, 0.800000011920929f);
    colors[ImGuiCol_HeaderActive] = ImVec4(9.999899930335232e-07f, 9.999945405070321e-07f, 9.999999974752427e-07f, 1.0f);
    colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.2117647081613541f, 0.2117647081613541f, 0.2117647081613541f, 1.0f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.2117647081613541f, 0.2117647081613541f, 0.2117647081613541f, 1.0f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.2000000029802322f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.6700000166893005f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.949999988079071f);
    colors[ImGuiCol_Tab] = ImVec4(0.304718017578125f, 0.3047195076942444f, 0.3047210574150085f, 0.8619999885559082f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.124462254345417f, 0.1244628354907036f, 0.1244634985923767f, 0.800000011920929f);
    colors[ImGuiCol_TabActive] = ImVec4(0.2117647081613541f, 0.2117647081613541f, 0.2117647081613541f, 1.0f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.4849785566329956f, 0.4849736988544464f, 0.4849742352962494f, 1.0f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2117647081613541f, 0.2117647081613541f, 0.2117647081613541f, 1.0f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);

	ImGuiStyle& style = ImGui::GetStyle();



	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 4.0f;
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.Alpha = 1.0f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 0.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.ChildRounding = 7.5f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 0.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f, 3.0f);
	style.FrameRounding = 7.300000190734863f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 14.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 4.0f;
	style.TabRounding = 4.0f;
	style.TabBorderSize = 0.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
}


namespace n_render {
	class c_render {
	public:
		auto imgui() -> bool {
			DXGI_SWAP_CHAIN_DESC swap_chain_description;
			ZeroMemory(&swap_chain_description, sizeof(swap_chain_description));
			swap_chain_description.BufferCount = 2;
			swap_chain_description.BufferDesc.Width = 0;
			swap_chain_description.BufferDesc.Height = 0;
			swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swap_chain_description.BufferDesc.RefreshRate.Numerator = 60;
			swap_chain_description.BufferDesc.RefreshRate.Denominator = 1;
			swap_chain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swap_chain_description.OutputWindow = window_handle;
			swap_chain_description.SampleDesc.Count = 1;
			swap_chain_description.SampleDesc.Quality = 0;
			swap_chain_description.Windowed = 1;
			swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			D3D_FEATURE_LEVEL d3d_feature_lvl;

			const D3D_FEATURE_LEVEL d3d_feature_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

			D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, d3d_feature_array, 2, D3D11_SDK_VERSION, &swap_chain_description, &d3d_swap_chain, &d3d_device, &d3d_feature_lvl, &d3d_device_ctx);

			ID3D11Texture2D* pBackBuffer;
			D3DX11_IMAGE_LOAD_INFO info;
			ID3DX11ThreadPump* pump{ nullptr };

			d3d_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

			d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &d3d_render_target);

			pBackBuffer->Release();

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
			ImFontConfig icons_config;

			ImFontConfig CustomFont;
			CustomFont.FontDataOwnedByAtlas = false;

			icons_config.MergeMode = true;
			icons_config.PixelSnapH = true;
			icons_config.OversampleH = 3;
			icons_config.OversampleV = 3;

			ImGui_ImplWin32_Init(window_handle);
			ImGui_ImplDX11_Init(d3d_device, d3d_device_ctx);

			ImFontConfig font_config;
			font_config.OversampleH = 1;
			font_config.OversampleV = 1;
			font_config.PixelSnapH = 1;

			static const ImWchar ranges[] =
			{
				0x0020, 0x00FF,
				0x0400, 0x044F,
				0,
			};

			MenuFont = io.Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\Bahnschrift.ttf"), 18.f);
			io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 16.5f, &icons_config, icons_ranges);
			io.Fonts->AddFontDefault();
			GameFont = io.Fonts->AddFontFromMemoryTTF(font, sizeof(font), 15.f);
			MenuFont2 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Bahnschrift.ttf", 13.f); //11 before
			MenuFont3 = io.Fonts->AddFontFromMemoryTTF(&LightFont, sizeof LightFont, 15.f);
			
			FortniteFont = io.Fonts->AddFontFromMemoryTTF(FnFont, sizeof(FnFont), 17.f);

			ImGui_ImplWin32_Init(window_handle);

			ImGui_ImplDX11_Init(d3d_device, d3d_device_ctx);

			d3d_device->Release();

			return true;
		}

		auto hijack() -> bool
		{
			SPOOF;
			while (!window_handle) {
				window_handle = SetUp::HiJackNotepadWindow(); //working you can play w it well but swap if possible
				Sleep(10);
			}

			MARGINS margin = { -1 };
			DwmExtendFrameIntoClientArea(window_handle, &margin);
			SetMenu(window_handle, NULL);
			SetWindowLongPtr(window_handle, GWL_STYLE, WS_VISIBLE);
			SetWindowLongPtr(window_handle, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);

			ShowWindow(window_handle, SW_SHOW);
			UpdateWindow(window_handle);

			SetWindowLong(window_handle, GWL_EXSTYLE, GetWindowLong(window_handle, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);

			return true;
		}

		void AlignForWidth(float width, float alignment = 0.5f)
		{
			ImGuiStyle& style = ImGui::GetStyle();
			float avail = ImGui::GetContentRegionAvail().x;
			float off = (avail - width) * alignment;
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
		}

		auto menu() -> void {
			struct tab
			{
				const char* tab_name;
				int tab_id;
			};

			static int tabs = 0;
			static int settingtab = 0;
			static int current_tab = 0;

			if (GetAsyncKeyState(VK_INSERT) & 1) {
				globals::g_render_menu = !globals::g_render_menu;
			}


	

			if (globals::g_render_menu) {
				ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y), ImGuiCond_Once);
				ImGui::SetNextWindowSize(ImVec2(window_size.x, window_size.y), ImGuiCond_Once);
				ImGui::SetNextWindowBgAlpha(1.0f);

				if (globals::menucrosshair)
				{
					ImVec2 mousePos = ImVec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
					float size = 4.0f;
					ImVec2 squareMin = ImVec2(mousePos.x - size, mousePos.y - size);
					ImVec2 squareMax = ImVec2(mousePos.x + size, mousePos.y + size);
					ImGui::GetForegroundDrawList()->AddRectFilled(squareMin, squareMax, IM_COL32(255, 255, 255, 255));
				}

				ImGui::Begin(verucryptt(""), NULL, window_flags);
				{
					float old = ImGui::GetFont()->Scale; ImGui::GetFont()->Scale = 1.2; ImGui::PushFont(ImGui::GetFont()); 
					ImGui::SetCursorPos(ImVec2(12, 3));
					ImGui::TextColored(ImVec4(10, 10, 10, 200), "ndyjan x vatos");
					ImGui::GetFont()->Scale = old; ImGui::PopFont(); ImGui::GetFont()->Scale = old;
					ImGui::SetCursorPos(ImVec2(5, 32));
					GradientLine::RenderRB(ImVec2(ImGui::GetContentRegionAvail().x * 2, 3));


					ImVec2 pos = ImGui::GetWindowPos();
					//-=-=-=-=-TABS-=-=-=-=-

					ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + 5, pos.y + 35), ImVec2(pos.x + 105, pos.y + 65), ImColor(39, 39, 39, 200));
					ImGui::SetCursorPos(ImVec2(5, 35));
					if (ImGui::Button(verucryptt("Combat"), ImVec2(100, 30)))
					{
						tabs = 0;
					}
					ImGui::SameLine();
					ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + 5, pos.y + 70), ImVec2(pos.x + 105, pos.y + 100), ImColor(39, 39, 39, 200));
					ImGui::SetCursorPos(ImVec2(5, 70));
					if (ImGui::Button(verucryptt("Visuals"), ImVec2(100, 30)))
					{
						tabs = 1;
					}
					ImGui::SameLine();
					ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + 5, pos.y + 105), ImVec2(pos.x + 105, pos.y + 135), ImColor(39, 39, 39, 200));
					ImGui::SetCursorPos(ImVec2(5, 105));
					if (ImGui::Button(verucryptt("Misc"), ImVec2(100, 30)))
					{
						tabs = 2;
					}
					ImGui::SameLine();
					ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x + 5, pos.y + 140), ImVec2(pos.x + 105, pos.y + 170), ImColor(39, 39, 39, 200));
					ImGui::SetCursorPos(ImVec2(5, 140));
					if (ImGui::Button(verucryptt("Other"), ImVec2(100, 30)))
					{
						tabs = 3;
					}


					//=-=-=-=--Features Tabs-=-=-=-=-=-
					//550, 430

					ImGui::SetCursorPos(ImVec2(110, 40));
					ImGui::BeginChild("##Main", ImVec2(435, 385));
					{
						if (tabs == 0) {
							ImGui::Text(verucryptt(ICON_FA_CROSSHAIRS" Combat"));

							ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f));
							ImGui::Separator();
							ImGui::PopStyleColor();
							 
							ImGui::Checkbox(verucryptt("Aimbot"), &globals::g_aimbot); 		
							ImGui::SameLine();
							ImGui::SetNextItemWidth(140.f);
							ImGui::Combo(verucryptt(("##Bones")), &globals::g_hitbox, ("Head\0\Neck\0\Chest\0\Random"));

							ImGui::Checkbox(verucryptt("Prediction"), &globals::prediction);
							ImGui::Checkbox(verucryptt("Draw Fov"), &globals::g_render_fov);
							ImGui::Checkbox(verucryptt("Visible Check"), &globals::g_visible_check);
							ImGui::Checkbox(verucryptt("Target Text"), &globals::g_target_text);


							float MaxSmooth = 20; if (!globals::vsync) { MaxSmooth = 100; }
							ImGui::SetNextItemWidth(190.f);
							ImGui::SliderFloat(verucryptt("##dupson"), &globals::g_smooth, 5, MaxSmooth, "%.0f");
							ImGui::SameLine();
							ImGui::Text(verucryptt("Smooth"));

							ImGui::SetNextItemWidth(190.f);
							ImGui::SliderFloat(verucryptt("##dupaa"), &globals::g_aimfov, 10, 300, "%.0f");
							ImGui::SameLine();
							ImGui::Text(verucryptt("Fov"));

							ImGui::Text(verucryptt("Aimbot Key"));
							ImGui::SameLine();

							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(255.0f, 255.0f, 255.0f, 1.0f));
							HotkeyButton(aimkey, ChangeKey, keystatus);
							ImGui::PopStyleColor(3);
						}
						else if (tabs == 1) {
							ImGui::Text(verucryptt(ICON_FA_EYE" Visuals"));

							ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f));
							ImGui::Separator();
							ImGui::PopStyleColor();

							ImGui::Checkbox(verucryptt("Boxes"), &globals::g_box_esp);
							ImGui::SameLine();


							ImGui::Checkbox(verucryptt("Skeleton "), &globals::g_skeleton);
							ImGui::SameLine();
							ImGui::Checkbox(verucryptt("Head"), &globals::headesp);
							ImGui::SameLine();
							ImGui::Checkbox(verucryptt("Enable Outline"), &globals::g_outline);



							ImGui::Checkbox(verucryptt("Platform "), &globals::g_platform);
							ImGui::Checkbox(verucryptt("Ranked Esp"), &globals::g_ranked);
							ImGui::Checkbox(verucryptt("Name"), &globals::g_username);

							ImGui::Checkbox(verucryptt("Snaplines"), &globals::g_line);
							ImGui::SameLine();
							ImGui::SetNextItemWidth(120.f);
							ImGui::Combo(verucryptt(("##LineType")), &globals::g_line_type, ("Top\0\Middle\0\Bottom\0"));		

							ImGui::Checkbox(verucryptt("Distance"), &globals::g_distance);

													
							ImGui::SliderFloat(verucryptt("##DistanceSlider"), &globals::g_render_distance, 10.f, 275.f, "%.1fM");
							ImGui::SameLine();
							ImGui::Text(verucryptt("Max Distance"));

							ImGui::SliderFloat(verucryptt("##BoxThicknes"), &globals::g_box_thick, 1, 5);
							ImGui::SameLine();
							ImGui::Text(verucryptt("Box Thickness"));
							
							ImGui::SliderInt(verucryptt("##Skeleton Thick"), &globals::g_skeletonthickness, 1, 5);
							ImGui::SameLine();
							ImGui::Text(verucryptt("Skeleton Thickness"));

							ImGui::SliderInt(verucryptt("##SnapLineThicknes"), &globals::g_line_thick, 1, 5);
							ImGui::SameLine();
							ImGui::Text(verucryptt("Snaplines Thick"));


						}
						else if (tabs == 2) {
							ImGui::Text(verucryptt(ICON_FA_SERVER" Misc"));

							ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f));
							ImGui::Separator();
							ImGui::PopStyleColor();

							ImGui::Checkbox(verucryptt("Enable Vsync"), &globals::vsync);
							ImGui::Checkbox(verucryptt("Enable Menu Crosshair"), &globals::menucrosshair);

							ImGui::Checkbox(verucryptt("Enable Watermark"), &globals::g_watermark);

							ImGui::Checkbox(verucryptt("Enable Crosshair"), &globals::crosshair);
							ImGui::ColorEdit3(verucryptt("Crosshair Color"), &globals::crosshair_color.x);
							ImGui::SliderFloat(verucryptt("Crosshair Size"), &globals::crosshair_size, 1, 5);
							ImGui::SliderFloat(verucryptt("Crosshair Length"), &globals::crosshair_thickness, 1, 7);
							ImGui::SliderFloat(verucryptt("Crosshair Thickness"), &globals::crosshair_thickness, 1, 10);


							ImGui::SliderFloat(verucryptt("Font Size"), &globals::g_font_size, 15, 30);

							ImGui::SliderFloat(verucryptt("Entity Distance"), &globals::entitydistance, 50, 250);


						}
						else if (tabs == 3) {
							ImGui::Text(verucryptt(ICON_FA_SERVER" Other"));

						}
					}
					ImGui::EndChild();

					ImGui::End();
				}
			}
		}

		auto draw() -> void {
			SPOOF;
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.f);

			embraceTheDarkness();

			md::FadeInOut fade_in_out;
			fade_in_out.init();

			md::FadeInOut heartbeat;
			heartbeat.init();


			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			g_main->actor_loop();
			//g_loop::LevelDrawing();

			//menu();

			ImGui::Render();
			const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			d3d_device_ctx->OMSetRenderTargets(1, &d3d_render_target, nullptr);
			d3d_device_ctx->ClearRenderTargetView(d3d_render_target, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			SetWindowPos(window_handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

			if (globals::vsync)
			{
				d3d_swap_chain->Present(1, 0);
			}
			else
			{
				d3d_swap_chain->Present(0, 0);
			}
		}

		auto render() -> bool {static 
			IDXGISwapChain* pSwapChain;
			IDXGISwapChain* pSwaChain = nullptr;
			static RECT rect_og;
			MSG msg = { NULL };
			ZeroMemory(&msg, sizeof(MSG));

			SPOOF;

			while (msg.message != WM_QUIT)
			{
				UpdateWindow(window_handle);
				ShowWindow(window_handle, SW_SHOW);

				if (PeekMessageA(&msg, window_handle, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				ImGuiIO& io = ImGui::GetIO();
				io.ImeWindowHandle = window_handle;
				io.DeltaTime = 1.0f / 60.0f;

				POINT p_cursor;
				GetCursorPos(&p_cursor);
				io.MousePos.x = p_cursor.x;
				io.MousePos.y = p_cursor.y;

				if (GetAsyncKeyState(VK_LBUTTON)) {
					io.MouseDown[0] = true;
					io.MouseClicked[0] = true;
					io.MouseClickedPos[0].x = io.MousePos.x;
					io.MouseClickedPos[0].x = io.MousePos.y;
				}
				else
					io.MouseDown[0] = false;

				draw();
			}
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();

			DestroyWindow(window_handle);


			return true;
		}

		void Overlay()
		{
		    hijack();

			imgui();

			render();
		};
	};
} static n_render::c_render* g_render = new n_render::c_render();