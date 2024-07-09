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
	MK_KEY_BACK_SPACE,
	MK_KEY_ENTER,
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
				case '3':
				{
					out = MK_KEY_DEL;
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
	
	if(c == 127)
	{
		out = MK_KEY_BACK_SPACE;
	}
	else if(c == 13)
	{
		out = MK_KEY_ENTER;
	}
	
	return out;
}

struct MK_Cursor
{
	i32 row;
	i32 col;
};

struct MK_Word
{
	char c;
	b32 is_tab;
	b32 is_space;
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
	
	i32 num_col;
	i32 num_tab;
	i32 count;
};

struct MK_Word_row_list
{
	MK_Word_row *rows;
	i32 num_rows;
};



internal MK_Word_node *mk_word_push(Arena *arena, MK_Word_row *row)
{
	MK_Word_node *out = push_struct(arena, MK_Word_node);
	
	if(row->last)
	{
		row->last = row->last->next = out;
	}
	else
	{
		row->last = row->first = out;
	}
	row->count++;
	return out;
}

internal MK_Word_node *mk_word_insert(Arena *arena, MK_Word_row *row, i32 index)
{
	MK_Word_node *at = row->first;
	
	if(index == 0)
	{
		MK_Word_node *out = push_struct(arena, MK_Word_node);
		out->next = row->first;
		row->first = out;
		return out;
	}
	
	for(i32 i = 0; i < index - 1; i ++)
	{
		if(at == NULL || at->next == NULL)
		{
			printf("not possible dont\r\n");
			INVALID_CODE_PATH();
		}
		at = at->next;
	}
	
	MK_Word_node *out = push_struct(arena, MK_Word_node);
	out->next = at->next;
	at->next = out;
	
	return out;
}

internal void mk_word_remove(Arena *arena, MK_Word_row *row, i32 index)
{
	
	if (index == 0)
	{
		//MK_Word_node *rem = row->first;
		row->first = row->first->next;
		
		return;
	}
	
	MK_Word_node *at = row->first;
	
	for(i32 i = 0; i < index - 1; i ++)
	{
		if(at == NULL || at->next == NULL)
		{
			printf("%s %d\r\n", __FILE__, __LINE__);
			INVALID_CODE_PATH();
			return;
		}
		at = at->next;
	}
	
	//MK_Word_node *rem = at->next;
	at->next = at->next->next;
	
}

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
};

internal void mk_window_submit(MK_Window *win)
{
	write(STDOUT_FILENO, win->wbuf.base, win->wbuf.used);
}

internal MK_Word_row_list mk_word_list_from_buffer(Arena *arena, u8 *file)
{
	MK_Word_row_list w_row_list = {};
	
	w_row_list.rows = push_array(arena, MK_Word_row, 10000);
	MK_Word_row *row = w_row_list.rows;
	
	char *c = (char*)file;
	
	while(*c != '\0')
	{
		if(*c == '\t')
		{
			MK_Word_node *node = mk_word_push(arena, row);
			node->w.is_tab = 1;
			row->num_tab ++;
			c++;
			row->num_col++;
			//printf("\t");
		}
		else if(*c == ' ')
		{
			MK_Word_node *node = mk_word_push(arena, row);
			node->w.is_space = 1;
			c++;
			row->num_col++;
			//printf(" ");
		}
		else if(*c == '\n')
		{
			row++;
			w_row_list.num_rows++;
			c++;
			
			//printf("\r\n");
		}
		else if(*c == '\r')
		{
			c++;
		}
		else
		{
			row->num_col ++;
			MK_Word_node *node = mk_word_push(arena, row);
			node->w.c = *c;
			c++;
		}
		
	}
	
#if 0
	for(i32 i = 0; i < w_row_list.num_rows; i ++)
	{
		row = w_row_list.rows + i;
		MK_Word_node *cur = row->first;
		while(cur)
		{
			if(cur->w.is_tab)
			{
				printf(" ");
			}
			else if(cur->w.is_space)
			{
				printf(" ");
			}
			else
			{
				for(i32 j = 0; j < cur->w.str.len; j ++)
				{
					printf("%c", cur->w.str.c[j]);
				}
			}
			cur = cur->next;
		}
		
		printf("\r\n");
	}
	
	INVALID_CODE_PATH();
#endif
	
	return w_row_list;
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
#if 0
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
		
		w_buffer_push(&win->wbuf, "\x1b[K", 3);
		if(i < win->row - 1)
		{
			w_buffer_push(&win->wbuf, "\r\n", 2);
		}
		
	}
#else
	
	for(i32 i = 0; i < win->row; i ++)
	{
		MK_Word_row *row = win->w_row_list.rows + i + win->scroll_row;
		MK_Word_node *cur = row->first;
		while(cur)
		{
			if(cur->w.is_tab)
			{
				w_buffer_push(&win->wbuf, " ", 1);
			}
			else if(cur->w.is_space)
			{
				w_buffer_push(&win->wbuf, " ", 1);
			}
			else
			{
				w_buffer_push(&win->wbuf, &cur->w.c, 1);
			}
			cur = cur->next;
		}
		
		w_buffer_push(&win->wbuf, "\x1b[K", 3);
		if(i < win->row - 1)
		{
			w_buffer_push(&win->wbuf, "\r\n", 2);
		}
		
		//mk_window_submit(win);
		//INVALID_CODE_PATH();
		//printf("\r\n");
	}
#endif
	
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

internal MK_Word_row *mk_cursor_get_word_row(MK_Window *win)
{
	return win->w_row_list.rows + win->cursor.row;
}

internal void mk_cursor_mv(MK_Window *win, char c)
{
	MK_KEY key = mk_key_from_char(c);
	MK_Cursor *curs = &win->cursor;
	
	MK_Word_row *wrow = mk_cursor_get_word_row(win);
	
	switch(key)
	{
		case MK_KEY_UP:
		{
			if(curs->row != 0)
			{
				curs->row --;
			}
		}break;
		case MK_KEY_ENTER:
		{
			
		}break;
		case MK_KEY_DOWN:
		{
			curs->row ++;
			
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
			if(curs->col < wrow->num_col)
			{
				curs->col ++;
			}
			else
			{
				curs->row++;
			}
		}break;
		case MK_KEY_HOME:
		case MK_KEY_END:
		case MK_KEY_PAGE_UP:
		case MK_KEY_PAGE_DOWN:
		{
			
		}break;
		case MK_KEY_DEL:
		{if(wrow->num_col > 0 && curs->col < wrow->num_col)
			{
				mk_word_remove(win->arena, wrow, curs->col);
				wrow->num_col--;
			}
		}break;
		case MK_KEY_BACK_SPACE:
		{
			if(wrow->num_col > 0 && curs->col > 0)
			{
				mk_word_remove(win->arena, wrow, --curs->col);
				wrow->num_col--;
			}
		}break;
		default:
		{
			if(!wrow)
			{
				printf("ffff\r\n");
				INVALID_CODE_PATH();
				break;
			}
			
			MK_Word_node *node = mk_word_insert(win->arena, wrow, curs->col++);
			node->w.c = c;
			wrow->num_col++;
			
		}break;
	}
	
	wrow = mk_cursor_get_word_row(win);
	
	// snapping
	{
		if(curs->col > wrow->num_col)
		{
			curs->col = wrow->num_col;
		}
	}
	
}

void mk_insert_word()
{
	
}

#endif //MK_WINDOW_H
