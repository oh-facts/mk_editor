MK_Text_buffer mk_load_text_buffer(u8 *file)
{
	MK_Text_buffer out = {};
	out.row_arena = arena_create(ARENA_COMMIT_SIZE, Gigabytes(1));
	out.char_arena = arena_create(ARENA_COMMIT_SIZE, Gigabytes(1));
	out.used = 0;
	out.row = 0;
	out.row_max = 10000;
	
	out.rows = push_array(out.row_arena, MK_Text_row, out.row_max);
	
	char *c = (char*)file;
	char *f = c;
	i32 num_col = 0;
	while(1)
	{
		//printf("%c\r\n", *c);
		if(*c == '\r' || *c == '\n' || *c == '\0')
		{
			
			if(out.row >= out.row_max)
			{
				MK_Text_row *new_row = out.rows + out.row;
				new_row = push_array(out.row_arena, MK_Text_row, 10000);
				out.row_max += 10000;
			}
			
			MK_Text_row *row = out.rows + out.row++;
			//row->arena = arena_from_list(&out);
			row->str.c = push_array(out.char_arena, u8, num_col);
			mem_cpy(row->str.c, f, num_col);
			row->str.len = num_col;
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
		c++;
	}
	
	return out;
}