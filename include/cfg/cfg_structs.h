#include <stdint.h>

struct FRDebug
{
	char * mode;
	char output_map_data_info;
	char show_load_info;
	char show_script_messages;
	char show_tile_num;
};

struct FRPreferences
{
	float brightness;
	char combat_difficulty;
	char combat_looks;
	char combat_messages;
	char combat_speed;
	char game_difficulty;
	char item_highlight;
	char language_filter;
	float mouse_sensitivity;
	char player_speedup;
	char running;
	char subtitles;
	char target_hightlight;
	float text_base_delay;
	float test_line_delay;
	char violence_level;
};

struct FRSound
{
	uint16_t cache_size;
	int16_t device;
	char initialize;
	int16_t irq;
	int16_t master_volume;
	char music;
	char * music_path1;
	char * music_path2;
	int16_t music_volume;
	int16_t port;
	int16_t sndfx_volume;
	char sounds;
	char speech;
	int16_t speech_volume;
};

struct FRSystem
{
	int16_t art_cache_size;
	char color_cycling;
	char * critter_dat;
	char * critter_patches;
	char cycle_speed_factor;
	char * executable;
	size_t free_space;
	char hashing;
	char interrupt_walk;
	char * language;
	char * master_dat;
	char * master_patches;
	char scroll_lock;
	char splash;
	char times_run;
};

struct FRConfig
{
	struct FRDebug Debug;
	struct FRPreferences Preferences;
	struct FRSound Sound;
	struct FRSystem System;
};
