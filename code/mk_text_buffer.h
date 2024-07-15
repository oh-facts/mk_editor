/* date = July 10th 2024 11:52 am */

#ifndef MK_TEXT_BUFFER_H
#define MK_TEXT_BUFFER_H

#define WORD_CAP 64

struct MK_Text_buffer
{
	Str8 str;
};

struct MK_Word
{
	char c;//[WORD_CAP];
	i32 len;
	b32 is_tab;
	b32 is_space;
};

struct MK_Word_node
{
	MK_Word_node *next;
	MK_Word w;
};

// TODO(mizu):
// then complete making row_index_nodes a virtual array
// then bring the status bar up to mark, and copy over any other changes you made
// then make a free list so you can take care of deleted nodes
// that will be end of second pass.
// then work on ykw fr
// then work on game
// It is 9 pm. I want to sleep on time for once. gn

struct MK_Word_row
{
	MK_Word_node *first;
	MK_Word_node *last;
	
	i32 num_col;
	i32 num_tab;
	//i32 count;
};

struct MK_Word_row_node
{
	MK_Word_row_node *next;
	
	MK_Word_row row;
};

struct MK_Word_row_list
{
	MK_Word_row_node *first;
	MK_Word_row_node *last;
	
	i32 count;
	
	MK_Word_row_node *row_index_nodes[100000];
};

internal MK_Word_node *mk_word_push(Arena *arena, MK_Word_row *row);
internal MK_Word_row_node *mk_word_row_push(Arena *arena, MK_Word_row_list *list);

internal MK_Word_node *mk_word_insert(Arena *arena, MK_Word_row *row, i32 index);
internal MK_Word_row_node *mk_word_row_insert(Arena *arena, MK_Word_row_list *list, i32 index);

internal void mk_word_remove(MK_Word_row *row, i32 index);
internal void mk_word_row_remove(MK_Word_row_list *list, i32 index);

internal MK_Word_row_list mk_word_list_from_buffer(Arena *arena, u8 *file);

internal MK_Word_row_node *mk_get_word_row(MK_Word_row_list* list, i32 index);
internal void mk_update_row_index_cache(MK_Word_row_list *list, i32 index);
#endif //MK_TEXT_BUFFER_H