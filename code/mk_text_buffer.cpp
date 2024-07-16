MK_Word *mk_word_push(Arena *arena, MK_Word_row *row)
{
	MK_Word *out = push_struct(arena, MK_Word);
	
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

MK_Word_row *mk_word_row_push(Arena *arena, MK_Word_row_list *list)
{
	MK_Word_row *out = push_struct(arena, MK_Word_row);
	
	if(list->last)
	{
		list->last = list->last->next = out;
	}
	else
	{
		list->last = list->first = out;
	}
	
	if(list->row_indices_cap <= list->count)
	{
		MK_Word_row **row_index = list->row_indices + list->row_indices_cap;
		row_index = push_array(list->row_indices_arena, MK_Word_row *, 10000);
		list->row_indices_cap += 10000;
	}
	
	list->row_indices[list->count] = out;
	
	list->count++;
	
	return out;
}

MK_Word *mk_word_insert(Arena *arena, MK_Word_row *row, i32 index)
{
	MK_Word *out = push_struct(arena, MK_Word);
	out->next = 0;
	out->c = 0;
	out->len = 0;
	out->is_tab = false;
	out->is_space = false;
	
	if (index <= 0)
	{
		out->next = row->first;
		row->first = out;
		if (row->last == 0)
		{
			row->last = out;
		}
	}
	else
	{
		MK_Word *prev = row->first;
		for (i32 i = 1; i < index && prev->next != 0; ++i)
		{
			prev = prev->next;
		}
		out->next = prev->next;
		prev->next = out;
		if (prev == row->last)
		{
			row->last = out;
		}
	}
	
	row->num_col++;
	return out;
}

MK_Word_row *mk_word_row_insert(Arena *arena, MK_Word_row_list *list, i32 index)
{
	MK_Word_row *out = push_struct(arena, MK_Word_row);
	if(index == 0)
	{
    if(list->count > 0)
    {
			out->next = list->first;
			list->first = out;
    }
    else
    {
			list->first = list->last = out;
    }
	}
	else if(index == list->count)
	{
    list->last->next = out;
    list->last = out;
	}
	else
	{
    MK_Word_row *at = list->row_indices[index];
    out->next = at;
    list->row_indices[index - 1]->next = out;
	}
	
	//mk_update_row_index_cache(list, index);
	for(i32 i = list->count; i > index; i--)
	{
    list->row_indices[i] = list->row_indices[i - 1];
	}
	list->row_indices[index] = out;
	
	
	
	if(list->row_indices_cap <= list->count)
	{
		MK_Word_row **row_index = list->row_indices + list->row_indices_cap;
		row_index = push_array(list->row_indices_arena, MK_Word_row *, 10000);
		list->row_indices_cap += 10000;
	}
	
	list->count++;
	
	return out;
	
}

void mk_word_remove(MK_Word_row *row, i32 index)
{
	if (row->first == 0)
	{
		printf("don't come here\r\n");
		INVALID_CODE_PATH();
		
	}
	
	if (index <= 0)
	{
		MK_Word *to_remove = row->first;
		row->first = to_remove->next;
		if (row->last == to_remove)
		{
			row->last = 0;
		}
	}
	else
	{
		MK_Word *prev = row->first;
		for (i32 i = 1; i < index && prev->next != 0; ++i)
		{
			prev = prev->next;
		}
		if (prev->next != 0)
		{
			MK_Word *to_remove = prev->next;
			prev->next = to_remove->next;
			if (row->last == to_remove)
			{
				row->last = prev;
			}
		}
	}
	
	row->num_col--;
}

void mk_word_row_remove(MK_Word_row_list *list, i32 index)
{
	if (index < 0 || index >= list->count)
	{
		INVALID_CODE_PATH();
		return;
	}
	
	MK_Word_row *to_remove = list->row_indices[index];
	
	if (index == 0)
	{
		list->first = to_remove->next;
		if (list->last == to_remove)
		{
			list->last = 0;
		}
	}
	else
	{
		MK_Word_row *prev = list->row_indices[index - 1];
		prev->next = to_remove->next;
		if (list->last == to_remove)
		{
			list->last = prev;
		}
		
		if (to_remove->first)
		{
			if (prev->last)
			{
				prev->last->next = to_remove->first;
			}
			else
			{
				prev->first = to_remove->first;
			}
			prev->last = to_remove->last;
			prev->num_col += to_remove->num_col;
		}
	}
	
	for (i32 i = index; i < list->count - 1; ++i)
	{
		list->row_indices[i] = list->row_indices[i + 1];
	}
	list->count--;
}

MK_Word_row_list mk_word_list_from_file(Arena *arena, u8 *file)
{
	
	MK_Word_row_list w_row_list = mk_word_list(arena);
	
	char *c = (char*)file;
	
	MK_Word_row *row = mk_word_row_push(arena, &w_row_list);
	
	while(*c != '\0')
	{
		
		if(*c == '\t')
		{
			MK_Word *word = mk_word_push(arena, row);
			word->is_tab = 1;
			row->num_tab ++;
			c++;
			//printf("\t");
		}
		else if(*c == ' ')
		{
			MK_Word *word = mk_word_push(arena, row);
			word->is_space = 1;
			c++;
			//printf(" ");
		}
		else if(*c == '\n')
		{
			row = mk_word_row_push(arena, &w_row_list);
			c++;
			
			//printf("\r\n");
		}
		else if(*c == '\r')
		{
			c++;
		}
		else
		{
			
			MK_Word *word = mk_word_push(arena, row);
			word->c = *c;
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

MK_Word_row_list mk_word_list_new_file(Arena *arena)
{
	MK_Word_row_list out = mk_word_list(arena);
	mk_word_row_push(arena, &out);
	
	return out;
}

MK_Word_row_list mk_word_list(Arena *arena)
{
	MK_Word_row_list w_row_list = {};
	w_row_list.row_indices_arena = arena_create(ARENA_COMMIT_SIZE, Gigabytes(1));
	w_row_list.row_indices = push_array(w_row_list.row_indices_arena, MK_Word_row *, 10000);
	w_row_list.row_indices_cap = 10000;
	
	return w_row_list;
}

MK_Word_row *mk_get_word_row(MK_Word_row_list* list, i32 index)
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
	MK_Word_row *row = list->row_indices[index];
	
#endif
	
	return row;
}

File_content mk_write_row_list_to_file(Arena *arena, MK_Word_row_list *list)
{
	File_content out = {};
	out.data = push_array(arena, char, arena->cmt - arena->used);
	MK_Word_row *row = list->first;
	i32 count = 0;
	while (row)
	{
    MK_Word *cur = row->first;
		
		while (cur)
		{
			if (cur->is_tab)
			{
				out.data[count++] = '\t';
			}
			else if (cur->is_space)
			{
				out.data[count++] = ' ';
			}
			else
			{
				out.data[count++] = cur->c;
			}
			cur = cur->next;
		}
		
		out.data[count++] = '\r';
		out.data[count++] = '\n';
		
		row = row->next;
	}
	out.size = count;
	return out;
	
}