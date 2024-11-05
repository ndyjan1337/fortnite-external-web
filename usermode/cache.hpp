#include "cache_helper.hpp"
#include <iomanip>
#include "triggerbothelper.h"
#include "xorst.h"
#include "crypter.h"
#include "ex.hpp"
#include "spoofing.hpp"

std::wstring string_to_wstring(const std::string& str) {
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
	return wstr;
}

std::string wstring_to_utf8(const std::wstring& wstr) {
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
	return str;
}


struct WeaponInformation
{
	int32_t ammo_count;
	int32_t max_ammo;

	BYTE tier;
	std::string weapon_name;
	std::string buildplan;
};
std::string LocalPlayerWeapon;
WeaponInformation WeaponInfo;
int InFovEnemy = 0;
int visennemy = 0;

__int64 TargetedBuild;
bool bTargetedBuild;

bool ShowRadar = true;
bool rect_radar = true;
float radar_position_x{ 10.0f };
float radar_position_y{ 10.0f };
float radar_size{ 150.0f };
float RadarDistance = { 50.f };
float downed[1];



namespace Offset
{
	namespace AFortWeapon
	{
		auto bIsReloadingWeapon = 0x388;
		auto Ammo = 0xe54;
	}

	namespace UFortMovementComp_Character
	{
		auto bWantsToSwing = 0x14bc;
		auto SwingAttachLocation = 0x14c0;
		auto bSwingInstantVelocity = 0x14bf;
		auto SwingLaunch = 0x14d8;

		auto bWantsToSkate = 0x5511;
		auto acceleration = 0x58;
	}
}

void CalcRadarPoint(fvector vOrigin, int& screenx, int& screeny)
{
	ue5->get_camera();
	fvector vAngle = camera_postion.rotation;
	auto fYaw = vAngle.y * M_PI / 180.0f;
	float dx = vOrigin.x - camera_postion.location.x;
	float dy = vOrigin.y - camera_postion.location.y;

	float fsin_yaw = sinf(fYaw);
	float fminus_cos_yaw = -cosf(fYaw);

	float x = dy * fminus_cos_yaw + dx * fsin_yaw;
	x = -x;
	float y = dx * fminus_cos_yaw - dy * fsin_yaw;

	float range = (float)RadarDistance * 1000.f;

	ue5->RadarRange(&x, &y, range);

	ImVec2 DrawPos = ImVec2(radar_position_x, radar_position_y);
	ImVec2 DrawSize = ImVec2(radar_size, radar_size);


	int rad_x = (int)DrawPos.x;
	int rad_y = (int)DrawPos.y;

	float r_siz_x = DrawSize.x;
	float r_siz_y = DrawSize.y;

	int x_max = (int)r_siz_x + rad_x - 5;
	int y_max = (int)r_siz_y + rad_y - 5;

	screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * r_siz_x));
	screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * r_siz_y));

	if (screenx > x_max)
		screenx = x_max;

	if (screenx < rad_x)
		screenx = rad_x;

	if (screeny > y_max)
		screeny = y_max;

	if (screeny < rad_y)
		screeny = rad_y;
}

void fortnite_radar(float x, float y, float size, bool rect = false)
{
	if (ShowRadar)
	{
		switch (globals::RadarType)
		{
		case 0:
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Once);
			static const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
			ImGui::Begin(("##radar"), nullptr, flags);

			float render_size = 5;
			ImDrawList* drawList = ImGui::GetWindowDrawList();

			// Draw the outline of a white circle
			drawList->AddCircle(ImVec2(x + size / 2, y + size / 2), size / 2, ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 1.0f }), 1000);

			// Optionally, draw a smaller circle at the center
			drawList->AddCircleFilled(ImVec2(x + size / 2, y + size / 2), render_size / 2, ImGui::GetColorU32({ globals::g_color[0], globals::g_color[1], globals::g_color[2], 1.0f }), 1000);

			ImGui::End();
			break;
		}
		case 1:
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Once);
			static const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
			ImGui::Begin(("##radar"), nullptr, flags);

			float render_size = 5;
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + size, y + size), ImGui::GetColorU32({ 0.43f, 0.43f, 0.43f, 1.f }), 0.0f, 0);
			drawList->AddCircleFilled(ImVec2(x + size / 2, y + size / 2), render_size / 2 * sqrt(2), ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.0f }), 1000);
			//  drawList->AddCircle(ImVec2(x + size / 2, y + size / 2), size, ImGui::GetColorU32({ 0.1f, 0.1f, 0.1f, 1.0f }), 1000, 5);
			ImGui::End();
			break;
		}
		}
	}
}

