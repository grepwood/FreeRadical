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
uint64_t AssignInteger(const char * String, FILE * ConfigFile)
{
	char * line = NULL;
	uint64_t result;
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
	const char * _FO_CFG_PATH = "~/.freeradical/freeradical.conf";
	const char * _FO_CFG_LOCK = "~/.freeradical/freeradical.lock";
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
	System.free_space = (uint32_t)AssignInteger("free_space=",ConfigFile);
	System.hashing = (char)AssignInteger("hashing=",ConfigFile);
	System.interrupt_walk = (char)AssignInteger("interrupt_walk=",ConfigFile);
	System.language = AssignString("language=",ConfigFile);
	System.master_dat = AssignString("master_dat=",ConfigFile);
	System.master_patches = AssignString("master_patches=",ConfigFile);
	System.scroll_lock = (char)AssignInteger("scroll_lock=",ConfigFile);
	System.splash = (char)AssignInteger("splash=",ConfigFile);
	System.times_run = (char)AssignInteger("times_run=",ConfigFile);
}
void FillConfigStruct(struct * FRConfig Config, const char * Conf)
{
	FILE * ConfigFile = fopen(Conf,"r");
	FillDebug(Config->Debug,ConfigFile);
	FillPreferences(Config->Preferences,ConfigFile);
	FillSound(Config->Sound,ConfigFile);
	FillSystem(Config->System,ConfigFile);
	fclose(ConfigFile);
}
void DumpDebug(struct * FRDebug Debug, FILE * ConfigFile)
{
	fputs("[debug]\n",ConfigFile);
	fpritnf(ConfigFile,"mode=%s\n",Debug.mode);
	fprintf(ConfigFile,"output_map_data_info=%i\n",Debug.output_map_data_info);
	fprintf(ConfigFile,"show_load_info=%i\n",Debug.show_load_info);
	fprintf(ConfigFile,"show_script_messages=%i\n",Debug.show_script_messages);
	fprintf(ConfigFile,"show_tile_num=%i\n\n",Debug.show_tile_num);
}
void DumpPreferences(struct * FRPreferences Preferences, FILE * ConfigFile)
{
	fputs("[preferences]\n",ConfigFile);
	fprintf(ConfigFile,"brightness=%.6f\n",Preferences.brightness);
	fprintf(ConfigFile,"combat_difficulty=%i\n",Preferences.combat_difficulty);
	fprintf(ConfigFile,"combat_looks=%i\n",Preferences.combat_looks);
	fprintf(ConfigFile,"combat_messages=%i\n",Preferences.combat_messages);
	fprintf(ConfigFile,"combat_speed=%i\n",Preferences.combat_speed);
	fprintf(ConfigFile,"game_difficulty=%i\n",Preferences.game_difficulty);
	fprintf(ConfigFile,"item_highlight=%i\n",Preferences.item_highlight);
	fprintf(ConfigFile,"language_filter=%i\n",Preferences.language_filter);
	fprintf(ConfigFile,"mouse_sensitivity=%.6f\n",Preferences.mouse_sensitivity);
	fprintf(ConfigFile,"player_speedup=%i\n",Preferences.player_speedup);
	fprintf(ConfigFile,"running=%i\n",Preferences.running);
	fprintf(ConfigFile,"subtitles=%i\n",Preferences.subtitles);
	fprintf(ConfigFile,"target_hightlight=%i\n",Preferences.target_hightlight);
	fprintf(ConfigFile,"text_base_delay=%.6f\n",Preferences.text_base_delay);
	fprintf(ConfigFile,"text_line_delay=%.6f\n",Preferences.text_line_delay);
	fprintf(ConfigFile,"violence_level=%i\n",Preferences.violence_level);
	fputc('\n',ConfigFile);
}
void DumpSound(struct * FRSound Sound, FILE * ConfigFile)
{
	fputs("[sound]\n",ConfigFile);
	fprintf(ConfigFile,"cache_size=%i\n",Sound.cache_size);
	fprintf(ConfigFile,"device=%i\n",Sound.device);
	fprintf(ConfigFile,"initialize=%i\n",Sound.initialize);
	fprintf(ConfigFile,"irq=%i\n",Sound.irq);
	fprintf(ConfigFile,"master_volume=%i\n",Sound.master_volume);
	fprintf(ConfigFile,"music=%i\n",Sound.music);
	fprintf(ConfigFile,"music_path1=%s\n",Sound.music_path1);
	fprintf(ConfigFile,"music_path2=%s\n",Sound.music_path2);
	fprintf(ConfigFile,"music_volume=%i\n",Sound.music_volume);
	fprintf(ConfigFile,"port=%i\n",Sound.port);
	fprintf(ConfigFile,"sndfx_volume=%i\n",Sound.sndfx_volume);
	fprintf(ConfigFile,"sounds=%i\n",Sound.sounds);
	fprintf(ConfigFile,"speech=%i\n",Sound.speech);
	fprintf(ConfigFile,"speech_volume=%i\n",Sound.speech_volume);
	fputs('\n',ConfigFile);
}
void DumpSystem(struct * FRSystem System, FILE * ConfigFile)
{
	fputs("[system]\n",ConfigFile);
	fprintf(ConfigFile,"art_cache_size=%i\n",System.art_cache_size);
	fprintf(ConfigFile,"color_cycling=%i\n",System.color_cycling);
	fprintf(ConfigFile,"critter_dat=%s\n",System.critter_dat);
	fprintf(ConfigFile,"critter_patches=%s\n",System.critter_patches);
	fprintf(ConfigFile,"cycle_speed_factor=%i\n",System.cycle_speed_factor);
	fprintf(ConfigFile,"executable=%s\n",System.executable);
	fprintf(ConfigFile,"free_space=%i\n",System.free_space);
	fprintf(ConfigFile,"hashing=%i\n",System.hashing);
	fprintf(ConfigFile,"interrupt_walk=%i\n",System.interrupt_walk);
	fprintf(ConfigFile,"language=%s\n",System.language);
	fprintf(ConfigFile,"master_dat=%s\n",System.master_dat);
	fprintf(ConfigFile,"master_patches=%s\n",System.master_patches);
	fprintf(ConfigFile,"scroll_lock=%i\n",System.scroll_lock);
	fprintf(ConfigFile,"splash=%i\n",System.splash);
	fprintf(ConfigFile,"times_run=%i\n",System.times_run);
	fputs('\n',ConfigFile);
}
void DumpMemConfig(struct * FRConfig Config, const char * Conf)
{
	FILE * ConfigFile = fopen(Conf,"w");
	DumpDebug(Config->Debug,ConfigFile);
	DumpPreferences(Config->Preferences,ConfigFile);
	DumpSound(Config->Sound,ConfigFile);
	DumpSystem(Config->System,ConfigFile);
	fclose(ConfigFile);
}
void ChangeConfig(struct * FRConfig Config)
{
	FILE * lock = fopen(_FO_CFG_LOCK,"w");
	fclose(lock);
	remove(_FO_CFG_PATH);
	DumpMemConfig(&Config,_FO_CFG_PATH);
	remove(_FO_CFG_LOCK);
}
int main()
{
	struct FRConfig Config;
	FillConfigStruct(&Config,_FO_CFG_PATH);
	
}
