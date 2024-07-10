/* date = July 10th 2024 11:48 am */

#ifndef MK_KEY_H
#define MK_KEY_H

enum MK_KEY
{
	MK_KEY_NONE,
	
	MK_KEY_UP,
	MK_KEY_DOWN,
	MK_KEY_RIGHT,
	MK_KEY_LEFT,
	
	MK_KEY_HOME,
	MK_KEY_END,
	
	MK_KEY_PAGE_UP,
	MK_KEY_PAGE_DOWN,
	
	MK_KEY_DEL,
	MK_KEY_BACK_SPACE,
	MK_KEY_ENTER,
	MK_KEY_COUNT
};

internal MK_KEY mk_key_from_char(char c);

global char *mk_key_to_cstr[MK_KEY_COUNT] = 
{
	"MK_KEY_NONE",
	
	"MK_KEY_UP",
	"MK_KEY_DOWN",
	"MK_KEY_RIGHT",
	"MK_KEY_LEFT",
	
	"MK_KEY_HOME",
	"MK_KEY_END",
	
	"MK_KEY_PAGE_UP",
	"MK_KEY_PAGE_DOWN",
	
	"MK_KEY_DEL",
	"MK_KEY_BACK_SPACE",
	"MK_KEY_ENTER",
};

#endif //MK_KEY_H
