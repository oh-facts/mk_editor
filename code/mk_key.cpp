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
				case '3':
				{
					out = MK_KEY_DEL;
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
	
	if(c == 127)
	{
		out = MK_KEY_BACK_SPACE;
	}
	else if(c == 13)
	{
		out = MK_KEY_ENTER;
	}
	
	return out;
}