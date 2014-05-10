#include <stdio.h>

#include "cfg_structs.h"
#include "grepline.h"

char * AssignString(const char * String, FILE * ConfigFile)
{
	char * line = NULL;
	char * result = NULL;
	uint8_t LineLen;
	const uint8_t ArgLen = strlen(String);
	while(strncmp(line,String,ArgLen))
	{
		grepline(&line,&LineLen,ConfigFile);
	}
	result = malloc(LineLen-ArgLen+1);
	result[LineLen-ArgLen] = 0;
	strcpy(result,line+ArgLen);
	free(line);
	return result;
}
float AssignFloat(const char * String, FILE * ConfigFile)
{
	char * line = NULL;
	float result;
	uint8_t LineLen;
	const uint8_t ArgLen = strlen(String);
	while(strncmp(line,String,ArgLen))
	{
		grepline(&line,&LineLen,ConfigFile);
	}
	result = strtof(line+ArgLen,NULL);
	free(line);
	return result;
}
uint32_t AssignInteger(const char * String, FILE * ConfigFile)
{
	char * line = NULL;
	uint32_t result;
	uint8_t LineLen;
	const uint8_t ArgLen = strlen(String);
	while(strncmp(line,String,ArgLen))
	{
		grepline(&line,&LineLen,ConfigFile);
	}
	result = strtoull(line+ArgLen,NULL,10);
	free(line);
	return result;
}

#ifdef linux
	const char * _FO_CFG_FILE = "~/.freeradical/freeradical.conf";
#else
#	error "Install Gentoo"
#endif

void FillDebug(struct * FRDebug Debug, FILE * ConfigFile)
{
	Debug.mode = AssignString("mode=",ConfigFile);
	Debug.output_map_data_info = (char)AssignInteger("output_map_data_info=",ConfigFile);
	Debug.show_load_info = (char)AssignInteger("show_load_info=",ConfigFile);
	Debug.show_script_messages = (char)AssignInteger("show_script_messages=",ConfigFile);
	Debug.show_tile_num = (char)AssignInteger("show_tile_num=",ConfigFile);
}
void FillPreferences(struct * FRPreferences Preferences, FILE * ConfigFile)
{
	Preferences.brightness = AssignFloat("brightness=",ConfigFile);
	Preferences.combat_difficulty = (char)AssignInteger("combat_difficulty=",ConfigFile);
	Preferences.combat_looks = (char)AssignInteger("combat_looks=",ConfigFile);
	Preferences.combat_messages = (char)AssignInteger("combat_messages=",ConfigFile);
	Preferences.combat_speed = (char)AssignInteger("combat_speed=",ConfigFile);
	Preferences.game_difficulty = (char)AssignInteger("game_difficulty=",ConfigFile);
	Preferences.item_highlight = (char)AssignInteger("item_highlight=",ConfigFile);
	Preferences.language_filter = (char)AssignInteger("language_filter=",ConfigFile);
	Preferences.mouse_sensitivity = AssignFloat("mouse_sensitivity=",ConfigFile);
	Preferences.player_speedup = (char)AssignInteger("player_speedup=",ConfigFile);
	Preferences.running = (char)AssignInteger("running=",ConfigFile);
	Preferences.subtitles = (char)AssignInteger("subtitles=",ConfigFile);
	Preferences.target_hightlight = (char)AssignInteger("target_hightlight=",ConfigFile);
	Preferences.text_base_delay = AssignFloat("text_base_delay=",ConfigFile);
	Preferences.text_line_delay = AssignFloat("text_line_delay=",ConfigFile);
	Preferences.violence_level = (char)AssignInteger("violence_level=",ConfigFile);
}
void FillSound(struct * FRSound Sound, FILE * ConfigFile)
{
	Sound.cache_size = (uint16_t)AssignInteger("cache_size=",ConfigFile);
	Sound.device = (int16_t)AssignInteger("device=",ConfigFile);
	Sound.initialize = (char)AssignInteger("initialize=",ConfigFile);
	Sound.irq = (int16_t)AssignInteger("irq=",ConfigFile);
	Sound.master_volume = (int16_t)AssignInteger("master_volume=",ConfigFile);
	Sound.music = (char)AssignInteger("music=",ConfigFile);
	Sound.music_path1 = AssignString("music_path1=",ConfigFile);
	Sound.music_path2 = AssignString("music_path2=",ConfigFile);
	Sound.music_volume = (int16_t)AssignInteger("music_volume=",ConfigFile);
	Sound.port = (int16_t)AssignInteger("port=",ConfigFile);
	Sound.sndfx_volume = (int16_t)AssignInteger("sndfx_volume=",ConfigFile);
	Sound.sounds = (char)AssignInteger("sounds=",ConfigFile);
	Sound.speech = (char)AssignInteger("speech=",ConfigFile);
	Sound.speech_volume = (int16_t)AssignInteger("speech_volume=",ConfigFile);
}
void FillSystem(struct * FRSystem System, FILE * ConfigFile)
{
	System.art_cache_size = (int16_t)AssignInteger("art_cache_size=",ConfigFile);
	System.color_cycling = (char)AssignInteger("color_cycling=",ConfigFile);
	System.critter_dat = AssignString("critter_dat=",ConfigFile);
	System.critter_patches = AssignString("critter_patches=",ConfigFile);
	System.cycle_speed_factor = (char)AssignInteger("cycle_speed_factor=",ConfigFile);
	System.executable = AssignString("executable=",ConfigFile);
	System.free_space = AssignInteger("free_space=",ConfigFile);
	System.hashing = (char)AssignInteger("hashing=",ConfigFile);
	System.interrupt_walk = (char)AssignInteger("interrupt_walk=",ConfigFile);
	System.language = AssignString("language=",ConfigFile);
	System.master_dat = AssignString("master_dat=",ConfigFile);
	System.master_patches = AssignString("master_patches=",ConfigFile);
	System.scroll_lock = (char)AssignInteger("scroll_lock=",ConfigFile);
	System.splash = (char)AssignInteger("splash=",ConfigFile);
	System.times_run = (char)AssignInteger("times_run=",ConfigFile);
}
void FillConfigStruct(struct * FRConfig Config)
{
	FILE * ConfigFile = fopen(_FO_CFG_FILE,"r");
	FillDebug(Config->Debug,ConfigFile);
	FillPreferences(Config->Preferences,ConfigFile);
	FillSound(Config->Sound,ConfigFile);
	FillSystem(Config->System,ConfigFile);
	fclose(ConfigFile);
}

int main()
{
	struct FRConfig Config;
	FillConfigStruct(&Config,ConfigFile);
	
}