void DrawArrow(float x, float y, float angle, float size, ImU32 color) {
	float arrow_half_size = size / 2.0f;

	ImVec2 points[3];
	points[0] = ImVec2(x + cosf(angle) * size, y + sinf(angle) * size);
	points[1] = ImVec2(x + cosf(angle + 1.5f) * arrow_half_size, y + sinf(angle + 1.5f) * arrow_half_size);
	points[2] = ImVec2(x + cosf(angle - 1.5f) * arrow_half_size, y + sinf(angle - 1.5f) * arrow_half_size);

	//ImGui::GetBackgroundDrawList()->AddTriangleFilled(points[0], points[1], points[2], color);
	ImGui::GetBackgroundDrawList()->AddLine(points[0], points[1], color, 1.0f);
	ImGui::GetBackgroundDrawList()->AddLine(points[1], points[2], color, 1.0f);
	ImGui::GetBackgroundDrawList()->AddLine(points[2], points[0], color, 1.0f);
}

static auto is_visible(uintptr_t skeletal_mesh) -> bool {

	auto last_submit = read <float>(skeletal_mesh + 0x2E8);
	auto last_render = read <float>(skeletal_mesh + 0x2F0);
	return (bool)(last_render + 0.06f >= last_submit);



}

bool is_vis = is_visible(skeletal_mesh);

void oof_arrow(const fvector& vOrigin, bool is_vis) {
    fvector vAngle = camera::rotation;
    float fYaw = vAngle.y * PI / 180.0f;
    float dx = vOrigin.x - camera::location.x;
    float dy = vOrigin.y - camera::location.y;

    float fsin_yaw = sinf(fYaw);
    float fminus_cos_yaw = -cosf(fYaw);

    float x = dy * fminus_cos_yaw + dx * fsin_yaw;
    x = -x;
    float y = dx * fminus_cos_yaw - dy * fsin_yaw;

    float length = sqrtf(x * x + y * y);
    x /= length;
    y /= length;

    float angle = atan2f(y, x);
    float fov_radius = globals::g_aimfov + 8.0f; // edge it


    float screen_center_x = screen_width / 2.0f;
    float screen_center_y = screen_height / 2.0f;

    fvector2d arrow_pos = { screen_center_x + x * fov_radius, screen_center_y + y * fov_radius };

    ImU32 color = is_vis ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255);

    DrawArrow(arrow_pos.x,arrow_pos.y, angle, 15.0f, color);
}

void DrawFilledESP(int x, int y, int w, int h, float alpha, int thickness) {
	SPOOF;

	// Calculate the number of gradient steps based on the width
	int steps = w;

	// Loop through each step and draw a rectangle with the corresponding color
	for (int i = 0; i < steps; i++) {
		float t = static_cast<float>(i) / static_cast<float>(steps - 1); // Normalized value between 0 and 1
		float r = 1.0f;
		float g = 1.0f;
		float b = 1.0f;
		float a = t * alpha * 0.20f; // Alpha gradient from 0 to the specified alpha

		ImU32 converted_color = ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));

		ImGui::GetBackgroundDrawList()->AddRectFilled(
			ImVec2(x + i, y), ImVec2(x + i + 1, y + h), converted_color, 0.0f, 0
		);
	}

	// Draw border if needed
	if (thickness > 0) {
		ImU32 border_color = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, alpha));
		ImGui::GetBackgroundDrawList()->AddRect(
			ImVec2(x, y), ImVec2(x + w, y + h), border_color, 0.0f, ImDrawFlags_None, thickness
		);
	}
}


void add_players_radar(fvector WorldLocation)
{
	if (ShowRadar)
	{
		static const auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
		ImGui::Begin(("##radar"), nullptr, flags);

		int ScreenX, ScreenY = 0;
		const auto& GetWindowDrawList = ImGui::GetWindowDrawList();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		CalcRadarPoint(WorldLocation, ScreenX, ScreenY);
		if (globals::g_visible_check) {
			drawList->AddCircleFilled(ImVec2(ScreenX, ScreenY), 5, ImGui::GetColorU32({ globals::g_color_Visible[0],globals::g_color_Visible[1],globals::g_color_Visible[2],  1.0f }), 7);
		}
		else {
			drawList->AddCircleFilled(ImVec2(ScreenX, ScreenY), 5, ImGui::GetColorU32({ globals::g_color_invisible[0],globals::g_color_invisible[1],globals::g_color_invisible[2],  1.0f }), 7);
		}


		ImGui::End();
	}
}

