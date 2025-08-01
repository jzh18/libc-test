#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include "test.h"

#define TEST(c, ...) ((c) ? 1 : (t_error(#c" failed: " __VA_ARGS__),0))
#define TESTE(c) (errno=0, TEST(c, "errno = %s\n", strerror(errno)))

int main(void)
{
	rtrace_printf_init();
	struct flock fl = {0};
	FILE *f;
	int fd;
	pid_t pid;
	int status;

	if (!TESTE(f=tmpfile())) return t_status;
	fd = fileno(f);

	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	rtrace_printf_begin("0x116fd0");
	rtrace_printf(TYPE_ARG, TYPE_INT, 0, fd);
	rtrace_printf(TYPE_ARG, TYPE_INT, 1, F_SETLK);
	rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &fl);
	int rc=0;
	TESTE((rc=fcntl(fd, F_SETLK, &fl))==0);
	rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
	rtrace_printf_end("0x116fd0");

	pid = fork();
	if (!pid) {
		fl.l_type = F_RDLCK;
		_exit(fcntl(fd, F_SETLK, &fl)==0 ||
			(errno!=EAGAIN && errno!=EACCES));
	}
	while (waitpid(pid, &status, 0)<0 && errno==EINTR);
	TEST(status==0, "lock failed to work\n");

	pid = fork();
	if (!pid) {
		fl.l_type = F_WRLCK;
		_exit(fcntl(fd, F_GETLK, &fl) || fl.l_pid != getppid());
	}
	while (waitpid(pid, &status, 0)<0 && errno==EINTR);
	TEST(status==0, "child failed to detect lock held by parent\n");

	fclose(f);

	return t_status;
}
