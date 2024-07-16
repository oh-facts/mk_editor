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
		
		if(pf->argc == 2)
		{
			Arena_temp temp = arena_temp_begin(editor->transient);
			Str8 arg_str = push_str8f(editor->transient, pf->argv[1]);
			char *file_name = file_name_from_path(editor->transient, arg_str);
			
			Str8 abs_file_path = str8_join(editor->arena, pf->app_dir, arg_str);
			
			printf("%s\r\n", abs_file_path.c);
			printf("%s\r\n", file_name);
			
			u8 *file = read_file(editor->transient, (char*)abs_file_path.c, FILE_TYPE_BINARY);
			
			MK_Editor *editor = (MK_Editor*)pf->memory;
			editor->window.file_name = abs_file_path;
			
			if(file)
			{
				editor->window.w_row_list = mk_word_list_from_buffer(arena, file);
			}
			else
			{
				editor->window.w_row_list = mk_word_list_new_file(arena);
			}
			//editor->window.scroll_row_node = editor->window.w_row_list.first;
			
			arena_temp_end(&temp);
		}
		else
		{
			submit_clear_screen();
			printf("Yout must pass a file name \r\n");
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
	local_persist u64 cycle_counts[DEBUG_CYCLE_COUNTER_COUNT] = {};
	
	BEGIN_TIMED_BLOCK(RECORD);
	mk_window_begin_render(win);
	
	// TODO(mizu): make a fn to format numbers so bytes are converted to bigger types automatically
	win->status_msg = push_str8f(trans, "res:%.fG cmt:%.fM cc:%lu cc:%lu cc:%lu row:%d col:%d scroll:%d mk editor v%d.%d.%d", pf->res * 0.000000001, pf->cmt * 0.000001, cycle_counts[0], cycle_counts[1], cycle_counts[2], win->cursor.row + 1, win->cursor.col + 1, win->scroll_row, MK_VERSION_MAJOR, MK_VERSION_MINOR ,MK_VERSION_PATCH);
	
	
	mk_window_render(win);
	
	mk_window_end_render(win);
	END_TIMED_BLOCK(RECORD);
	
	BEGIN_TIMED_BLOCK(SUBMIT);
	mk_window_submit(win);
	END_TIMED_BLOCK(SUBMIT);
	
	arena_temp_end(&temp);
	
	END_TIMED_BLOCK(UPDATE_AND_RENDER);
	
	for(i32 i = 0; i < DEBUG_CYCLE_COUNTER_COUNT; i++)
	{
		cycle_counts[i] = tcxt.counters[i].cycle_count;
	}
	
	
	process_debug_counters();
}