/* date = July 7th 2024 10:42 am */

#ifndef MK_EDITOR_H
#define MK_EDITOR_H

#include "stdio.h"

#include "base_core.h"
#include "base_core.cpp"

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

#define MK_VERSION_MAJOR (0)
#define MK_VERSION_MINOR (0)
#define MK_VERSION_PATCH (1)

#define CTRL_KEY(k) ((k) & 0x1f)

#define submit_clear_screen() write(STDOUT_FILENO,"\x1b[2J", 4)
#define submit_reset_cursor() write(STDOUT_FILENO, "\x1b[H", 3)

enum MK_KEY
{
	MK_KEY_NONE,
	
	MK_KEY_UP,
	MK_KEY_DOWN,
	MK_KEY_RIGHT,
	MK_KEY_LEFT,
	
	MK_KEY_DEL,
	
	
	MK_KEY_COUNT
};

MK_KEY mk_key_from_char(char c)
{
	MK_KEY out = {};
	if(c == '\x1b')
	{
		char seq[3];
		read(STDIN_FILENO, &seq, 3);
		
		switch(seq[1])
		{
			case 'A':
			{
				out = MK_KEY_UP;
			}break;
			case 'B':
			{
				out = MK_KEY_DOWN;
			}break;
			case 'C':
			{
				out = MK_KEY_RIGHT;
			}break;
			case 'D':
			{
				out = MK_KEY_LEFT;
			}break;
			case '~':
			{
				out = MK_KEY_DEL;
			}break;
		}
		
	}
	
	return out;
}

struct MK_Buffer
{
	char *base;
	size_t used;
};

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

#define scratch_begin(conflicts, count) arena_temp_begin(tcxt_get_scratch(conflicts, count))
#define scratch_end(scratch) arena_temp_end(scratch);

internal void mk_buffer_push(MK_Buffer *buffer, char *c, int len)
{
	for(u32 i = 0; i < len; i ++)
	{
		buffer->base[buffer->used + i] = c[i];
	}
	
	buffer->used += len;
}

internal void mk_buffer_pushf(MK_Buffer *buffer, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	Arena_temp scratch = scratch_begin(0,0);
	char *str = push_array(scratch.arena, char, Megabytes(1));
	u32 len = stbsp_vsnprintf(str, Megabytes(1), fmt, args);
	
	va_end(args);
	
	mk_buffer_push(buffer, str, len);
	
	scratch_end(&scratch);
	
}

#define mk_buffer_push_reset_cursor(buffer) mk_buffer_push(buffer, "\x1b[H", 3)
#define mk_buffer_push_hide_cursor(buffer) mk_buffer_push(buffer, "\x1b[?25l", 6)
#define mk_buffer_push_show_cursor(buffer) mk_buffer_push(buffer, "\x1b[?25h", 6);
#define mk_buffer_push_clear_screen(buffer) mk_buffer_push(buffer, "\x1b[2J", 4);

internal void mk_buffer_submit(MK_Buffer *buffer)
{
	write(STDOUT_FILENO, buffer->base, buffer->used);
	buffer->used = 0;
	buffer->base = 0;
}

internal v2i get_win_size()
{
	winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	
	return (v2i){{ws.ws_col, ws.ws_row}};
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

struct MK_Platform
{
	void *memory;
	size_t mem_size;
	int argc;
	char **argv;
	Str8 app_dir;
};

typedef void (*update_and_render_fn)(MK_Platform *, char);

#endif //MK_EDITOR_H