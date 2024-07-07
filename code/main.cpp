#include "base_core.h"
#include "base_core.cpp"
#include "stdio.h"
#include "stdlib.h"
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
#include "base_string.h"
#include "base_string.cpp"

#include "base_math.h"
#include "base_math.cpp"

#include <termios.h>

#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>

#include "mk_editor.h"

#include <dlfcn.h>
int main()
{
	termios start;
	tcgetattr(STDIN_FILENO, &start);
	
	enable_raw_mode();
	submit_clear_screen();
	
	void *handle = dlopen("./out/libyk.so", RTLD_LAZY);
	if(!handle)
	{
		printf("dll not found\n\r");
	}
	update_and_render_fn update_and_render = (update_and_render_fn)dlsym(handle, "update_and_render");
	
	if(!update_and_render)
	{
		printf("fn not found\n\r");
	}
	
	printf("%p\n\r", update_and_render);
	MK_Platform pf;
	pf.mem_size = Megabytes(10);
	pf.memory = calloc(1, pf.mem_size);
	
	char c;
	while(read(STDIN_FILENO, &c, 1) == 1)
	{
		if(c == CTRL_KEY('q'))
		{
			break;
		}
		if(c == CTRL_KEY('R'))
		{
			dlclose(handle);
			handle = dlopen("./out/libyk.so", RTLD_LAZY);
			update_and_render = (update_and_render_fn)dlsym(handle, "update_and_render");
		}
		update_and_render(&pf, c);
	}
	
	submit_clear_screen();
	submit_reset_cursor();
	
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &start);
	
}