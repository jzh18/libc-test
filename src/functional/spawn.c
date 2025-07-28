#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <spawn.h>
#include <sys/wait.h>
#include "test.h"

#define TEST(f, x) (void)( \
	(r = (f)) == (x) || \
	t_error("%s failed, got %d want %d\n", #f, r, x) )

#define TEST_E(f) (void)( \
	(errno = 0), (f) || \
	t_error("%s failed (errno = %d \"%s\")\n", #f, errno, strerror(errno)) )

int main(void)
{
	rtrace_printf_init();
	int r;
	char foo[10];
	int p[2];
	pid_t pid;
	int status;
	posix_spawn_file_actions_t fa;

rtrace_printf_begin("0x11b410"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, p); 
int rc;
	TEST_E(!(rc=pipe(p)));
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x11b410"); 


rtrace_printf_begin("0x10f110"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &fa); 
	TEST((rc=posix_spawn_file_actions_init(&fa)), 0);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x10f110"); 

rtrace_printf_begin("0x10ed40"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &fa); 
rtrace_printf(TYPE_ARG, TYPE_INT, 1, p[0]); 
	TEST((rc=posix_spawn_file_actions_addclose(&fa, p[0])), 0);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x10ed40"); 

rtrace_printf_begin("0x10ee20"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &fa); 
rtrace_printf(TYPE_ARG, TYPE_INT, 1, p[1]); 
rtrace_printf(TYPE_ARG, TYPE_INT, 2, 1); 
	TEST((rc=posix_spawn_file_actions_adddup2(&fa, p[1], 1)), 0);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x10ee20"); 

rtrace_printf_begin("0x10ed40"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &fa); 
rtrace_printf(TYPE_ARG, TYPE_INT, 1, p[1]); 
	TEST((rc=posix_spawn_file_actions_addclose(&fa, p[1])), 0);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x10ed40");

rtrace_printf_begin("0x10fec0"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &pid); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, "echo"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 2, &fa); 
rtrace_printf(TYPE_ARG, TYPE_INT, 3, 0); 
char ** ps_argv4={"echo","hello",0};
rtrace_printf(TYPE_ARG, TYPE_POINTER, 4, ps_argv4); 
rtrace_printf(TYPE_ARG, TYPE_INT, 5, 0); 
	TEST((rc=posix_spawnp(&pid, "echo", &fa, 0, ps_argv4, 0)), 0);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x10fec0");

	close(p[1]);
	TEST(waitpid(pid, &status, 0), pid);
	TEST(read(p[0], foo, sizeof foo), 6);
	close(p[0]);

rtrace_printf_begin("0x10f050"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &fa); 
	TEST((rc=posix_spawn_file_actions_destroy(&fa)), 0);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x10f050");
	return t_status;
}
