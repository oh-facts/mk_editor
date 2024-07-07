/* date = July 7th 2024 10:42 am */

#ifndef MK_EDITOR_H
#define MK_EDITOR_H
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

internal void mk_buffer_push(MK_Buffer *buffer, char *c, int len)
{
	for(u32 i = 0; i < len; i ++)
	{
		buffer->base[buffer->used + i] = c[i];
	}
	
	buffer->used += len;
}

internal void mk_buffer_pushf(MK_Buffer *buffer, Arena *arena, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	char str[256];
	u32 len = stbsp_vsnprintf(str, 256, fmt, args);
	
	va_end(args);
	
	mk_buffer_push(buffer, str, len);
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

struct MK_Platform
{
	void *memory;
	size_t mem_size;
};

typedef void (*update_and_render_fn)(MK_Platform *, char);

#endif //MK_EDITOR_H
