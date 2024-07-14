void mk_window_submit(MK_Window *win)
{
	write(STDOUT_FILENO, win->wbuf.base, win->wbuf.used);
}

void mk_window_render(MK_Window *win)
{
#if 0
	for(int y = 0; y < win->row - 1; y ++)
	{
		w_buffer_push(&win->wbuf, "\x1b[K", 3);
		w_buffer_push(&win->wbuf, "~\r\n", 3);
	}
	w_buffer_push(&win->wbuf, "\x1b[K", 3);
#endif
#if 1
	for(i32 i = 0; i < win->row ; i ++)
	{
		MK_Text_row *row = win->tbuf.rows + i + win->scroll_row;
		
		for(i32 i = 0; i < row->str.len; i ++)
		{
			char c = row->str.c[i];
			if(c == '\t')
			{
				w_buffer_push(&win->wbuf, " ", 1);
			}
			else
			{
				w_buffer_push(&win->wbuf, &c, 1);
			}
		}
		
		//printf("%c\r\n", *row->str.c);
		//INVALID_CODE_PATH();
		
		w_buffer_push(&win->wbuf, "\x1b[K", 3);
		if(i < win->row - 1)
		{
			w_buffer_push(&win->wbuf, "\r\n", 2);
		}
		
	}
	
	//INVALID_CODE_PATH();
#endif
	w_buffer_push_cursor_pos(&win->wbuf, win->col - win->status_msg.len, win->row - 1);
	w_buffer_push(&win->wbuf, (char*)win->status_msg.c, win->status_msg.len);
	
	//w_buffer_push(&win->wbuf, "~", 1);
}

void mk_set_win_size(MK_Window *win)
{
	winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	
	win->row = ws.ws_row;
	win->col = ws.ws_col;
}

void mk_window_begin_render(MK_Window *win)
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

void mk_window_end_render(MK_Window *win)
{
	w_buffer_push_cursor_pos(&win->wbuf, win->cursor.col, win->cursor.row - win->scroll_row);
	w_buffer_push_show_cursor(&win->wbuf);
}

void mk_cursor_mv(MK_Window *win, char c)
{
	MK_KEY key = mk_key_from_char(c);
	MK_Cursor *curs = &win->cursor;
	MK_Text_row *row = win->tbuf.rows + curs->row;
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
			for(i32 i = win->tbuf.row; i >= curs->row; i --)
			{
				MK_Text_row *row = win->tbuf.rows + i;
				mem_cpy(row + 1, row, sizeof(MK_Text_row));
				
			}
			row->str.c = push_array(win->tbuf.char_arena, u8, 300);
			row->str.len = 0;
			win->tbuf.row++;
			curs->row++;
		}break;
		
		case MK_KEY_DOWN:
		{
			if(curs->row < win->tbuf.row - 1)
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
			if(curs->col < row->str.len)
			{
				curs->col++;
			}
			else
			{
				if(curs->row < win->tbuf.row - 1)
				{
					curs->row++;
				}
			}
		}break;
		case MK_KEY_HOME:
		{
			curs->row = 0;
		}break;
		case MK_KEY_END:
		{
			curs->row = win->tbuf.row - 1;//win->w_row_list.count - 1;
		}break;
		case MK_KEY_PAGE_UP:
		case MK_KEY_PAGE_DOWN:
		{
			
		}break;
		
		case MK_KEY_DEL:
		{
			if (curs->col < row->str.len) 
			{
				Arena_temp scratch = scratch_begin(0, 0);
				
				char *buf = push_array(scratch.arena, char, 300);
				mem_cpy(buf, row->str.c, row->str.len);
				
				mem_cpy(row->str.c + curs->col, (u8*)buf + curs->col + 1, row->str.len - curs->col - 1);
				
				row->str.len--;
				scratch_end(&scratch);
			}
		} break;
		
		case MK_KEY_BACK_SPACE:
		{
			if (curs->col > 0) 
			{
				Arena_temp scratch = scratch_begin(0, 0);
				
				char *buf = push_array(scratch.arena, char, 300);
				mem_cpy(buf, row->str.c, row->str.len);
				
				mem_cpy(row->str.c + curs->col - 1, (u8*)buf + curs->col, row->str.len - curs->col);
				
				row->str.len--;
				curs->col--;
				scratch_end(&scratch);
			}
		} break;
		
		default:
		{
			Arena_temp scratch = scratch_begin(0,0);
			
			char *buf = push_array(scratch.arena, char, 300);
			mem_cpy(buf, row->str.c, row->str.len);
			
			mem_cpy(row->str.c, buf, curs->col);
			row->str.c[curs->col] = c;
			mem_cpy(row->str.c + curs->col + 1, (u8*)buf + curs->col, row->str.len - curs->col);
			
			row->str.len++;
			scratch_end(&scratch);
			curs->col++;
		}break;
	}
	
	row = win->tbuf.rows + curs->row;
	
	// snapping
	if(curs->col > row->str.len)
	{
		curs->col = row->str.len; 
	}
	
}