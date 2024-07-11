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
	
	MK_Word_row_node *row_node = mk_get_word_row(&win->w_row_list, win->scroll_row);
	
	while(row_node && i < win->row)
	{
		MK_Word_row *row = &row_node->row;
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
		i++;
		row_node = row_node->next;
	}
	
	
	//mk_window_submit(win);
	//INVALID_CODE_PATH();
	//printf("\r\n");
	
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
	
	MK_Word_row_node *wrow_node = mk_get_word_row(&win->w_row_list, win->cursor.row);
	MK_Word_row *wrow = &wrow_node->row;
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
			if(curs->col == 0)
			{
        mk_word_row_insert(win->arena, &win->w_row_list, win->cursor.row);
			}
			else
			{
        MK_Word_row_node *new_row_node = mk_word_row_insert(win->arena, &win->w_row_list, win->cursor.row + 1);
        MK_Word_row *new_row = &new_row_node->row;
				
        MK_Word_node *word_node = wrow->first;
				
        for(i32 i = 0; i < curs->col - 1; i++)
        {
					word_node = word_node->next;
        }
				
        MK_Word_node *end_nodes = word_node->next;
        MK_Word_node *last_end_node = wrow->last;
				
        word_node->next = nullptr;
        wrow->last = word_node;
				
        i32 new_row_col_count = 0;
        for (MK_Word_node *count_node = end_nodes; count_node != 0; count_node = count_node->next)
        {
					new_row_col_count++;
        }
				
        wrow->num_col = curs->col;
        
        new_row->first = end_nodes;
        new_row->last = last_end_node;
        new_row->num_col = new_row_col_count;
        //new_row->num_tab = 0;
			}
			curs->row++;
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
		{
			curs->row = 0;
		}break;
		case MK_KEY_END:
		{
			curs->row = win->w_row_list.count - 1;
		}break;
		case MK_KEY_PAGE_UP:
		case MK_KEY_PAGE_DOWN:
		{
			
		}break;
		case MK_KEY_DEL:
		{
			if(wrow->num_col > 0 && curs->col < wrow->num_col)
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
			
			if(curs->col == 0)
			{
				mk_word_row_remove(win->arena, &win->w_row_list, curs->row);
			}
		}break;
		
		default:
		{
			if ((c >= 0 && c <= 31) || c == 127)
			{
			}
			else
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
	
	wrow_node = mk_get_word_row(&win->w_row_list, win->cursor.row);
	wrow = &wrow_node->row;
	
	// snapping
	{
		if(curs->col > wrow->num_col)
		{
			curs->col = wrow->num_col;
		}
	}
	
	
}