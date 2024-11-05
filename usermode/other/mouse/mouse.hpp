#include "../lazyimporter.hpp"

enum InjectedInputMouseOptions
{
	move = 1,
	left_up = 4,
	left_down = 2,
	right_up = 8,
	right_down = 16
};

struct InjectedInputMouseInfo
{
	int move_direction_x;
	int move_direction_y;
	unsigned int mouse_data;
	InjectedInputMouseOptions mouse_options;
	unsigned int time_offset_in_miliseconds;
	void* extra_info;
};

bool(*_NtUserInjectMouseInput)(InjectedInputMouseInfo*, int) = nullptr;

namespace mouse
{
	auto WINAPI initialize() -> BOOLEAN {
		auto win32u = LI_FN(LoadLibraryA).safe_cached()(_("win32u.dll"));
		void* NtUserInjectMouseInputAddress = (void*)LI_FN(GetProcAddress).safe_cached()(win32u, _("NtUserInjectMouseInput"));
		if (!NtUserInjectMouseInputAddress) return FALSE;
		*(void**)&_NtUserInjectMouseInput = NtUserInjectMouseInputAddress;
		LI_FN(FreeLibrary).get()(win32u);
		return TRUE;
	}

	auto WINAPI set_cursor_position(int X, int Y, unsigned int time = 0) -> BOOLEAN {
		InjectedInputMouseInfo temp = {};
		temp.mouse_data = 0;
		temp.mouse_options = InjectedInputMouseOptions::move;
		temp.move_direction_x = X;
		temp.move_direction_y = Y;
		temp.time_offset_in_miliseconds = time;
		return _NtUserInjectMouseInput(&temp, 1);
	}

	bool left_down(int x = 0, int y = 0)
	{
		InjectedInputMouseInfo temp = {};
		temp.mouse_data = 0;
		temp.mouse_options = InjectedInputMouseOptions::left_down;
		temp.move_direction_x = x;
		temp.move_direction_y = y;
		temp.time_offset_in_miliseconds = 0;
		return _NtUserInjectMouseInput(&temp, 1);
	}

	bool left_up(int x = 0, int y = 0)
	{
		InjectedInputMouseInfo temp = {};
		temp.mouse_data = 0;
		temp.mouse_options = InjectedInputMouseOptions::left_up;
		temp.move_direction_x = x;
		temp.move_direction_y = y;
		temp.time_offset_in_miliseconds = 0;
		return _NtUserInjectMouseInput(&temp, 1);
	}
	auto target_screen_pos( int x, int y ) -> void {

		fvector2d ScreenCenter = { ( double )cached::screen_width / 2 , ( double )cached::screen_height / 2 };
		fvector2d Target;

		if ( x != 0 )
		{
			if ( x > ScreenCenter.x )
			{
				Target.x = -( ScreenCenter.x - x );
				Target.x /= globals::g_smooth + 1;
				if ( Target.x + ScreenCenter.x > ScreenCenter.x * 2 ) Target.x = 0;
			}

			if ( x < ScreenCenter.x )
			{
				Target.x = x - ScreenCenter.x;
				Target.x /= globals::g_smooth + 1;
				if ( Target.x + ScreenCenter.x < 0 ) Target.x = 0;
			}
		}
		if ( y != 0 )
		{
			if ( y > ScreenCenter.y )
			{
				Target.y = -( ScreenCenter.y - y );
				Target.y /= globals::g_smooth + 1;
				if ( Target.y + ScreenCenter.y > ScreenCenter.y * 2 ) Target.y = 0;
			}

			if ( y < ScreenCenter.y )
			{
				Target.y = y - ScreenCenter.y;
				Target.y /= globals::g_smooth + 1;
				if ( Target.y + ScreenCenter.y < 0 ) Target.y = 0;
			}
		}

		mouse_event( MOUSEEVENTF_MOVE, Target.x, Target.y, 0, 0 );
	}
}