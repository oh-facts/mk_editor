/* date = July 9th 2024 0:36 am */

#ifndef W_BUFFER_H
#define W_BUFFER_H

struct W_Buffer
{
	u8 *base;
	u64 used;
	u64 size;
};

internal W_Buffer w_buffer_alloc(Arena *arena, u64 size);
internal void w_buffer_push(W_Buffer *buffer, char *c, int len);
internal void w_buffer_pushf(W_Buffer *buffer, const char *fmt, ...);
internal void w_buffer_push_cursor_pos(W_Buffer *buf, i32 x, i32 y);

#define w_buffer_push_reset_cursor(buffer) w_buffer_push(buffer, "\x1b[H", 3)
#define w_buffer_push_hide_cursor(buffer) w_buffer_push(buffer, "\x1b[?25l", 6)
#define w_buffer_push_show_cursor(buffer) w_buffer_push(buffer, "\x1b[?25h", 6);
#define w_buffer_push_clear_screen(buffer) w_buffer_push(buffer, "\x1b[2J", 4);

#endif //W_BUFFER_H