namespace globals 
{

	uintptr_t image_base;

	bool g_aimbot = 1;

	bool g_ignore_downed = 1;

	bool g_ignore_bots = 0;

	bool g_visible_check = 1;

	int g_aimfov = 15;

	int g_hitbox = 0;
	
	bool g_target_line;

	bool g_skeleton = 0;

	bool g_staticbox = 1;

	bool g_distance = 1;

	bool g_username = 1;

	bool g_snapline = 0;

	float g_render_distance = 300;
	
	bool g_view_angle = 0;

	int g_smooth = 6;

	bool g_render_fov = 1;

	bool g_render_crosshair = 0;

	bool g_render_menu = 1;

	bool g_shotguns_only = 1;

	bool g_ignore_keybind = 0;

	int g_aim_key = VK_RBUTTON;

	bool g_triggerbot = 0;
	float g_maximum_distance = 10;
	float g_custom_delay = 50;
	bool g_has_clicked;
	std::chrono::steady_clock::time_point g_tb_begin;
	std::chrono::steady_clock::time_point g_tb_end;
	int g_tb_time_since;

	bool g_streamproof;
}

namespace rifle
{

	bool aimbot = 1;
	bool ignore_downed = 1;
	bool ignore_bots = 0;
	bool visible_check = 1;
	bool target_line = 1;
	float aim_fov = 15;
	float smooth = 3;

}

namespace shotgun
{

	bool aimbot = 1;
	bool ignore_downed = 1;
	bool ignore_bots = 0;
	bool visible_check = 1;
	bool target_line = 1;
	float aim_fov = 15;
	float smooth = 3;

}

namespace smg
{

	bool aimbot = 1;
	bool ignore_downed = 1;
	bool ignore_bots = 0;
	bool visible_check = 1;
	bool target_line = 1;
	float aim_fov = 15;
	float smooth = 3;

}

namespace sniper
{

	bool aimbot = 1;
	bool ignore_downed = 1;
	bool ignore_bots = 0;
	bool visible_check = 1;
	bool target_line = 1;
	float aim_fov = 15;
	float smooth = 3;

}