std::chrono::time_point<std::chrono::steady_clock> last_update_time = std::chrono::steady_clock::now();
char fps[64] = "0";
void updateFPS() {
	auto now = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update_time).count();

	if (duration >= 1000) {
		sprintf_s(fps, sizeof(fps), "%.0f", ImGui::GetIO().Framerate);
		last_update_time = now;

		//std::cout << "FPS Updated: " << fps << std::endl;
	}
}

namespace g_loop {
	class g_fn {
	public:

		auto actor_loop() -> void {
			//ImGui::PushFont(GameFont);
			ImGui::PushFont(FortniteFont);

			ue5->get_camera();

			ImDrawList* draw_list = ImGui::GetForegroundDrawList();
			float target_dist = FLT_MAX;
			uintptr_t target_entity = 0;

			if (globals::g_watermark) {
				updateFPS();

				DrawString(globals::g_font_size, 10, 10, ImColor(255, 255, 255, 255), false, true, "ndyjan X vatos");
				DrawString(globals::g_font_size, 10, 30, ImColor(255, 255, 255, 255), false, true, fps);
			}

			if (globals::g_render_fov) {
				//ImGui::GetForegroundDrawList()->AddCircle(ImVec2(screen_width / 2, screen_height / 2), globals::g_aimfov, ImColor(0, 0, 0), 64, 3); //outline

				ImGui::GetForegroundDrawList()->AddCircle(ImVec2(screen_width / 2, screen_height / 2), globals::g_aimfov, ImColor(255, 255, 255), 64, 1);
			}

			if (globals::crosshair)
			{
				ImVec2 center(screen_width / 2, screen_height / 2);

				// Convert ImVec4 color to ImU32
				ImU32 crosshair_color_u32 = ImGui::ColorConvertFloat4ToU32(globals::crosshair_color);

				// Main crosshair lines
				draw_list->AddLine(ImVec2(center.x - globals::crosshair_size, center.y), ImVec2(center.x - globals::crosshair_length, center.y), crosshair_color_u32, globals::crosshair_thickness);
				draw_list->AddLine(ImVec2(center.x + globals::crosshair_size, center.y), ImVec2(center.x + globals::crosshair_length, center.y), crosshair_color_u32, globals::crosshair_thickness);
				draw_list->AddLine(ImVec2(center.x, center.y - globals::crosshair_size), ImVec2(center.x, center.y - globals::crosshair_length), crosshair_color_u32, globals::crosshair_thickness);
				draw_list->AddLine(ImVec2(center.x, center.y + globals::crosshair_size), ImVec2(center.x, center.y + globals::crosshair_length), crosshair_color_u32, globals::crosshair_thickness);

				// Secondary crosshair lines
				draw_list->AddLine(ImVec2(center.x - globals::crosshair_size - 1, center.y), ImVec2(center.x - globals::crosshair_length + 1, center.y), crosshair_color_u32, globals::crosshair_thickness - 1);
				draw_list->AddLine(ImVec2(center.x + globals::crosshair_size + 1, center.y), ImVec2(center.x + globals::crosshair_length - 1, center.y), crosshair_color_u32, globals::crosshair_thickness - 1);
				draw_list->AddLine(ImVec2(center.x, center.y - globals::crosshair_size - 1), ImVec2(center.x, center.y - globals::crosshair_length + 1), crosshair_color_u32, globals::crosshair_thickness - 1);
				draw_list->AddLine(ImVec2(center.x, center.y + globals::crosshair_size + 1), ImVec2(center.x, center.y + globals::crosshair_length - 1), crosshair_color_u32, globals::crosshair_thickness - 1);
			}

			const float centerWidth = screen_width / 2;
			const float centerHeight = screen_height / 2;

			for (auto& cached : entity_list) {
				auto root_bone = ue5->Bone(cached.skeletal_mesh, bone::Root);
				root = ue5->w2s(root_bone);

				root_box = ue5->w2s(fvector(root_bone.x, root_bone.y, root_bone.z + 30));

				root_box1 = ue5->w2s(fvector(root_bone.x, root_bone.y, root_bone.z - 15));

				auto head_bone = ue5->Bone(cached.skeletal_mesh, bone::Head);
				head = ue5->w2s(head_bone);
				head_box = ue5->w2s(fvector(head_bone.x, head_bone.y, head_bone.z + 15));

				float box_height = abs(head.y - root_box1.y);
				float box_width = box_height * 0.38f;
				float distance = arrays->relative_location.distance(root_bone) / 100;

				auto pawn_private1 = read<uintptr_t>(player_state + 0x308);

				if (distance > globals::g_render_distance && arrays->acknowledged_pawn) continue;

				if (globals::g_aimbot) {
					auto dx = head.x - (screen_width / 2);
					auto dy = head.y - (screen_height / 2);
					auto dist = sqrtf(dx * dx + dy * dy);

					if (globals::g_visible_check) {
						if (ue5->is_visible(cached.skeletal_mesh)) {
							if (dist < globals::g_aimfov && dist < target_dist) {
								target_dist = dist;
								target_entity = cached.entity;
							}
						}
					}
					else {
						if (dist < globals::g_aimfov && dist < target_dist) {
							target_dist = dist;
							target_entity = cached.entity;
						}
					}
				}

				int visibleMeshes = 0;
				int nonVisibleMeshes = 0;
				ImVec2 countPosition = ImVec2(10, 10);
				std::string visibleCountStr = std::string(verucryptt("Visible Entities: ")) + std::to_string(visibleMeshes);
				ImVec2 visibleTextSize = ImGui::CalcTextSize(visibleCountStr.c_str());
				if (arrays->aactor != arrays->acknowledged_pawn)
				{
					visibleMeshes++;
				}
				int playersWithin50m = 0;

				if (globals::nearbyplayers)
				{
					if (distance <= globals::entitydistance)
					{
						playersWithin50m++;
					}

					ImVec2 playerCountPosition = ImVec2(countPosition.x, countPosition.y + visibleTextSize.y + 50);
					std::string nearbyPlayerCountStr = std::string(verucryptt("Nearby Entities: ")) + std::to_string(playersWithin50m);

					ImVec2 distancePosition = ImVec2(playerCountPosition.x, playerCountPosition.y + visibleTextSize.y + 50);
					std::string distanceText = std::string(verucryptt("Current Nearby Entities Distance: ")) + std::to_string(globals::entitydistance);

					ImGui::GetBackgroundDrawList()->AddText(playerCountPosition, ImColor(0, 255, 0, 255), nearbyPlayerCountStr.c_str());
					ImGui::GetBackgroundDrawList()->AddText(distancePosition, ImColor(255, 255, 255, 255), distanceText.c_str());
				}				

				ImColor vis;
				ImColor invis;
				ImColor normal;

				vis = ImVec4(globals::g_color_Visible[0], globals::g_color_Visible[1], globals::g_color_Visible[2], 1.0f);
				invis = ImVec4(globals::g_color_invisible[0], globals::g_color_invisible[1], globals::g_color_invisible[2], 1.0f);
				normal = ImVec4(globals::g_color[0], globals::g_color[1], globals::g_color[2], 1.0f);

				ImColor visibleColor;
				if (globals::g_visible_check && globals::g_enable_esp)
				{
					visibleColor = ue5->is_visible(cached.skeletal_mesh) ? ImColor(vis) : ImColor(invis);
				}
				else
				{
					visibleColor = ImColor(normal);
				}

				if (globals::g_line && globals::g_enable_esp)
				{
					switch (globals::g_line_type)
					{
					case 0: //top
						draw_list->AddLine(ImVec2(screen_width / 2, 0), ImVec2(head_box.x, head_box.y), visibleColor, globals::g_line_thick);
						break;
					case 1:	//middle
						draw_list->AddLine(ImVec2(screen_width / 2, screen_height / 2), ImVec2(head_box.x, head_box.y), visibleColor, globals::g_line_thick);
						break;
					case 2: //bottom
						draw_list->AddLine(ImVec2(screen_width / 2, screen_height), ImVec2(root_box.x, root_box.y), visibleColor, globals::g_line_thick);
						break;
					}
				}

				if (globals::g_box_esp && globals::g_enable_esp) {
					switch (globals::g_box_type)
					{
					case 0: /*filled*/
					{

						if (globals::g_outline)
						{
							//float alpha = 1.0f; // Assuming you want full alpha

							//DrawFilledESP(head_box.x - (box_width / 2), head_box.y, box_width, box_height, alpha, globals::g_box_thick);
							//DrawFilledESP(head_box.x - (box_width / 2), head_box.y, box_width, box_height, alpha, globals::g_box_thick);
							TwoD(head_box.x - (box_width / 2), head_box.y, box_width, box_height, ImColor(0, 0, 0), globals::g_box_thick + 2.5);

						}

						float alpha = 1.0f; // Assuming you want full alpha

						DrawFilledESP(head_box.x - (box_width / 2), head_box.y, box_width, box_height, alpha, globals::g_box_thick);
						DrawFilledESP(head_box.x - (box_width / 2), head_box.y, box_width, box_height, alpha, globals::g_box_thick);


						break;
					}
					}
				}



				if (globals::g_username && globals::g_enable_esp) {
					switch (globals::g_username_type)
					{
					case 0: //top
					{
						std::string username_str = ue5->get_player_name(cached.player_state);
						ImVec2 text_size = ImGui::CalcTextSize(username_str.c_str());
						//DrawString(globals::g_font_size, head_box.x - (text_size.x / 2), head_box.y - 20, ImColor(255, 255, 255), false, true, username_str.c_str());
						DrawString(globals::g_font_size, head_box.x - (text_size.x / 2) , head_box.y - 40, ImColor(255, 255, 255), false, true, username_str.c_str());

						break;
					}
					}
				}



				if (globals::g_ranked && globals::g_enable_rankedesp)
				{
					ImVec2 text_size;
					uintptr_t habenero = read<uintptr_t>(cached.player_state + 0x9f0);
					int32_t RankProgress = read<int32_t>(habenero + 0xb8 + 0x10);
					std::string ranktype = ue5->get_rank(RankProgress);
					std::wstring w_ranktype = string_to_wstring(ranktype);
					std::string final = wstring_to_utf8(w_ranktype);
					char ranked[64];
					sprintf_s(ranked, E("%s"), (final));

					ImVec4 rankedColor = ue5->get_rank_color(RankProgress);

					//DrawString(13, root_box1.x - 13, root_box1.y - 7, rankedColor, true, true, ranked);
					//DrawString(globals::g_font_size, head_box.x - (text_size.x / 2), head_box.y - 40, rankedColor, false, true, ranked);
					DrawString(globals::g_font_size, head_box.x - (text_size.x / 2) - 25, head_box.y - 20, rankedColor, false, true, ranked);

				}

				if (globals::oof)
				{
					oof_arrow(root_bone, is_vis);
				}



				if (globals::g_platform) {
					DWORD_PTR test_platform = read<DWORD_PTR>((uintptr_t)cached.player_state + 0x438);

					wchar_t platform[64]; 
					
					test_platform = read<DWORD_PTR>((uintptr_t)cached.player_state + 0x438);

					std::wstring platform_wstr(platform);

					std::string platform_str(platform_wstr.begin(), platform_wstr.end());

					ImVec2 text_size = ImGui::CalcTextSize(platform_str.c_str());

					DrawString(globals::g_font_size, root.x - (text_size.x / 2), head_box.y - 35, ImColor(255, 255, 255), false, true, platform_str.c_str());
				}

				if (globals::headesp)
				{
					fvector2d head_2d = ue5->w2s(fvector(head_bone.x, head_bone.y, head_bone.z + 20));

					fvector delta = head_bone - camera_postion.location;
					float distance = delta.length();

					const float constant_circle_size = 10;

					float circle_radius = constant_circle_size * (screen_height / (2.0f * distance * tanf(camera_postion.fov * (float)M_PI / 360.f))) - 1.5;

					float y_offset = +70.0f;
					head_2d.y += y_offset;

					int segments = 50;
					float thickness = 2.0f;

					if (globals::g_outline)
					{
						ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), circle_radius, ImColor(0, 0, 0, 255), segments, globals::g_skeletonthickness + 2);
					}
					ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(head_2d.x, head_2d.y), circle_radius, visibleColor, segments, globals::g_skeletonthickness);
				}

