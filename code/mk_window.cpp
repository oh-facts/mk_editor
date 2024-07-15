void mk_window_submit(MK_Window *win)
{
	write(STDOUT_FILENO, win->wbuf.base, win->wbuf.used);
}

// TODO(mizu): Calculate this based on loc
#define CURS_X_PAD 7

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
	i32 i = 0;
	MK_Word_row *row = mk_get_word_row(&win->w_row_list, win->scroll_row);
	
	while (i < win->row)
	{
    if (row)
    {
			MK_Word *cur = row->first;
			w_buffer_pushf(&win->wbuf, "%5d| ", win->scroll_row + i + 1);
			while (cur)
			{
				if (cur->is_tab)
				{
					w_buffer_push(&win->wbuf, " ", 1);
				}
				else if (cur->is_space)
				{
					w_buffer_push(&win->wbuf, " ", 1);
				}
				else
				{
					w_buffer_push(&win->wbuf, &cur->c, 1);
				}
				cur = cur->next;
			}
			
			w_buffer_push(&win->wbuf, "\x1b[K", 3);
			if (i < win->row - 1)
			{
				w_buffer_push(&win->wbuf, "\r\n", 2);
			}
			i++;
			row = row->next;
    }
    else
    {
			w_buffer_push(&win->wbuf, "~", 1);
			w_buffer_push(&win->wbuf, "\x1b[K", 3);
			if (i < win->row - 1)
			{
				w_buffer_push(&win->wbuf, "\r\n", 2);
			}
			i++;
			
    }
	}
	
	//mk_window_submit(win);
	//INVALID_CODE_PATH();
	//printf("\r\n");
	
#endif
	w_buffer_push(&win->wbuf, "\r\n", 2);
	w_buffer_push(&win->wbuf, "\x1b[K", 3);
	w_buffer_push_cursor_pos(&win->wbuf, win->col - win->status_msg.len, win->row);
	w_buffer_push(&win->wbuf, (char*)win->status_msg.c, win->status_msg.len);
	
	//w_buffer_push(&win->wbuf, "~", 1);
}

void mk_set_win_size(MK_Window *win)
{
	winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	
	// space for status bar so minus
	
	win->row = ws.ws_row - 1;
	
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
	w_buffer_push_cursor_pos(&win->wbuf, win->cursor.col + CURS_X_PAD, win->cursor.row - win->scroll_row);
	w_buffer_push_show_cursor(&win->wbuf);
}

void mk_cursor_mv(MK_Window *win, char c)
{
	MK_KEY key = mk_key_from_char(c);
	MK_Cursor *curs = &win->cursor;
	
	
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
			curs->row ++;
			
		}break;
		case MK_KEY_LEFT:
		{
			if(curs->col != 0)
			{
				curs->col --;
			}
			else if(curs->row > 0)
			{
				curs->row --;
				curs->col = mk_get_word_row(&win->w_row_list, curs->row)->num_col;
			}
		}break;
		case MK_KEY_RIGHT:
		{
			MK_Word_row *row = mk_get_word_row(&win->w_row_list, win->cursor.row);
			
			if(curs->col < row->num_col)
			{
				curs->col ++;
			}
			else
			{
				curs->col = 0;
				curs->row++;
			}
		}break;
		case MK_KEY_HOME:
		{
			curs->col = 0;
		}break;
		case MK_KEY_END:
		{
			MK_Word_row *row = mk_get_word_row(&win->w_row_list, win->cursor.row);
			
			curs->col = row->num_col;
		}break;
		case MK_KEY_CTRL_HOME:
		{
			curs->row = 0;
		}break;
		case MK_KEY_CTRL_END:
		{
			curs->row = win->w_row_list.count - 1;
		}break;
		case MK_KEY_PAGE_UP:
		case MK_KEY_PAGE_DOWN:
		{
			// jump to page end if not at page end.
			// jump by a page if at page end.
			// acct for edge cases
		}break;
		case MK_KEY_DEL:
		{
			MK_Word_row *row = mk_get_word_row(&win->w_row_list, win->cursor.row);
			if(curs->col < row->num_col)
			{
				mk_word_remove(row, curs->col);
			}
			else
			{
				if(curs->row + 1 < win->w_row_list.count)
				{
					mk_word_row_remove(&win->w_row_list, curs->row + 1);
				}
				
				//curs->col += new_col;
			}
		}break;
		
		case MK_KEY_BACK_SPACE:
		{
			if(curs->col > 0)
			{
				MK_Word_row *row = mk_get_word_row(&win->w_row_list, win->cursor.row);
				
				mk_word_remove(row, --curs->col);
			}
			else if(curs->row > 0)
			{
				i32 new_col = win->w_row_list.row_indices[curs->row - 1]->num_col;
				
				mk_word_row_remove(&win->w_row_list, curs->row);
				curs->row--;
				curs->col = new_col;
			}
		}break;
		
		case MK_KEY_ENTER:
		{
			if(curs->col == 0)
			{
        mk_word_row_insert(win->arena, &win->w_row_list, win->cursor.row);
			}
			else
			{
				MK_Word_row *wrow = mk_get_word_row(&win->w_row_list, win->cursor.row);
				
        MK_Word_row *new_row = mk_word_row_insert(win->arena, &win->w_row_list, win->cursor.row + 1);
        
        MK_Word *word = wrow->first;
				
        for(i32 i = 0; i < curs->col - 1; i++)
        {
					word = word->next;
        }
				
        MK_Word *end = word->next;
        MK_Word *last_end = wrow->last;
				
        word->next = 0;
        wrow->last = word;
				
        i32 new_row_col_count = 0;
        for (MK_Word *count = end; count != 0; count = count->next)
        {
					new_row_col_count++;
        }
				
        wrow->num_col = curs->col;
        
        new_row->first = end;
        new_row->last = last_end;
        new_row->num_col = new_row_col_count;
        
			}
			curs->row++;
			curs->col = 0;
		}break;
		default:
		{
			if ((c >= 0 && c <= 31) || c == 127)
			{
			}
			else
			{
				MK_Word_row *row = mk_get_word_row(&win->w_row_list, win->cursor.row);
				
				MK_Word *node = mk_word_insert(win->arena, row, curs->col);
				node->c = c;
				
				curs->col++;
			}
			
		}break;
	}
	
	// bounds
	{
		if(curs->row > win->w_row_list.count - 1)
		{
			curs->row = win->w_row_list.count - 1 ;
		}
	}
	
	MK_Word_row *row = mk_get_word_row(&win->w_row_list, win->cursor.row);
	
	// snapping
	{
		if(curs->col > row->num_col)
		{
			curs->col = row->num_col;
		}
	}
	
	{
		
	}
	
}