#include "base_core.h"
#include "base_core.cpp"

#include "stdio.h"

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
#include "base_string.h"
#include "base_string.cpp"

#include "base_math.h"
#include "base_math.cpp"

#include <termios.h>

#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>

#include "mk_editor.h"

struct MK_Editor
{
	Arena arena;
	b32 initialized;
	v2i pos;
	v2i size;
};

extern "C"
{
	void update_and_render(MK_Platform *, char);
}

void update_and_render(MK_Platform *pf, char c)
{
	MK_Editor *editor = (MK_Editor *)pf->memory;
	Arena *arena = &editor->arena;
	if(!editor->initialized)
	{
		editor->initialized = true;
		arena_innit(arena, pf->mem_size - sizeof(*editor), (u8*)pf->memory + sizeof(*editor));
	}
	
	Arena_temp temp = arena_temp_begin(arena);
	
	MK_Buffer buf = {};
	buf.base = push_array(arena, char, Megabytes(1));
	
	mk_buffer_push_clear_screen(&buf);
	mk_buffer_push_hide_cursor(&buf);
	mk_buffer_push_reset_cursor(&buf);
	
	editor->size = get_win_size();
	
	MK_KEY mv_key = mk_key_from_char(c);
	
	switch(mv_key)
	{
		case MK_KEY_UP:
		{
			if(editor->pos.y > 0)
			{
				editor->pos.y --;
			}
		}break;
		case MK_KEY_DOWN:
		{
			if(editor->pos.y < editor->size.y - 1)
			{
				editor->pos.y ++;
			}
		}break;
		case MK_KEY_RIGHT:
		{
			if(editor->pos.x < editor->size.x - 1)
			{
				editor->pos.x ++;
			}
		}break;
		case MK_KEY_LEFT:
		{
			if(editor->pos.x > 0)
			{
				editor->pos.x --;
			}
		}break;
		default:
		{
			
		}
	}
	
	// rows
	{
		for(i32 i = 0; i < editor->size.y - 1; i ++)
		{
			mk_buffer_pushf(&buf, arena, "~\r\n");
		}
		mk_buffer_pushf(&buf, arena, "~");
		
		Str8 editor_msg = push_str8f(arena, "mk editor");
		for(i32 i = 0; i < editor->size.x - 1 - editor_msg.len; i ++)
		{
			mk_buffer_pushf(&buf, arena, " ");
		}
		mk_buffer_pushf(&buf, arena, "%s", editor_msg.c);
		
	}
	
	mk_buffer_pushf(&buf, arena, "\x1b[%d;%dH", editor->pos.y + 1, editor->pos.x + 1);
	
	mk_buffer_pushf(&buf, arena, "%c", c);
	
	mk_buffer_push_show_cursor(&buf);
	mk_buffer_submit(&buf);
	
	arena_temp_end(&temp);
}