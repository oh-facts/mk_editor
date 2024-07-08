/* date = July 7th 2024 10:42 am */

#ifndef MK_EDITOR_H
#define MK_EDITOR_H

#define MK_VERSION_MAJOR (0)
#define MK_VERSION_MINOR (0)
#define MK_VERSION_PATCH (1)

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
	
	MK_KEY_COUNT
};

MK_KEY mk_key_from_char(char c)
{
	MK_KEY out = {};
	if(c == '\x1b')
	{
		char seq[3];
		read(STDIN_FILENO, &seq, 3);
		
		if(seq[2] == '~')
		{
			switch(seq[1])
			{
				case '1':
				{
					out = MK_KEY_HOME;
				}break;
				case '4':
				{
					out = MK_KEY_END;
				}break;
				case '5':
				{
					out = MK_KEY_PAGE_UP;
				}break;
				case '6':
				{
					out = MK_KEY_PAGE_DOWN;
				}break;
				case '7':
				{
					out = MK_KEY_HOME;
				}break;
				case '8':
				{
					out = MK_KEY_END;
				}break;
			}
		}
		else
		{
			switch(seq[1])
			{
				case 'A':
				{
					out = MK_KEY_UP;
				}break;
				case 'B':
				{
					out = MK_KEY_DOWN;
				}break;
				case 'C':
				{
					out = MK_KEY_RIGHT;
				}break;
				case 'D':
				{
					out = MK_KEY_LEFT;
				}break;
				case '~':
				{
					out = MK_KEY_DEL;
				}break;
				case 'H':
				{
					out = MK_KEY_HOME;
				}break;
				case 'F':
				{
					out = MK_KEY_END;
				}break;
			}
		}
	}
	
	return out;
}

internal v2i get_win_size()
{
	winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	
	return (v2i){{ws.ws_col, ws.ws_row}};
}



#endif //MK_EDITOR_H