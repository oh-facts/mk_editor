MK_Word_node *mk_word_push(Arena *arena, MK_Word_row *row)
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

MK_Word_node *mk_word_insert(Arena *arena, MK_Word_row *row, i32 index)
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

void mk_word_remove(Arena *arena, MK_Word_row *row, i32 index)
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

MK_Word_row_list mk_word_list_from_buffer(Arena *arena, u8 *file)
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
