/* date = July 8th 2024 2:57 pm */

#ifndef MK_WINDOW_H
#define MK_WINDOW_H

struct MK_Cursor
{
	i32 row;
	i32 col;
};

struct MK_Window
{
	Arena *arena;
	
	i32 scroll_row;
	i32 row;
	i32 col;
	
	MK_Cursor cursor;
	W_Buffer wbuf;
	
	MK_Word_row_list w_row_list;
	Str8 status_msg;
	
	Str8 file_name;
};

internal void mk_window_submit(MK_Window *win);
internal void mk_window_render(MK_Window *win);
internal void mk_set_win_size(MK_Window *win);
internal void mk_window_begin_render(MK_Window *win);
internal void mk_window_end_render(MK_Window *win);

internal void mk_cursor_mv(MK_Window *win, char c);

#endif //MK_WINDOW_H
