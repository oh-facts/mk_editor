/* date = July 8th 2024 2:56 pm */

#ifndef MK_BUFFER_H
#define MK_BUFFER_H

struct MK_Buffer
{
	char *base;
	u64 used;
};

#define LINE_MAX_LEN (200)

internal void mk_buffer_push(MK_Buffer *buffer, char *c, int len)
{
	u32 bytes_used = 0;
	u32 line_len = 0;
	
	for(u32 i = 0; i < len; i ++)
	{
		if(line_len > LINE_MAX_LEN)
		{
			buffer->base[buffer->used + bytes_used] = '\r';
			bytes_used++;
			buffer->base[buffer->used + bytes_used] = '\n';
			bytes_used++;
			buffer->base[buffer->used + bytes_used] = c[i];
			bytes_used++;
			line_len = 0;
		}
		else
		{
			if(c[i] == '\t')
			{
				buffer->base[buffer->used + bytes_used] = ' ';
				bytes_used++;
				line_len++;
			}
			else if(c[i] == '\r')
			{
				
			}
			else if(c[i] == '\n')
			{
				mk_buffer_push(buffer, "\x1b[K",3);
				buffer->base[buffer->used + bytes_used] = '\r';
				bytes_used++;
				buffer->base[buffer->used + bytes_used] = '\n';
				bytes_used++;
				line_len = 0;
			}
			else
			{
				buffer->base[buffer->used + bytes_used] = c[i];
				bytes_used++;
				line_len++;
			}
		}
		
	}
	
	buffer->used += bytes_used;
}

internal void mk_buffer_push_row(MK_Buffer *buffer, char *c, int len)
{
	mk_buffer_push(buffer, c, len);
	mk_buffer_push(buffer, "\x1b[K", 3);
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

internal void mk_buffer_push_set_cursor(MK_Buffer *buffer, v2i pos)
{
	mk_buffer_pushf(buffer,"\x1b[%d;%dH", pos.y, pos.x);
}

#endif //MK_BUFFER_H
