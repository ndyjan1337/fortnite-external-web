enum bone : int {
	Head = 110,
	Neck = 66,
	Chest = 37,
	Pelvis = 2,
	LShoulder = 9,
	LElbow = 10,
	LHand = 11,
	RShoulder = 38,
	RElbow = 39,
	RHand = 40,
	LHip = 71,
	LKnee = 72,
	LFoot = 73,
	RHip = 78,
	RKnee = 79,
	RFoot = 82,
	Root = 0
};

enum offset {
	uworld = 0x12da3a98,
	game_instance = 0x208,
	game_state = 0x190,
	local_player = 0x38,
	player_controller = 0x30,
	acknowledged_pawn = 0x350,
	aactor = 0xA8,
	skeletal_mesh = 0x328,
	player_state = 0x2b0,
	root_component = 0x1b0,
	velocity = 0x140,
	relative_location = 0x11c,
	relative_rotation = 0x138,
	current_weapon = 0x9f8,
	weapon_data = 0x510,
	tier = 0x9b,
	team_index = 0xF28,
	player_array = 0x2c0,
	pawn_private = 0x320,
	component_to_world = 0x1c0,
};

enum triggerbotoffsets {
	location_under_reticle = 0x2560,
};




