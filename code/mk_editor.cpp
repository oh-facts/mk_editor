#include "mk_editor.h"

enum FILE_TYPE
{
  FILE_TYPE_TEXT,
  FILE_TYPE_BINARY,
  FILE_TYPE_COUNT
};

// ty pine
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

v2i mk_get_cursor_pos()
{
	v2i out = {};
	printf("\033[6n");
	scanf("\033[%d;%dR", &out.x, &out.y);
	
	return out;
}

struct MK_File
{
	u8 *data;
	i32 num_lines;
};

struct MK_Editor
{
	Arena arena;
	Arena transient;
	b32 initialized;
	v2i pos;
	v2i size;
	MK_File file;
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
		
		if(pf->argc == 2)
		{
			Str8 abs_file_path = str8_join(trans, pf->app_dir, str8_lit(pf->argv[1]));
			editor->file.data = read_file(arena, (char*)abs_file_path.c, FILE_TYPE_BINARY);
			editor->file.num_lines++;
			char *buf = (char*)editor->file.data;
			while(*buf)
			{
				if(*buf == '\n')
				{
					editor->file.num_lines++;
				}
				buf++;
			}
			
		}
		
	}
	
	Arena_temp temp = arena_temp_begin(trans);
	
	MK_Buffer buf = {};
	buf.base = push_array(trans, char, Megabytes(1));
	
	mk_buffer_push_hide_cursor(&buf);
	mk_buffer_push_reset_cursor(&buf);
	
	editor->size = get_win_size();
	
	local_persist b32 insert = 0;
	if(c == CTRL_KEY('I'))
	{
		insert = !insert;
	}
	
	if(!insert)
	{
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
	}
	mk_buffer_pushf(&buf,"%s\r\n", editor->file);
	
	// rows
	{
		
		for(i32 i = editor->file.num_lines; i < editor->size.y - 1; i ++)
		{
			mk_buffer_pushf(&buf, "~\r\n");
		}
		mk_buffer_pushf(&buf, "~");
		
		Str8 editor_msg = push_str8f(trans, "mk editor v%d.%d.%d", 
																 MK_VERSION_MAJOR, 
																 MK_VERSION_MINOR, 
																 MK_VERSION_PATCH);
		
		for(i32 i = 0; i < editor->size.x - 1 - editor_msg.len; i ++)
		{
			mk_buffer_pushf(&buf, " ");
		}
		mk_buffer_pushf(&buf,"%s", editor_msg.c);
		
	}
	
	mk_buffer_pushf(&buf,"\x1b[%d;%dH", editor->pos.y + 1, editor->pos.x + 1);
	
	if(insert)
	{
		if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			mk_buffer_pushf(&buf, "%c", c);
			editor->pos.x++;
		}
	}
	
	mk_buffer_push_show_cursor(&buf);
	mk_buffer_submit(&buf);
	
	arena_temp_end(&temp);
}