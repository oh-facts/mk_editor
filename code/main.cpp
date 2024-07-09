#include "mk_platform.h"

#include <dlfcn.h>

int main(int argc, char **argv)
{
	submit_clear_screen();
	submit_reset_cursor();
	
	Arena arena;
	arena_innit(&arena, Megabytes(1), calloc(1, Megabytes(1)));
	
	Str8 app_dir = os_linux_get_app_dir(&arena);
	Str8 dll_rel_path = str8_lit("libyk.so");
	Str8 dll_path = str8_join(&arena, app_dir, dll_rel_path);
	void *handle = dlopen((char *)dll_path.c, RTLD_LAZY);
	
	termios start;
	tcgetattr(STDIN_FILENO, &start);
	
	MK_Platform pf;
	pf.mem_size = Megabytes(20);
	pf.memory = calloc(1, pf.mem_size);
	pf.argc = argc;
	pf.argv = argv;
	pf.app_dir = app_dir;
	
	if(!handle)
	{
		printf("dll not found\n\r");
	}
	
	update_and_render_fn update_and_render = (update_and_render_fn)dlsym(handle, "update_and_render");
	
	if(!update_and_render)
	{
		printf("fn not found\n\r");
	}
	
	printf("Do not enter is written on the doorway.\n");
	printf("Why can't everyone just go away.\n");
	
	printf("\n\n");
	
	printf("Welcome to the mk editor dev build\n");
	
	printf("ctrl + q to quit\n");
	printf("ctrl + r to hot reload\n");
	printf("ctrl + i to toggle between insert and normal\n");
	
	printf("\n\n");
	
	printf("press any key to continue\n");
	
	enable_raw_mode();
	char c = '\0';
	read(STDIN_FILENO, &c, 1);
	
	submit_clear_screen();
	submit_reset_cursor();
	
	do
	{
		if(c == CTRL_KEY('q'))
		{
			break;
		}
		if(c == CTRL_KEY('R'))
		{
			dlclose(handle);
			handle = dlopen((char*)dll_path.c, RTLD_LAZY);
			update_and_render = (update_and_render_fn)dlsym(handle, "update_and_render");
		}
		update_and_render(&pf, c);
	}while(read(STDIN_FILENO, &c, 1) == 1);
	
	submit_clear_screen();
	submit_reset_cursor();
	
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &start);
	
}