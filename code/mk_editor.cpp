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

enum FILE_TYPE
{
  FILE_TYPE_TEXT,
  FILE_TYPE_BINARY,
  FILE_TYPE_COUNT
};

#if defined(OS_WIN32)
#define _file_open(file, filepath, mode) fopen_s(file, filepath, mode)
#elif defined (OS_LINUX)
#define _file_open(file, filepath, mode) *file = fopen(filepath, mode)
#endif

u8 *read_file(Arena *arena, const char *filepath, FILE_TYPE type)
{
  FILE *file;
  
  local_persist char *file_type_table[FILE_TYPE_COUNT] = 
  {
    "r",
    "rb"
  };
  
  _file_open(&file, filepath, file_type_table[type]);
  
  if(!file)
  {
    printf("file %s not found\n", filepath);
    INVALID_CODE_PATH();
  }
  
  fseek(file, 0, SEEK_END);
  
  i32 len = ftell(file);
  //print("%d", len);
  
  fseek(file, 0, SEEK_SET);
  
  u8 *buffer = push_array(arena, u8, len);
  fread(buffer, sizeof(u8), len, file);
  
  fclose(file);
  
  return buffer;
}

struct MK_Editor
{
	Arena arena;
	Arena transient;
	b32 initialized;
	v2i pos;
	v2i size;
	u8 *file;
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
	if(!editor->initialized)
	{
		editor->initialized = true;
		arena_innit(arena, pf->mem_size / 2, (u8*)pf->memory + sizeof(*editor));
		arena_innit(trans, pf->mem_size / 2 - sizeof(*editor), arena->base + arena->size);
		
		editor->file = read_file(arena, "out/hi.txt", FILE_TYPE_BINARY);
	}
	
	Arena_temp temp = arena_temp_begin(trans);
	
	MK_Buffer buf = {};
	buf.base = push_array(trans, char, Megabytes(1));
	
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
			mk_buffer_pushf(&buf, trans, "~\r\n");
		}
		mk_buffer_pushf(&buf, trans, "~");
		
		Str8 editor_msg = push_str8f(trans, "mk editor v%d.%d%d", 
																 MK_VERSION_MAJOR, 
																 MK_VERSION_MINOR, 
																 MK_VERSION_PATCH);
		
		for(i32 i = 0; i < editor->size.x - 1 - editor_msg.len; i ++)
		{
			mk_buffer_pushf(&buf, trans, " ");
		}
		mk_buffer_pushf(&buf, trans, "%s", editor_msg.c);
		
	}
	
	mk_buffer_pushf(&buf, trans, "\x1b[%d;%dH", editor->pos.y + 1, editor->pos.x + 1);
	
	mk_buffer_pushf(&buf, trans, "%s", editor->file);
	
	mk_buffer_pushf(&buf, trans, "%c", c);
	
	mk_buffer_push_show_cursor(&buf);
	mk_buffer_submit(&buf);
	
	arena_temp_end(&temp);
}