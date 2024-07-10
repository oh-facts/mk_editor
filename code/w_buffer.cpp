W_Buffer w_buffer_alloc(Arena *arena, u64 size)
{
	W_Buffer out = {};
	out.base = push_array(arena, u8, size);
	out.size = size;
	return out;
}

void w_buffer_push(W_Buffer *buffer, char *c, int len)
{
	for(u32 i = 0; i < len; i++)
	{
		buffer->base[buffer->used++] = c[i];
	}
}

void w_buffer_pushf(W_Buffer *buffer, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	Arena_temp scratch = scratch_begin(0,0);
	char *str = push_array(scratch.arena, char, Megabytes(1));
	u32 len = stbsp_vsnprintf(str, Megabytes(1), fmt, args);
	
	va_end(args);
	
	w_buffer_push(buffer, str, len);
	
	scratch_end(&scratch);
}

void w_buffer_push_cursor_pos(W_Buffer *buf, i32 x, i32 y)
{
	w_buffer_pushf(buf,"\x1b[%d;%dH", y + 1, x + 1);
}