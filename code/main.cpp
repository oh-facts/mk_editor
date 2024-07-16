#include "mk_platform.h"
#include <errno.h>
#include <string.h>

#include <dlfcn.h>
#include <sys/mman.h>

global const char *welcome_msg = "⠀⠀⠀⠀⠀⠀\r\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣤⣤⠤⣬⣿⣦⣠⣤⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\r\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣤⠶⠛⠋⠉⠀⠀⠀⠀⠈⠙⠋⠛⠛⠲⢦⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀\r\n\
⠀⠀⠀⠀⠀⠀⠀⢀⣤⡶⠞⠉⠁⢀⣀⣠⠤⠤⠤⣀⣀⣀⣠⠤⠴⠶⢤⡀⠀⠈⠙⢵⡄⠀⠀⠀⠀⠀⠀⠀\r\n\
⠀⠀⠀⠀⢀⣤⠶⠋⠁⢀⣤⠖⠋⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠳⣄⠀⠀⠙⢷⡀⠀⠀⠀⠀⠀\r\n\
⠀⠀⠀⣰⠟⠁⢀⡴⠊⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣦⡀⠀⠈⢻⣆⠀⠀⠀⠀\r\n\
⠀⠀⣾⠃⢀⡴⠋⠀⠀⣠⡆⠀⠀⠀⠀⠀⢀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⠷⣄⠀⠀⠹⣧⠀⠀⠀\r\n\
⠀⣾⠃⢠⠏⠀⠀⢀⡼⠋⠀⠀⠀⠀⠀⠀⡾⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣦⠀⠙⢧⡀⠀⠘⣧⠀⠀\r\n\
⢠⡏⢠⠏⠀⠀⢠⡟⠁⠀⠀⠀⠀⠀⠀⣼⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⢧⡀⠈⢳⡄⠀⠘⣧⠀\r\n\
⣸⡇⡾⠀⣶⣰⢿⠇⠀⢀⡟⠀⠀⠀⢠⣿⠀⠀⠀⠀⠀⠀⢠⡆⠀⢻⡄⠀⠀⠀⠀⠈⣧⠀⢰⣿⡄⠀⢹⡇\r\n\
⣿⢃⡇⠀⣿⡏⣿⠀⠀⣼⠁⠀⡄⠀⢸⣿⠀⠀⠀⠀⠀⠀⠈⡇⠀⠘⡇⠀⠀⠀⠀⠀⠸⣇⠈⣯⣷⠀⠀⣷\r\n\
⣿⢰⡇⠀⣿⠃⣿⠀⠀⣿⠀⠘⡇⠀⢸⣿⠀⠀⠀⢠⠀⠀⢀⡇⠀⢀⡇⠐⣆⠀⢷⡀⠀⢻⡀⣿⢹⡆⠀⢿\r\n\
⣿⠸⡇⠀⢻⡀⢿⡀⢠⣿⠀⢠⣧⠀⠀⢻⡀⠀⠀⣿⠀⠀⢸⣇⡀⣼⢹⡄⣿⣆⢸⣷⡀⢘⡇⣿⢸⡇⠀⣼\r\n\
⢹⡇⡇⠀⠸⣇⠘⣇⢸⡟⣇⣸⠛⣧⠀⣿⣇⠀⢀⡇⠀⠀⣼⣽⣰⠃⠀⢷⣿⢿⡄⣇⣧⣸⣿⣿⣿⡇⠀⣿\r\n\
⠘⣷⡹⡄⠀⢻⡄⠸⣿⡧⢽⣿⠤⠼⣆⢸⣿⡄⢸⣷⠀⢰⡿⣿⣓⣒⣖⣚⣿⠚⢿⣿⣹⣿⣿⢹⣿⠇⢰⡏\r\n\
⠀⠘⣧⡱⣼⠀⠻⠠⣿⢷⡟⡻⣿⣿⡟⢾⡝⢻⣼⣿⣠⠟⠹⠇⢉⣻⣿⡿⢛⠟⠀⢉⡽⢻⡟⣸⡿⢀⣿⠀\r\n\
⠀⠀⠘⢷⣽⡄⠀⠀⠙⣧⣄⠀⠈⠉⠉⠈⠁⠀⠉⠋⠁⠀⠀⠀⠀⠀⠀⠀⠁⠀⣠⣾⠅⢸⣇⡟⣇⣾⠃⠀\r\n\
⠀⠀⠀⠈⠿⣇⠀⢸⡆⠙⢿⣧⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣼⠿⠋⣴⢀⣜⣷⡟⠁⠀⠀\r\n\
⠀⠀⠀⠀⠘⣿⡆⢠⣿⣄⠈⠳⣌⠙⠓⠆⠀⠀⠀⠓⠀⠀⣠⠄⠀⠀⡴⢞⣽⡃⢀⣼⣇⣼⣟⡿⠁⠀⠀⠀\r\n\
⠀⠀⠀⠀⠀⠘⢿⣼⣿⠻⣿⣮⣿⡷⣄⡀⠀⠀⠀⠉⠋⠉⠁⠀⠀⣠⣴⣿⣿⣧⡾⢻⣿⡏⠀⠀⠀⠀⠀⠀\r\n\
⠀⠀⠀⠀⠀⠀⠈⠻⣿⠀⠀⠙⠻⠿⠛⠛⣷⣤⣀⠀⠀⠀⣀⠴⠚⢱⢸⡇⠘⠋⠀⠈⠿⠇⠀⠀⠀⠀⠀⠀\r\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⡤⠾⡏⠙⠓⠒⠋⠁⠀⠀⢸⢸⠷⣦⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\r\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⡾⠟⢱⠀⢀⡿⠀⠀⠀⠀⠀⠀⠀⠘⣟⣆⡤⠙⢷⣄⠀⠀⠀⠀⠀⠀⠀⠀\r\n\
⠀⠀⠀⠀⠀⠀⠀⢀⣠⡶⠟⠉⠀⠀⠘⣖⠋⠀⠀⠀⠀⠀⠀⠀⠀⢀⡼⠋⠠⢄⠀⠙⠻⣢⣀⠀⠀⠀⠀⠀\r\n\
⠀⠀⠀⠀⠀⣤⠾⠛⠁⠀⠀⠀⠀⠀⠀⠈⢧⡀⠀⠳⠤⠖⠂⣠⠞⠁⠀⠀⠀⠀⠑⢄⠀⠈⠙⠷⣣⣀⡀⠀\r\n\
⠀⠀⢀⣤⡾⠛⠙⣲⣤⣀⡀⠀⠀⠀⠀⠀⠀⠳⣆⠀⢀⣤⠞⠁⠀⠀⠀⠀⠀⠀⠀⠀⠑⠤⣀⣀⡬⢵⡯⡄";

