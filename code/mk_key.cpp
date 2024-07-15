// TODO(mizu): hash and index into it
MK_KEY mk_key_from_char(char c)
{
	MK_KEY out = MK_KEY_NONE;
	
	if(c == '\x1b')
	{
		char seq[6];
		if(read(STDIN_FILENO, &seq, 2) == 2)
		{
			if(seq[0] == '[')
			{
				if(seq[1] >= '0' && seq[1] <= '9')
				{
					
					if(read(STDIN_FILENO, &seq[2], 1) == 1 && seq[2] == '~')
					{
						switch(seq[1])
						{
							case '1':
							case '7':
							{
								out = MK_KEY_HOME;
							}break;
							case '4':
							case '8':
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
						}
					}
					
					else if (seq[2] == ';') 
					{
						if (read(STDIN_FILENO, &seq[3], 2) && seq[3] == '5') 
						{ 
							
							if(seq[4]  == 'H')
							{
								out = MK_KEY_CTRL_HOME;
							}
							else if(seq[4] == 'F')
							{
								out = MK_KEY_CTRL_END;
							}
							
						}
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
		}
	}
	else if(c == 127)
	{
		out = MK_KEY_BACK_SPACE;
	}
	else if(c == 13)
	{
		out = MK_KEY_ENTER;
	}
	
	
	
	return out;
}