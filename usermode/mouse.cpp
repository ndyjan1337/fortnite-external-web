#include <Windows.h>

#include "mouse.hpp"

#include "skcrypt.h"
#include "lazy.h"

auto c_mouse::initialize( ) -> bool {
	auto win32u = LI_FN( LoadLibraryA ).safe_cached( )( skCrypt( "win32u.dll" ) );
	void* NtUserInjectMouseInputAddress = ( void* )LI_FN( GetProcAddress ).safe_cached( )( win32u, skCrypt( "NtUserInjectMouseInput" ) );
	if (!NtUserInjectMouseInputAddress) return false;
	*( void** )&_NtUserInjectMouseInput = NtUserInjectMouseInputAddress;
	LI_FN( FreeLibrary ).get( )( win32u );
	return true;
}

auto c_mouse::set_cursor_position( 
	int x, int y ) -> bool 
{
	InjectedInputMouseInfo temp = {};
	temp.mouse_data = 0;
	temp.mouse_options = InjectedInputMouseOptions::move;
	temp.move_direction_x = x;
	temp.move_direction_y = y;
	temp.time_offset_in_miliseconds = 0;
	return _NtUserInjectMouseInput( &temp, 1 );
}

auto c_mouse::left_down( ) -> bool {
	InjectedInputMouseInfo temp = {};
	temp.mouse_data = 0;
	temp.mouse_options = InjectedInputMouseOptions::left_down;
	temp.move_direction_x = 0;
	temp.move_direction_y = 0;
	temp.time_offset_in_miliseconds = 0;
	return _NtUserInjectMouseInput( &temp, 1 );
}

auto c_mouse::left_up( ) -> bool {
	InjectedInputMouseInfo temp = {};
	temp.mouse_data = 0;
	temp.mouse_options = InjectedInputMouseOptions::left_up;
	temp.move_direction_x = 0;
	temp.move_direction_y = 0;
	temp.time_offset_in_miliseconds = 0;
	return _NtUserInjectMouseInput( &temp, 1 );
}

auto c_mouse::target_screen_pos(int x, int y) -> void {

	fvector2d ScreenCenter = { (double)globals::screen_width / 2 , (double)globals::screen_height / 2 };
	fvector2d Target;

	if (x != 0)
	{
		if (x > ScreenCenter.x)
		{
			Target.x = -(ScreenCenter.x - x);
			Target.x /= globals::aimbot_smooth;
			if (Target.x + ScreenCenter.x > ScreenCenter.x * 2) Target.x = 0;
		}

		if (x < ScreenCenter.x)
		{
			Target.x = x - ScreenCenter.x;
			Target.x /= globals::aimbot_smooth;
			if (Target.x + ScreenCenter.x < 0) Target.x = 0;
		}
	}
	if (y != 0)
	{
		if (y > ScreenCenter.y)
		{
			Target.y = -(ScreenCenter.y - y);
			Target.y /= globals::aimbot_smooth;
			if (Target.y + ScreenCenter.y > ScreenCenter.y * 2) Target.y = 0;
		}

		if (y < ScreenCenter.y)
		{
			Target.y = y - ScreenCenter.y;
			Target.y /= globals::aimbot_smooth;
			if (Target.y + ScreenCenter.y < 0) Target.y = 0;
		}
	}

	set_cursor_position(Target.x, Target.y);

}