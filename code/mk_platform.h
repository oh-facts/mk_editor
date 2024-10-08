/* date = July 8th 2024 8:51 pm */

#ifndef MK_PLATFORM_H
#define MK_PLATFORM_H

#define MK_VERSION_MAJOR (0)
#define MK_VERSION_MINOR (2)
#define MK_VERSION_PATCH (1)

#include "stdio.h"
#include "string.h"
#include <unistd.h>
#include "base_core.h"

typedef void *(*os_reserve_fn)(u64 size);
typedef b32 (*os_commit_fn)(void *ptr, u64 size);
typedef void (*os_decommit_fn)(void *ptr, u64 size);
typedef void (*os_release_fn)(void *ptr, u64 size);

global os_reserve_fn os_reserve;
global os_commit_fn os_commit;
global os_decommit_fn os_decommit;
global os_release_fn os_release;

struct MK_Platform_api
{
	os_reserve_fn os_reserve;
	os_commit_fn os_commit;
	os_decommit_fn os_decommit;
	os_release_fn os_release;
};

internal void mk_global_platform_api_init(MK_Platform_api *api)
{
	os_reserve = api->os_reserve;
	os_commit = api->os_commit;
	os_decommit = api->os_decommit;
	os_release = api->os_release;
}

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

enum DEBUG_CYCLE_COUNTER
{
	DEBUG_CYCLE_COUNTER_UPDATE_AND_RENDER,
	DEBUG_CYCLE_COUNTER_RECORD,
	DEBUG_CYCLE_COUNTER_SUBMIT,
	DEBUG_CYCLE_COUNTER_COUNT
};

global char *debug_cycle_to_str[DEBUG_CYCLE_COUNTER_COUNT] = 
{
	"update and render",
	"record",
	"submit",
};

struct debug_cycle_counter
{
	u64 cycle_count;
	u32 hit_count;
};

struct TCXT
{
	Arena *arenas[2];
	debug_cycle_counter counters[DEBUG_CYCLE_COUNTER_COUNT];
};

global TCXT tcxt;

struct MK_Platform
{
	int argc;
	char **argv;
	Str8 app_dir;
	MK_Platform_api api;
	b32 initialized;
	b32 reloaded;
	u64 res;
	u64 cmt;
	void *memory;
};

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
#if defined (OS_LINUX)
#define BEGIN_TIMED_BLOCK(ID) u64 start_cycle_count_##ID = __rdtsc(); ++tcxt.counters[DEBUG_CYCLE_COUNTER_##ID].hit_count
#define END_TIMED_BLOCK(ID)  tcxt.counters[DEBUG_CYCLE_COUNTER_##ID].cycle_count += __rdtsc() - start_cycle_count_##ID
#else
#define BEGIN_TIMED_BLOCK(ID)
#define END_TIMED_BLOCK(ID)
#endif

internal void tcxt_init()
{
	for(u32 i = 0; i < ARRAY_LEN(tcxt.arenas); i ++)
	{
		tcxt.arenas[i] = arena_create(Megabytes(10), Megabytes(64));
	}
}

internal void process_debug_counters()
{
	for(i32 i = 0; i < ARRAY_LEN(tcxt.counters); i ++)
	{
		debug_cycle_counter *counter = tcxt.counters + i;
		
		//printf("%d: %lu\n", i, counter->cycle_count);
		counter->hit_count = 0;
		counter->cycle_count = 0;
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
			if(tcxt.arenas[i] == conflicts[j])
			{
				has_conflict = 1;
				break;
			}
		}
		if(!has_conflict)
		{
			out = tcxt.arenas[i];
		}
	}
	
	return out;
}

#define scratch_begin(conflicts, count) arena_temp_begin(tcxt_get_scratch(conflicts, count))
#define scratch_end(scratch) arena_temp_end(scratch);

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

typedef void (*update_and_render_fn)(MK_Platform *, char);

enum FILE_TYPE
{
  FILE_TYPE_TEXT,
  FILE_TYPE_BINARY,
  FILE_TYPE_COUNT
};

// ty pine
#if defined(OS_WIN32)
#define _file_open(file, filepath, mode) fopen_s(file, filepath, mode)
#elif defined (OS_LINUX) || defined (OS_APPLE)
#define _file_open(file, filepath, mode) *file = fopen(filepath, mode)
#endif

internal u8 *read_file(Arena *arena, const char *filepath, FILE_TYPE type)
{
  FILE *file;
  
  local_persist char *file_type_table[FILE_TYPE_COUNT] = 
  {
    "r",
    "rb"
  };
  
	if (access(filepath, F_OK) != 0)
	{
    file = fopen(filepath, "wb+");
    
    fclose(file);
  }
	
  _file_open(&file, filepath, file_type_table[type]);
  
  fseek(file, 0, SEEK_END);
  
  i32 len = ftell(file);
  //print("%d", len);
  
  fseek(file, 0, SEEK_SET);
  
  u8 *buffer = push_array(arena, u8, len);
  fread(buffer, sizeof(u8), len, file);
  
  fclose(file);
  
  return buffer;
}

internal void write_file(const char *filepath, FILE_TYPE type, void *data, size_t size)
{
	FILE *file;
	
	local_persist char *file_type_table[FILE_TYPE_COUNT] = 
  {
    "w",
    "wb"
  };
	
	_file_open(&file, filepath, file_type_table[type]);
	
	fwrite(data, size, 1, file);
	
	fclose(file);
	
}
#endif //MK_PLATFORM_H