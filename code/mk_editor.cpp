/* date = May 1st 2024 2:40 pm */

#include "mk_platform.h"
#include "mk_editor.h"
#include "mk_buffer.h"
#include "mk_window.h"

struct MK_Editor
{
	Arena arena;
	Arena transient;
	b32 initialized;
};

extern "C"
{
	void update_and_render(MK_Platform *, char);
}

void update_and_render(MK_Platform *pf, char c)
{
	MK_Editor *editor = (MK_Editor *)pf->memory;
	Arena *arena = &editor->arena;
	Arena *trans = &editor->transient;
	Arena_temp temp = arena_temp_begin(trans);
	
	if(!editor->initialized)
	{
		editor->initialized = true;
		tcxt_init();
		arena_innit(arena, pf->mem_size / 2, (u8*)pf->memory + sizeof(MK_Editor));
		arena_innit(trans, pf->mem_size / 2, arena->base + arena->size);
		
		if(pf->argc == 2)
		{
			Str8 arg_str = push_str8f(trans, pf->argv[1]);
			char *file_name = file_name_from_path(trans, arg_str);
			Str8 abs_file_path = str8_join(trans, pf->app_dir, arg_str);
			
			printf("%s\r\n", abs_file_path.c);
			printf("%s\r\n", file_name);
			
		}
		
	}
	
	
	arena_temp_end(&temp);
	
}