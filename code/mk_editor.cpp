#include "mk_platform.h"

#include "w_buffer.h"

#include "mk_config.h"
#include "mk_key.h"
#include "mk_text_buffer.h"
#include "mk_window.h"
#include "mk_editor.h"

#include "w_buffer.cpp"
#include "mk_key.cpp"
#include "mk_text_buffer.cpp"
#include "mk_window.cpp"

extern "C"
{
	void update_and_render(MK_Platform *, char);
}

void update_and_render(MK_Platform *pf, char c)
{
	BEGIN_TIMED_BLOCK(UPDATE_AND_RENDER);
	if(pf->reloaded)
	{
		mk_global_platform_api_init(&pf->api);
		tcxt_init();
		pf->reloaded = 0;
	}
	
	if(!pf->initialized)
	{
		pf->initialized = true;
		mk_global_platform_api_init(&pf->api);
		Arena *arena = arena_create(Megabytes(1), Gigabytes(1));
		MK_Editor *editor = push_struct(arena, MK_Editor);
		pf->memory = (void*)editor;
		editor->arena = arena;
		editor->transient = arena_create();
		
		tcxt_init();
		Arena_temp temp = arena_temp_begin(editor->transient);
		
		if(pf->argc == 2)
		{
			Str8 arg_str = push_str8f(editor->transient, pf->argv[1]);
			char *file_name = file_name_from_path(editor->transient, arg_str);
			Str8 abs_file_path = str8_join(editor->transient, pf->app_dir, arg_str);
			
			printf("%s\r\n", abs_file_path.c);
			printf("%s\r\n", file_name);
			
			u8 *file = read_file(editor->transient, (char*)abs_file_path.c, FILE_TYPE_BINARY);
			
			editor->window.w_row_list = mk_word_list_from_buffer(editor->arena, file);
			printf("wewewewewew\r\n");
			
			//editor->window.scroll_row_node = editor->window.w_row_list.first;
			
			arena_temp_end(&temp);
		}
		else
		{
			INVALID_CODE_PATH();
		}
		
	}
	
	MK_Editor *editor = (MK_Editor*)pf->memory;
	Arena *arena = editor->arena;
	Arena *trans = editor->transient;
	Arena_temp temp = {};
	
	temp = arena_temp_begin(trans);
	MK_Window *win = &editor->window;
	win->wbuf = w_buffer_alloc(trans, trans->cmt / 4);
	win->arena = arena;
	
	mk_set_win_size(win);
	
	mk_cursor_mv(win, c);
	
	mk_window_begin_render(win);
	local_persist u64 cycle_count = 0;
	local_persist u32 hit_count = 0;
	
	win->status_msg = push_str8f(trans, "hc:%u cc:%lu row:%d col:%d scroll:%d mk editor v%d.%d.%d", hit_count, cycle_count, win->cursor.row + 1, win->cursor.col + 1, win->scroll_row, MK_VERSION_MAJOR, MK_VERSION_MINOR ,MK_VERSION_PATCH);
	
	mk_window_render(win);
	
	mk_window_end_render(win);
	
	mk_window_submit(win);
	arena_temp_end(&temp);
	
	END_TIMED_BLOCK(UPDATE_AND_RENDER);
	
	cycle_count = tcxt.counters[DEBUG_CYCLE_COUNTER_UPDATE_AND_RENDER].cycle_count;
	hit_count = tcxt.counters[DEBUG_CYCLE_COUNTER_UPDATE_AND_RENDER].hit_count;
	
	
	process_debug_counters();
}