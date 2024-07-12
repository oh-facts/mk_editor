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
	row->num_col++;
	//row->count++;
	return out;
}

MK_Word_row_node *mk_word_row_push(Arena *arena, MK_Word_row_list *list)
{
	MK_Word_row_node *out = push_struct(arena, MK_Word_row_node);
	
	if(list->last)
	{
		out->prev = list->last;
		list->last = list->last->next = out;
	}
	else
	{
		list->last = list->first = out;
	}
	list->row_index_nodes[list->count] = out;
	
	list->count++;
	
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

MK_Word_row_node *mk_word_row_insert(Arena *arena, MK_Word_row_list *list, i32 index)
{
	if(index < 0 || index > list->count)
	{
		printf("Invalid index\r\n");
		INVALID_CODE_PATH();
	}
	
	MK_Word_row_node *out = push_struct(arena, MK_Word_row_node);
	if(index == 0)
	{
		if(list->first)
		{
			out->next = list->first;
			list->first->prev = out;
			list->first = out;
		}
		else
		{
			list->first = list->last = out;
		}
	}
	else if(index == list->count)
	{
		out->prev = list->last;
		list->last->next = out;
		list->last = out;
	}
	else
	{
		MK_Word_row_node *at = list->row_index_nodes[index];
		out->next = at;
		out->prev = at->prev;
		if(at->prev) at->prev->next = out;
		at->prev = out;
	}
	
	for(i32 i = list->count; i > index; i--)
	{
		list->row_index_nodes[i] = list->row_index_nodes[i - 1];
	}
	list->row_index_nodes[index] = out;
	
	list->count++;
	
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

void mk_word_row_remove(Arena *arena, MK_Word_row_list *list, i32 index)
{
	if(index < 0 || index >= list->count)
	{
		printf("Invalid index\r\n");
		INVALID_CODE_PATH();
		return;
	}
	
	MK_Word_row_node *rem = list->row_index_nodes[index];
	
	if(rem->prev)
	{
		rem->prev->next = rem->next;
	}
	else
	{
		list->first = rem->next;
	}
	
	if(rem->next)
	{
		rem->next->prev = rem->prev;
	}
	else
	{
		list->last = rem->prev;
	}
	
	for(i32 i = index; i < list->count - 1; i++)
	{
		list->row_index_nodes[i] = list->row_index_nodes[i + 1];
	}
	
	list->count--;
	
}

MK_Word_row_list mk_word_list_from_buffer(Arena *arena, u8 *file)
{
	MK_Word_row_list w_row_list = {};
	
	char *c = (char*)file;
	
	MK_Word_row_node *row_node = mk_word_row_push(arena, &w_row_list);
	
	while(*c != '\0')
	{
		MK_Word_row *row = &row_node->row;
		
		if(*c == '\t')
		{
			MK_Word_node *node = mk_word_push(arena, row);
			node->w.is_tab = 1;
			row->num_tab ++;
			c++;
			//printf("\t");
		}
		else if(*c == ' ')
		{
			MK_Word_node *node = mk_word_push(arena, row);
			node->w.is_space = 1;
			c++;
			//printf(" ");
		}
		else if(*c == '\n')
		{
			row_node = mk_word_row_push(arena, &w_row_list);
			c++;
			
			//printf("\r\n");
		}
		else if(*c == '\r')
		{
			c++;
		}
		else
		{
			
			MK_Word_node *node = mk_word_push(arena, row);
			node->w.c = *c;
			c++;
			
		}
		
	}
	
	//printf("%d\r\n", w_row_list.count);
	//INVALID_CODE_PATH();
	
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

MK_Word_row_node *mk_get_word_row(MK_Word_row_list* list, i32 index)
{
	
#if 0
	MK_Word_row_node *row_node = list->first; // + i + win->scroll_row;
	int i = 0;
	while(row_node)
	{
		
		if(i == index)
		{
			break;
		}
		
		i++;
		row_node = row_node->next;
	}
#else
	MK_Word_row_node *row_node = list->row_index_nodes[index];
	
#endif
	
	return row_node;
}