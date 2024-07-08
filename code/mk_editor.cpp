/* date = May 1st 2024 2:40 pm */
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

struct MK_Line
{
	u8 *data;
	u32 num_col;
};

struct MK_File
{
	Str8 name;
	MK_Line *lines;
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
		tcxt_init();
		arena_innit(arena, pf->mem_size / 2, (u8*)pf->memory + sizeof(MK_Editor));
		arena_innit(trans, pf->mem_size / 2, arena->base + arena->size);
		
		if(pf->argc == 2)
		{
			Str8 arg_str = push_str8f(trans, pf->argv[1]);
			{
				char *cur = (char*)&arg_str.c[arg_str.len - 1];
				u32 count = 0;
				while(*cur != '/')
				{
					cur--;
					count++;
				}
				
				char *file_name_cstr = push_array(arena, char, count + 1);
				mem_cpy(file_name_cstr, cur + 1, count);
				file_name_cstr[count] = '\0';
				editor->file.name = str8((u8*)file_name_cstr, count);
			}
			Str8 abs_file_path = str8_join(trans, pf->app_dir, arg_str);
			
			editor->pos.y = 1;
			editor->pos.x = 1;
			
			u8 *file_data = read_file(trans, (char*)abs_file_path.c, FILE_TYPE_TEXT);
			
			// TODO(mizu): Allow arbitary sized lines. Use chunked lists.
			editor->file.lines = push_array(arena, MK_Line, 100000);
			
			char *start = (char *)file_data;
			char *cur = start;
			while(1)
			{
				if(*cur == '\n' || *cur == '\0')
				{
					editor->file.lines[editor->file.num_lines].data = push_array(arena, u8, (cur - start + 1));
					mem_cpy(editor->file.lines[editor->file.num_lines].data, start, cur - start);
					editor->file.num_lines++;
					start = cur;
					if(*cur == '\0')
					{
						break;
					}
					
				}
				
				editor->file.lines[editor->file.num_lines].num_col++;
				cur++;
			}
			
		}
		
	}
	
	Arena_temp temp = arena_temp_begin(trans);
	
	MK_Buffer buf = {};
	buf.base = push_array(trans, char, Megabytes(1));
	
	mk_buffer_push_clear_screen(&buf);
	mk_buffer_push_hide_cursor(&buf);
	mk_buffer_push_reset_cursor(&buf);
	
	editor->size = get_win_size();
	
	local_persist b32 insert = 0;
	if(c == CTRL_KEY('I'))
	{
		insert = !insert;
	}
	
	local_persist i32 start = 0;
	
	if(!insert)
	{
		MK_KEY mv_key = mk_key_from_char(c);
		switch(mv_key)
		{
			case MK_KEY_UP:
			{
				if(editor->pos.y > 1)
				{
					editor->pos.y--;
				}
				else
				{
					if(start > 0)
					{
						start--;
						//mk_buffer_push_clear_screen(&buf);
					}
				}
			}break;
			case MK_KEY_DOWN:
			{
				if(editor->pos.y < editor->size.y - 1)
				{
					editor->pos.y++;
				}
				else
				{
					if(start + editor->size.y < editor->file.num_lines + 2)
					{
						start++;
					}
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
				if(editor->pos.x > 1)
				{
					editor->pos.x --;
				}
			}break;
			case MK_KEY_PAGE_DOWN:
			{
				if(editor->pos.y < editor->size.y - 1)
				{
					editor->pos.y += editor->size.y;
					
					if(editor->pos.y > editor->size.y)
					{
						editor->pos.y = editor->size.y - 1;
					}
					
				}
				else
				{
					start += editor->size.y;
					
					if(start + editor->pos.y > editor->file.num_lines - 1)
					{
						start = editor->file.num_lines - editor->size.y + 2;
					}
					
				}
				
			}break;
			case MK_KEY_PAGE_UP:
			{
				if(editor->pos.y > 1)
				{
					editor->pos.y -= editor->size.y;
					
					if(editor->pos.y < 1)
					{
						editor->pos.y = 1;
					}
					
				}
				else
				{
					start -= editor->size.y;
					
					if(start <= 1)
					{
						start = 1;
					}
				}
				
			}break;
			case MK_KEY_HOME:
			{
				editor->pos.y = 1;
				start = 1;
			}break;
			
			case MK_KEY_END:
			{
				editor->pos.y = editor->size.y - 1;
				start = editor->file.num_lines - editor->size.y + 2;
			}break;
			
			default:
			{
				
			}
		}
	}
	
	i32 end = start + editor->size.y;
	
	if(end > editor->file.num_lines)
	{
		end = editor->file.num_lines;
	}
	//printf("%d %d\r\n", start, end);
	//INVALID_CODE_PATH();
	for(i32 i = start; i < end; i ++)
	{
		mk_buffer_push(&buf, (char*)editor->file.lines[i].data, editor->file.lines[i].num_col);
	}
	
	mk_buffer_pushf(&buf,"\x1b[%d;%dH", editor->size.y, 1);
	
	// rows
	{
		Str8 editor_msg = push_str8f(trans, "%s row:%d col: %d mk editor v%d.%d.%d",
																 editor->file.name.c,
																 editor->pos.y + start,
																 editor->pos.x,
																 MK_VERSION_MAJOR,
																 MK_VERSION_MINOR,
																 MK_VERSION_PATCH);
		
		for(i32 i = 0; i < editor->size.x - 1 - editor_msg.len; i ++)
		{
			mk_buffer_push(&buf, " ", 1);
		}
		mk_buffer_push(&buf, (char*)editor_msg.c, editor_msg.len);
		
	}
	
	{
		v2i pos = editor->pos;
		
		mk_buffer_pushf(&buf,"\x1b[%d;%dH", pos.y, pos.x);
	}
	
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