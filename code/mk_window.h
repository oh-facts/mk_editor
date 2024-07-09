/* date = July 8th 2024 2:57 pm */

#ifndef MK_WINDOW_H
#define MK_WINDOW_H

enum MK_KEY
{
	MK_KEY_NONE,
	
	MK_KEY_UP,
	MK_KEY_DOWN,
	MK_KEY_RIGHT,
	MK_KEY_LEFT,
	
	MK_KEY_HOME,
	MK_KEY_END,
	MK_KEY_PAGE_UP,
	MK_KEY_PAGE_DOWN,
	
	MK_KEY_DEL,
	
	MK_KEY_COUNT
};

internal MK_KEY mk_key_from_char(char c)
{
	MK_KEY out = {};
	if(c == '\x1b')
	{
		char seq[3];
		read(STDIN_FILENO, &seq, 3);
		
		if(seq[2] == '~')
		{
			switch(seq[1])
			{
				case '1':
				{
					out = MK_KEY_HOME;
				}break;
				case '4':
				{
					out = MK_KEY_END;
				}break;
				case '5':
				{
					out = MK_KEY_PAGE_UP;
				}break;
				case '6':
				{
					out = MK_KEY_PAGE_DOWN;
				}break;
				case '7':
				{
					out = MK_KEY_HOME;
				}break;
				case '8':
				{
					out = MK_KEY_END;
				}break;
			}
		}
		else
		{
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
				case 'H':
				{
					out = MK_KEY_HOME;
				}break;
				case 'F':
				{
					out = MK_KEY_END;
				}break;
			}
		}
	}
	
	return out;
}

struct MK_Cursor
{
	i32 row;
	i32 col;
};

struct MK_Row
{
	i32 num_tab;
	i32 col;
	char *c;
};

struct MK_Buffer
{
	MK_Row *rows;
	i32 num_rows;
};

struct MK_Word
{
	Str8 str;
	b32 is_tab;
};

struct MK_Word_node
{
	MK_Word_node *next;
	MK_Word w;
};

struct MK_Word_row
{
	MK_Word_node *first;
	MK_Word_node *last;
	i32 count;
};



struct MK_Window
{
	i32 scroll_row;
	i32 row;
	i32 col;
	MK_Cursor cursor;
	W_Buffer wbuf;
	MK_Buffer *buf;
	MK_Buffer *ren_buf;
	
	Str8 status_msg;
};

internal void mk_buffer_print_raw(MK_Buffer *buf)
{
	for(i32 i = 0; i < buf->num_rows; i ++)
	{
		MK_Row *row = buf->rows + i;
		for(i32 j = 0; j < row->col; j ++)
		{
			printf("%c",row->c[j]);
		}
		printf("\r\n");
	}
}

internal MK_Buffer mk_buffer_from_file(Arena *arena, u8 *file)
{
	MK_Buffer out = {};
	char *c = (char*)file;
	char *f = c;
	i32 num_col = 0;
	out.rows = push_array(arena, MK_Row, 10000);
	
	while(1)
	{
		if(*c == '\r' || *c == '\n' || *c == '\0')
		{
			MK_Row *row = out.rows + out.num_rows++;
			row->c = push_array(arena, char, num_col);
			mem_cpy(row->c, f, num_col);
			row->col = num_col;
			num_col = 0;
			
			if(*c == '\0')
			{
				break;
			}
			
			if (*c == '\r' && *(c + 1) == '\n')
			{
				c++;
			}
			f = c + 1;
		}
		else
		{
			num_col++;
		}
		
	}
	
	return out;
}

internal void mk_window_render(MK_Window *win)
{
#if 0
	for(int y = 0; y < win->row - 1; y ++)
	{
		w_buffer_push(&win->wbuf, "\x1b[K", 3);
		w_buffer_push(&win->wbuf, "~\r\n", 3);
	}
	w_buffer_push(&win->wbuf, "\x1b[K", 3);
#endif
	
	for(i32 i = 0; i < win->row ; i ++)
	{
		MK_Row *row = win->buf->rows + i + win->scroll_row;
		
		for(i32 j = 0; j < row->num_tab; j ++)
		{
			for(i32 k = 0; k < global_config.tab_width; k++)
			{
				w_buffer_push(&win->wbuf, " ", 2);
			}
		}
		
		for(i32 j = row->num_tab; j < row->col - row->num_tab; j ++)
		{
			w_buffer_push(&win->wbuf, &row->c[j], 1);
		}
		//w_buffer_push(&win->wbuf, row->c, row->col);
		
		
		
		w_buffer_push(&win->wbuf, "\x1b[K", 3);
		
		if(i < win->row - 1)
		{
			w_buffer_push(&win->wbuf, "\r\n", 2);
		}
		
	}
	
	w_buffer_push_cursor_pos(&win->wbuf, win->col - win->status_msg.len, win->row - 1);
	w_buffer_push(&win->wbuf, (char*)win->status_msg.c, win->status_msg.len);
	
	//w_buffer_push(&win->wbuf, "~", 1);
}

internal void mk_set_win_size(MK_Window *win)
{
	winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	
	win->row = ws.ws_row;
	win->col = ws.ws_col;
}

internal void mk_window_submit(MK_Window *win)
{
	write(STDOUT_FILENO, win->wbuf.base, win->wbuf.used);
}

internal void mk_window_begin_render(MK_Window *win)
{
	
	if(win->cursor.row < win->scroll_row)
	{
		win->scroll_row = win->cursor.row;
	}
	if(win->cursor.row >= win->scroll_row + win->row)
	{
		win->scroll_row = win->cursor.row - win->row + 1;
	}
	
	w_buffer_push_hide_cursor(&win->wbuf);
	w_buffer_push_reset_cursor(&win->wbuf);
}

internal void mk_window_end_render(MK_Window *win)
{
	w_buffer_push_cursor_pos(&win->wbuf, win->cursor.col, win->cursor.row - win->scroll_row);
	w_buffer_push_show_cursor(&win->wbuf);
}

internal MK_Row *mk_cursor_get_row(MK_Window *win)
{
	return win->buf->rows + win->cursor.row;
}

internal void mk_cursor_mv(MK_Window *win, char c)
{
	MK_KEY key = mk_key_from_char(c);
	MK_Cursor *curs = &win->cursor;
	
	MK_Row *row = mk_cursor_get_row(win);
	
	switch(key)
	{
		case MK_KEY_UP:
		{
			if(curs->row != 0)
			{
				curs->row --;
			}
		}break;
		case MK_KEY_DOWN:
		{
			if(curs->row < win->buf->num_rows)
			{
				curs->row ++;
			}
		}break;
		case MK_KEY_LEFT:
		{
			if(curs->col != 0)
			{
				curs->col --;
			}
		}break;
		case MK_KEY_RIGHT:
		{
			if(curs->col < row->col)
			{
				curs->col ++;
			}
			else if(curs->row < win->buf->num_rows)
			{
				curs->row++;
			}
		}break;
		
	}
	
	row = mk_cursor_get_row(win);
	
	if(curs->col > row->col)
	{
		curs->col = row->col;
	}
	
}
#endif //MK_WINDOW_H
