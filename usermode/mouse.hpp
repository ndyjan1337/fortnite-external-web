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

inline bool(*_NtUserInjectMouseInput)(InjectedInputMouseInfo*, int) = nullptr;

class c_mouse {
public:

	auto initialize( ) -> bool;

	auto set_cursor_position( int x, int y ) -> bool;

	auto left_down( ) -> bool;

	auto left_up( ) -> bool;

	auto target_screen_pos( int x, int y ) -> void;

}; static c_mouse* mouse = new c_mouse;