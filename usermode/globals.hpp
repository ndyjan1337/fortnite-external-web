#include <mutex>
#include <memory>
namespace globals
{


	inline bool vsync = 0;
	
	inline bool performancemode = 0;


	inline bool g_aimbot = 1;

	inline bool g_triggerbot = 0;
	int triggerbot_key;

	bool has_clicked;

	int HeldWeaponType;

	int maximum_distance = 20;

	std::chrono::steady_clock::time_point tb_begin;

	std::chrono::steady_clock::time_point tb_end;

	int tb_time_since;

	int custom_delay = 1;

	HWND hwnd = 0;

	inline float g_font_size = 15;

	inline int g_username_type = 0;

	inline int g_watermark_type = 0;
	inline int RadarType = 0;

	inline bool particles = 1;

	inline bool g_target_text = 0;

	inline bool g_details = 0;

	inline bool playerfly = 0;
	inline bool playerfly2 = 0;
	inline bool speedhack = 0;
	inline bool freecam = 0;

	inline bool g_visible_check = 0;

	inline bool g_enemiesarround = 1;

	inline bool g_watermark = 1;

	inline bool customname = 0;

	inline bool g_line = 0;

	inline int g_line_type = 0;

	inline float g_aimfov = 20; //modified old /100

	inline int g_hitbox = 0;

	inline bool nigger = 0;

	inline bool prediction = 0;
	inline bool magicbullet = 1;
	inline float predictiondistance = 50;

	inline bool g_skeleton = 1;

	inline bool PlayerInfo = 0;

	inline bool nearbyplayers = 0;

	inline bool g_box_esp = 1;

	inline bool oof = 0;

	inline bool headesp = 0;

	inline bool g_enable_esp = 1;

	inline bool oof_enable = 1;

	inline bool g_enable_rankedesp = 1;

	inline bool g_distance = 0;
	inline float entitydistance = 50;

	inline bool crosshair = 0;
	float crosshair_size = 2.0f; // Size of the main crosshair lines
	float crosshair_length = 9.0f; // Length of the main crosshair lines
	float crosshair_thickness = 1.0f; // Thickness of the main crosshair lines
	ImVec4 crosshair_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Color of the crosshair lines

	inline bool menucrosshair = 0;

	inline bool keybindlist = 1;

	inline bool g_username = 0;

	inline bool g_ranked = 0;

	inline bool radaresp = 0;

	inline bool weaponesp = 0;

	inline bool g_platform = 0;

	inline int g_distance_type = 0;

	inline bool g_snapline = 0;


	static float g_color_Visible[3] = { 87 / 255.0f, 173 / 255.0f, 17 / 255.0f };
	static float g_color_invisible[3] = { 173 / 255.0f, 17 / 255.0f, 17 / 255.0f };
	static float g_color[3] = { 255.00f, 255.00f, 255.00f };
	inline float FovColor[3] = { 0.f, 0.f, 1.f };

	float g_render_distance = 275;

	inline int g_box_type = 0;

	inline bool g_outline = 0;

	inline float g_box_thick = 1;

	inline int g_line_thick = 1;

	inline int g_skeletonthickness = 1;

	inline float g_aim_max_dist = 275;

	inline float g_smooth = 13; //modified old /6

	inline bool g_render_fov = 1;

	//Loot esp
	inline bool draw_chests = 0;

	inline bool draw_cooler = 0;

	inline int cooler_render = 50;

	inline bool draw_ammo = 0;

	inline int g_weapon_type = 0;

	inline bool g_weapon_rarity = 0;

	inline int ammo_render = 50;

	inline int lammarender = 275;

	inline int vehiclesrender = 50;

	inline bool lamma = 0;

	inline bool pickups = 0;

	inline bool draw_vehicles = 0;

	inline int chestrender = 50;

	inline int pickupsrender = 50;

	inline bool Common = 1;

	inline bool rare = 1;

	inline bool purple = 1;

	inline bool gold = 1;

	inline bool mythic = 1;

	inline bool UnCommon = 1;

	inline bool g_render_menu = 0;

	inline bool settingswindow = 0;

	inline bool in_lobby;
}


namespace developer
{
	namespace menutesting
	{
		inline bool paint = false;
		inline bool notepad = false;
	}
}