global u64 total_cmt;
global u64 total_res;

internal void *os_linux_reserve(u64 size)
{
	void *out = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	total_res += size;
	return out;
}

internal b32 os_linux_commit(void *ptr, u64 size)
{
	if(mprotect(ptr, size, PROT_READ | PROT_WRITE) == -1)
	{
		int err = errno;
		printf("mprotect failed: %s\r\n", strerror(err));
		return 0;
	}
	total_cmt += size;
	return 1;
}

internal void os_linux_decommit(void *ptr, u64 size)
{
	madvise(ptr, size, MADV_DONTNEED);
  mprotect(ptr, size, PROT_NONE);
}

internal void os_linux_release(void *ptr, u64 size)
{
  munmap(ptr, size);
}

int main(int argc, char **argv)
{
	
	MK_Platform pf = {};
	pf.argc = argc;
	pf.argv = argv;
	
	pf.api.os_reserve = os_linux_reserve;
	pf.api.os_commit = os_linux_commit;
	
	mk_global_platform_api_init(&pf.api);
	
	Arena *arena = arena_create();
	
	Str8 app_dir = os_linux_get_app_dir(arena);
	
	pf.app_dir = app_dir;
	Str8 dll_rel_path = str8_lit("libyk.so");
	Str8 dll_path = str8_join(arena, app_dir, dll_rel_path);
	void *handle = dlopen((char *)dll_path.c, RTLD_LAZY);
	
	termios start;
	tcgetattr(STDIN_FILENO, &start);
	enable_raw_mode();
	
	if(!handle)
	{
		printf("dll not found\n\r");
	}
	
	update_and_render_fn update_and_render = (update_and_render_fn)dlsym(handle, "update_and_render");
	
	if(!update_and_render)
	{
		printf("fn not found\n\r");
	}
	
	submit_clear_screen();
	submit_reset_cursor();
	
	printf("%s\r\n\r\n", welcome_msg);
	
	printf("\"Do not enter\" is written on the doorway.\r\n");
	printf("Why can't everyone just go away?\r\n");
	printf("Except you, you can stay\r\n");
	
	printf("\r\n\r\n");
	
	printf("Welcome to the mk editor dev build\r\n");
	
	printf("ctrl + s   : save\r\n");
	printf("ctrl + q   : quit\r\n");
	printf("ctrl + r   : hot reload\r\n");
	printf("arrow keys : move\r\n");
	printf("home       : line start\r\n");
	printf("end        : line end\r\n");
	printf("ctrl home  : file start\r\n");
	printf("ctrl end   : file end\r\n");
	
	printf("\r\n\r\n");
	
	printf("press any key to continue\r\n");
	
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
			pf.reloaded = 1;
		}
		pf.res = total_res;
		pf.cmt = total_cmt;
		update_and_render(&pf, c);
		
	}while(read(STDIN_FILENO, &c, 1) == 1);
	
	submit_clear_screen();
	submit_reset_cursor();
	
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &start);
	
}