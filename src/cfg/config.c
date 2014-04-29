#include <stdio.h>

#include "cfg_structs.h"
#include "grepline.h"

#ifdef linux
	const char * _FO_CFG_FILE = "~/.freeradical/freeradical.conf";
#else
#	error "Install Gentoo"
#endif

void FillDebug(struct * FRDebug Debug, FILE * ConfigFile)
{
// Initialize
	char * line = NULL;
	size_t len;
// Reading mode
	while(strcmp(line,"mode="))
	{
		grepline(&line,&len,ConfigFile);
	}
	Debug.mode = malloc(len-5+1);
	strcpy(Debug.mode,line+5);
// Reading output_map_data_info
	while(strcmp(line,"output_map_data_info="))
	{
		grepline(&line,&len,ConfigFile);
	}
	Debug.output_map_data_info = strtol(line+21,NULL,10);
// Reading show_load_info
	while(strcmp(line,"show_load_info="))
	{
		grepline(&line,&len,ConfigFile);
	}
	Debug.show_load_info = strtol(line+15,NULL,10);
// Reading show_script_messages
	while(strcmp(line,"show_script_messages="))
	{
		grepline(&line,&len,ConfigFile);
	}
	Debug.show_script_messages = strtol(line+21,NULL,10);
// Reading show_tile_num
	while(strcmp(line,"show_tile_num="))
	{
		grepline(&line,&len,ConfigFile);
	}
	Debug.show_tile_num = strtol(line+14,NULL,10);
	free(line);
}

void FillPreferences(struct * FRPreferences Preferences, FILE * ConfigFile)
{
	char * line = NULL;
	size_t len;
// Reading britghtness
	while(strcmp(line,"brightness="))
	{
		grepline(&line,&len,ConfigFile);
	}
	Preferences.brightness = strtof(line+11,NULL);
// Reading combat_difficulty
	while(strcmp(line,"combat_difficulty="))
	{
		grepline(&line,&len,ConfigFile);
	}
	Preferences.combat_difficulty = strtol(line+18,NULL,10);
// Reading combat_looks
	while(strcmp(line,"combat_looks="))
	{
		grepline(&line,&len,ConfigFile);
	}
	Preferences.combat_looks = strtol(line+13,NULL,10);
// Reading combat_messages
	while(strcmp(line,"combat_messages="))
	{
		grepline(&line,&len,ConfigFile);
	}
	Preferences.combat_messages = strtol(line+16,NULL,10);
// Reading combat_speed
	while(strcmp(line,"combat_speed="))
	{
		grepline(&line,&len,ConfigFile);
	}
	Preferences.combat_speed = strtol(line+13,NULL,10);
// Reading game_difficulty
	while(strcmp(line,"game_difficulty="))
	{
		grepline(&line,&len,ConfigFile);
	}
	Preferences.game_difficulty = strtol(line+16,NULL,10);
// Reading item_highlight
	while(strcmp(line,"item_highlight=")
	{
		grepline(&line,&len,ConfigFile);
	}
	Preferences.item_highlight = strtol(line+15,NULL,10);
// Reading language_filter
	while(strcmp(line,"language_filter="))
	{
		grepline(&line,&len,FileConfig);
	}
	Preferences.language_filter = strtol(line+16,NULL,10);
// Reading mouse_sensitivity
	while(strcmp(line,"mouse_sensitivity="))
	{
		grepline(&line,&len,FileConfig);
	}
	Preferences.mouse_sensitivity = strtof(line+18,NULL);
// Reading player_speedup
	while(strcmp(line,"player_speedup="))
	{
		grepline(&line,&len,FileConfig);
	}
	Preferences.player_speedup = strtol(line+15,NULL,10);
// Reading running
	while(strcmp(line,"running="))
	{
		grepline(&line,&len,FileConfig);
	}
	Preferences.running = strtol(line+8,NULL,10);
// Reading subtitles
	while(strcmp(line,"subtitles="))
	{
		grepline(&line,&len,FileConfig);
	}
	Preferences.subtitles = strtol(line+10,NULL,10);
// Reading target_highlight
	while(strcmp(line,"target_hightlight="))
	{
		grepline(&line,&len,FileConfig);
	}
	Preferences.target_hightlight = strtol(line+18,NULL,10);
// Reading test_line_delay
	while(strcmp(line,"test_line_delay="))
	{
		grepline(&line,&len,FileConfig);
	}
	Preferences.test_line_delay = strtof(line+16,NULL);
// Reading text_base_delay
	while(strcmp(line,"text_base_delay="))
	{
		grepline(&line,&len,FileConfig);
	}
	Preferences.text_base_delay = strtof(line+16,NULL);
// Reading violence_level
	while(strcmp(line,"violence_level="))
	{
		grepline(&line,&len,FileConfig);
	}
	Preferences.violence_level = strtol(line+15,NULL,10);
	free(line);
}

void FillSound(struct * FRSound Sound, FILE * ConfigFile)
{
	char * line = NULL
	size_t len;
	while(strcmp(line,"cache_size="))
	{
		grepline(&line,&len,FileConfig);
	}
	Sound.cache_size = strtol(line+11,NULL,10);
	
}

void FillConfigStruct(struct * FRConfig Config, FILE * ConfigFile)
{
	FillDebug(Config->Debug,ConfigFile);
	FillPreferences(Config->Preferences,ConfigFile);
	FillSound(Config->Sound,ConfigFile);
	FillSystem(Config->System,ConfigFile);
}

int main()
{
	FILE * ConfigFile = fopen(_FO_CFG_FILE,"r");
	struct FRConfig Config;
	FillConfigStruct(&Config,ConfigFile);
}
