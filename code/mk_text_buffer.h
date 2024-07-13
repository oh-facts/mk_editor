/* date = July 10th 2024 11:52 am */

#ifndef MK_TEXT_BUFFER_H
#define MK_TEXT_BUFFER_H

struct MK_Text_row
{
	Arena *arena;
	Str8 str;
};

struct MK_Text_buffer
{
	Arena *free_list;
	
	Arena *row_arena;
	MK_Text_row *rows;
	i32 row;
	i32 row_max;
};

internal void mk_buffer_print_raw(MK_Text_buffer *buf)
{
	for(i32 i = 0; i < buf->row; i ++)
	{
		MK_Text_row *row = buf->rows + i;
		for(i32 j = 0; j < row->str.len; j ++)
		{
			printf("%c",row->str.c[j]);
		}
		printf("\r\n");
	}
}

internal Arena *arena_from_list(MK_Text_buffer *tbuf)
{
	Arena *out = tbuf->free_list;
	
	if(out)
	{
		tbuf->free_list = tbuf->free_list->next;
		out->used = 0;
	}
	else
	{
		out = arena_create(200, Kilobytes(3));
	}
	
	return out;
}

internal void arena_release(MK_Text_buffer *tbuf, Arena *arena)
{
	arena->next = tbuf->free_list;
	tbuf->free_list = arena;
}

internal MK_Text_buffer mk_load_text_buffer(u8 *file);

#endif //MK_TEXT_BUFFER_H