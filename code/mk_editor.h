/* date = July 7th 2024 10:42 am */

#ifndef MK_EDITOR_H
#define MK_EDITOR_H

struct MK_Editor
{
	Arena *arena;
	Arena *transient;
	
	MK_Window window;
};

#endif //MK_EDITOR_H