/* date = July 8th 2024 8:51 pm */

#ifndef MK_PLATFORM_H
#define MK_PLATFORM_H

#define MK_VERSION_MAJOR (0)
#define MK_VERSION_MINOR (0)
#define MK_VERSION_PATCH (1)

#include "stdio.h"

#include "base_core.h"
#include "base_core.cpp"

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
#include "base_string.h"
#include "base_string.cpp"

#include <termios.h>

#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#define CTRL_KEY(k) ((k) & 0x1f)

#define submit_clear_screen() write(STDOUT_FILENO,"\x1b[2J", 4)
#define submit_reset_cursor() write(STDOUT_FILENO, "\x1b[H", 3)

internal void enable_raw_mode()
{
	termios raw;
	tcgetattr(STDIN_FILENO, &raw);
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// ty yeti
internal Str8 os_linux_get_app_dir(Arena *arena)
{
	char buffer[256];
  ssize_t len = readlink("/proc/self/exe", buffer, 256);
	
	char *c = &buffer[len];
  while(*(--c) != '/')
  {
    *c = 0;
    --len;
  }
	
  u8 *str = push_array(arena, u8, len);
	mem_cpy(str, buffer, len);
	
	Str8 out = str8(str, len);
	
	return out;
}


struct TCXT
{
	Arena arenas[2];
};

global TCXT tcxt;

void tcxt_init()
{
	for(u32 i = 0; i < ARRAY_LEN(tcxt.arenas); i ++)
	{
		arena_innit(&tcxt.arenas[i], Megabytes(10), calloc(1,Megabytes(10)));
	}
}

internal Arena *tcxt_get_scratch(Arena **conflicts, u64 count)
{
	Arena *out = 0;
	for(u32 i = 0; i < ARRAY_LEN(tcxt.arenas); i ++)
	{
		b32 has_conflict = 0;
		for(u32 j = 0; j < count; j ++)
		{
			if(&tcxt.arenas[i] == conflicts[j])
			{
				has_conflict = 1;
				break;
			}
		}
		if(!has_conflict)
		{
			out = &tcxt.arenas[i];
		}
	}
	
	return out;
}

internal char *file_name_from_path(Arena *arena, Str8 path)
{
	char *cur = (char*)&path.c[path.len - 1];
	u32 count = 0;
	
	//NOTE(mizu): pig
	while(*cur != '/' && *cur != '\0')
	{
		cur--;
		count++;
	}
	
	
	char *file_name_cstr = push_array(arena, char, count + 1);
	mem_cpy(file_name_cstr, cur + 1, count);
	file_name_cstr[count] = '\0';
	
	return file_name_cstr;
}

#define scratch_begin(conflicts, count) arena_temp_begin(tcxt_get_scratch(conflicts, count))
#define scratch_end(scratch) arena_temp_end(scratch);

struct MK_Platform
{
	void *memory;
	size_t mem_size;
	int argc;
	char **argv;
	Str8 app_dir;
};

typedef void (*update_and_render_fn)(MK_Platform *, char);

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

#endif //MK_PLATFORM_H