				if (globals::radaresp)
				{
					float radarX = radar_position_x;
					float radarY = radar_position_y;
					float radarSize = radar_size;
					bool isRect = rect_radar;
					fortnite_radar(radarX, radarY, radarSize, isRect);
					add_players_radar(root_bone);

				}

				if (globals::g_distance && globals::g_enable_esp)
				{
					switch (globals::g_distance_type)
					{
					case 0: //top
					{
						std::string Distance = "[" + std::to_string((int)distance) + (std::string)"m]";

						ImVec2 text_size = ImGui::CalcTextSize(Distance.c_str());

						DrawString(globals::g_font_size, root.x - (text_size.x / 2), root.y + 5, ImColor(255, 255, 255), false, true, Distance.c_str());
						break;
					}
					}
				}

				if (globals::g_skeleton) {
					ue5->skeleton(cached.skeletal_mesh);
				}
			}


			float ProjectileSpeed = 60000; // for the Reaper Sniper Rifle 
			fvector ComponentVelocity = read<fvector>(arrays->root_component + 0x168); // ComponentVelocity value
			auto test = read<uintptr_t>(target_entity + offset::root_component);



			if (globals::prediction && GetAsyncKeyState(aimkey)) {
				//float projectileSpeed = 27000.f;
				float projectileSpeed = 60000;
				float projectileGravityScale = 3.5f;
				auto closest_mesh2 = read<std::uint64_t>(target_entity + offset::skeletal_mesh);

				fvector hitbox = ue5->Bone(closest_mesh2, bone::Head);

				float distance = arrays->relative_location.distance(hitbox);

				auto test = read<uintptr_t>(target_entity + offset::root_component);

				fvector Velocity = read<fvector>(test + 0x168);

				fvector Predictor = ue5->Prediction(hitbox, Velocity, distance, ProjectileSpeed);

				fvector2d hitbox_screen_predict = ue5->w2s(Predictor);

				input->move(hitbox_screen_predict);

				// Assign hitbox_screen_predict to wherever you need it, not ue5->PredictLocation
			
			}


			if (globals::PlayerInfo)
			{
				auto less = 5;

				uintptr_t kills1 = read<uintptr_t>(arrays->player_array + 0x1264);
				uintptr_t Levels = read<uintptr_t>(arrays->player_state + 0x1268);
				

				char const* string1;


				char text_ammo_count[256];
				char text_ammo_count2[256];

				sprintf_s(text_ammo_count, ("[Kills: %d]"), kills1);
				sprintf_s(text_ammo_count2, ("[Levels: %d]"), Levels);

				DrawString(18, (head_box.x + 80) - (globals::g_font_size / 2), (head_box.y + 35 - less), ImColor(255, 255, 255), false, true, text_ammo_count);
				DrawString(18, (head_box.x + 80) - (globals::g_font_size / 2), (head_box.y + 55 - less), ImColor(255, 255, 255), false, true, text_ammo_count2);
			}

			if (target_entity && globals::g_aimbot) {
				auto closest_mesh = read<std::uint64_t>(target_entity + offset::skeletal_mesh);

				float projectileSpeed = 0;
				float projectileGravityScale = 0;
				fvector hitbox;
				fvector2d hitbox_screen;

				switch (globals::g_hitbox) {
				case 0:
					hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Head));
					hitbox = ue5->Bone(closest_mesh, bone::Head);;
					break;
				case 1:
					hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Neck));
					hitbox = ue5->Bone(closest_mesh, bone::Neck);;
					break;
				case 2:
					hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Chest));
					hitbox = ue5->Bone(closest_mesh, bone::Chest);;
					break;
				case 3:
					std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
					std::uniform_int_distribution<int> distribution(0, 3);
					int randomHitbox = distribution(rng);

					switch (randomHitbox) {
					case 0:
						hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Head));
						hitbox = ue5->Bone(closest_mesh, bone::Head);
						break;
					case 1:
						hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Neck));
						hitbox = ue5->Bone(closest_mesh, bone::Neck);
						break;
					case 2:
						hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::Chest));
						hitbox = ue5->Bone(closest_mesh, bone::Chest);
						break;
					case 3:
						hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::RShoulder));
						hitbox = ue5->Bone(closest_mesh, bone::RShoulder);
						break;
					case 4:
						hitbox_screen = ue5->w2s(ue5->Bone(closest_mesh, bone::LShoulder));
						hitbox = ue5->Bone(closest_mesh, bone::LShoulder);
						break;
					}
					break;
				}
				std::string LocalPlayerWeapon;

				fvector2d hitbox_screen2 = ue5->w2s(hitbox);

				if (hitbox.x != 0 || hitbox.y != 0 && (get_cross_distance(hitbox.x, hitbox.y, screen_width / 2, screen_height / 2) <= globals::g_aimfov))
				{
					if (ue5->is_visible(closest_mesh))
					{
						if (globals::g_target_text)
						{
							DrawString(20, hitbox_screen.x - 6, hitbox_screen.y - 50, ImColor(255, 0, 0), true, true, ("[T]"));
						}

						if (GetAsyncKeyState(aimkey))
							input->move(hitbox_screen);
					}
				}
			}
			else {
				target_dist = FLT_MAX;
				target_entity = NULL;
			}

			ImGui::PopFont();
		}


	};
} static g_loop::g_fn* g_main = new g_loop::g_fn();