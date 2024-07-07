#include "base_core.h"
#include "base_core.cpp"

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
#include "base_string.h"
#include "base_string.cpp"

#include "base_math.h"
#include "base_math.cpp"

#include "stdio.h"
#include <termios.h>

#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
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

struct MK_Editor
{
	v2i pos;
	v2i size;
};

global MK_Editor editor;

struct MK_Buffer
{
	char *base;
	size_t used;
};

internal void mk_buffer_push(MK_Buffer *buffer, char *c, int len)
{
	for(u32 i = 0; i < len; i ++)
	{
		buffer->base[buffer->used + i] = c[i];
	}
	
	buffer->used += len;
}

#define mk_buffer_push_reset_cursor(buffer) mk_buffer_push(buffer, "\x1b[H", 3)
#define mk_buffer_push_hide_cursor(buffer) mk_buffer_push(buffer, "\x1b[?25l", 6)
#define mk_buffer_push_show_cursor(buffer) mk_buffer_push(buffer, "\x1b[?25h", 6);
#define mk_buffer_push_clear_screen(buffer) mk_buffer_push(buffer, "\x1b[2J", 4);

internal void mk_buffer_pushf(MK_Buffer *buffer, Arena *arena, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	char str[256];
	u32 len = stbsp_vsnprintf(str, 256, fmt, args);
	
	va_end(args);
	
	mk_buffer_push(buffer, str, len);
}

internal void mk_buffer_submit(MK_Buffer *buffer)
{
	write(STDOUT_FILENO, buffer->base, buffer->used);
	buffer->used = 0;
	buffer->base = 0;
}

v2i get_win_size()
{
	winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	
	return (v2i){{ws.ws_col, ws.ws_row}};
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

int main()
{
	Arena arena = {};
	size_t mem_size = Megabytes(10);
	void *memory = calloc(0, mem_size);
	arena_innit(&arena, mem_size, memory);
	
	termios start;
	tcgetattr(STDIN_FILENO, &start);
	
	enable_raw_mode();
	submit_clear_screen();
	
	char c;
	while(read(STDIN_FILENO, &c, 1) == 1)
	{
		Arena_temp temp = arena_temp_begin(&arena);
		
		MK_Buffer buf = {};
		buf.base = push_array(&arena, char, Megabytes(1));
		
		mk_buffer_push_clear_screen(&buf);
		mk_buffer_push_hide_cursor(&buf);
		mk_buffer_push_reset_cursor(&buf);
		
		editor.size = get_win_size();
		
		if(c == CTRL_KEY('q'))
		{
			break;
		}
		
		MK_KEY mv_key = mk_key_from_char(c);
		
		switch(mv_key)
		{
			case MK_KEY_UP:
			{
				if(editor.pos.y > 0)
				{
					editor.pos.y --;
				}
			}break;
			case MK_KEY_DOWN:
			{
				if(editor.pos.y < editor.size.y - 1)
				{
					editor.pos.y ++;
				}
			}break;
			case MK_KEY_RIGHT:
			{
				if(editor.pos.x < editor.size.x - 1)
				{
					editor.pos.x ++;
				}
			}break;
			case MK_KEY_LEFT:
			{
				if(editor.pos.x > 0)
				{
					editor.pos.x --;
				}
			}break;
			default:
			{
				
			}
		}
		
		// rows
		{
			for(i32 i = 0; i < editor.size.y - 1; i ++)
			{
				mk_buffer_pushf(&buf, &arena, "~\r\n");
			}
			mk_buffer_pushf(&buf, &arena, "~");
		}
		
		mk_buffer_pushf(&buf, &arena, "\x1b[%d;%dH", editor.pos.y + 1, editor.pos.x + 1);
		
		mk_buffer_pushf(&buf, &arena, "%c", c);
		
		mk_buffer_push_show_cursor(&buf);
		mk_buffer_submit(&buf);
		
		arena_temp_end(&temp);
	}
	
	submit_clear_screen();
	submit_reset_cursor();
	
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &start);
	return 0;
}