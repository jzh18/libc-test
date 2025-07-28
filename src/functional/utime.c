#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include "test.h"

#define TEST(c, ...) ((c) ? 1 : (t_error(#c" failed: " __VA_ARGS__),0))
#define TESTVAL(v,op,x) TEST(v op x, "%jd\n", (intmax_t)(v))

int main(void)
{
	rtrace_printf_init();
	struct stat st;
	FILE *f;
	int fd;
	time_t t;
rtrace_printf_begin("0x11c510"); 
rtrace_printf(TYPE_ARG, TYPE_INT, 0, AT_FDCWD); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, "/dev/null/invalid"); 
struct timespec utime_argv3[]={{.tv_nsec=UTIME_OMIT},{.tv_nsec=UTIME_OMIT}};
rtrace_printf(TYPE_ARG, TYPE_POINTER, 2, utime_argv3); 
rtrace_printf(TYPE_ARG, TYPE_INT, 3, 0); 
int rc;
	TEST((rc=utimensat(AT_FDCWD, "/dev/null/invalid", utime_argv3, 0))==0 || errno==ENOTDIR,
		"%s\n", strerror(errno));
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x11c510"); 
	TEST(futimens(-1, ((struct timespec[2]){{.tv_nsec=UTIME_OMIT},{.tv_nsec=UTIME_OMIT}}))==0 || errno==EBADF,
		"%s\n", strerror(errno));

	if (!TEST(f = tmpfile())) return t_status;
	fd = fileno(f);
rtrace_printf_begin("0x11a3e0"); 
rtrace_printf(TYPE_ARG, TYPE_INT, 0, fd); 
struct timespec ftime_argv1[]={0};
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, ftime_argv1); 
	TEST(((rc=futimens(fd, ftime_argv1)) == 0, "\n"));
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x11a3e0"); 

	TEST(fstat(fd, &st) == 0, "\n");
	TESTVAL(st.st_atim.tv_sec,==,0);
	TESTVAL(st.st_atim.tv_nsec,==,0);
	TESTVAL(st.st_mtim.tv_sec,==,0);
	TESTVAL(st.st_mtim.tv_nsec,==,0);

	TEST(futimens(fd, ((struct timespec[2]){{.tv_sec=1,.tv_nsec=UTIME_OMIT},{.tv_sec=1,.tv_nsec=UTIME_OMIT}})) == 0, "\n");
	TEST(fstat(fd, &st) == 0, "\n");
	TESTVAL(st.st_atim.tv_sec,==,0);
	TESTVAL(st.st_atim.tv_nsec,==,0);
	TESTVAL(st.st_mtim.tv_sec,==,0);
	TESTVAL(st.st_mtim.tv_nsec,==,0);

	t = time(0);

	TEST(futimens(fd, ((struct timespec[2]){{.tv_nsec=UTIME_NOW},{.tv_nsec=UTIME_OMIT}})) == 0, "\n");
	TEST(fstat(fd, &st) == 0, "\n");
	TESTVAL(st.st_atim.tv_sec,>=,t);
	TESTVAL(st.st_mtim.tv_sec,==,0);
	TESTVAL(st.st_mtim.tv_nsec,==,0);
	
	TEST(futimens(fd, (struct timespec[2]){0}) == 0, "\n");
	TEST(futimens(fd, ((struct timespec[2]){{.tv_nsec=UTIME_OMIT},{.tv_nsec=UTIME_NOW}})) == 0, "\n");
	TEST(fstat(fd, &st) == 0, "\n");
	TESTVAL(st.st_atim.tv_sec,==,0);
	TESTVAL(st.st_mtim.tv_sec,>=,t);

	TEST(futimens(fd, ((struct timespec[2]){{.tv_nsec=UTIME_NOW},{.tv_nsec=UTIME_OMIT}})) == 0, "\n");
	TEST(fstat(fd, &st) == 0, "\n");
	TESTVAL(st.st_atim.tv_sec,>=,t);
	TESTVAL(st.st_mtim.tv_sec,>=,t);

	if (TEST((time_t)(1LL<<32) == (1LL<<32), "implementation has Y2038 EOL\n")) {
		if (TEST(futimens(fd, ((struct timespec[2]){{.tv_sec=1LL<<32},{.tv_sec=1LL<<32}})) == 0, "%s\n", strerror(errno))) {
			TEST(fstat(fd, &st) == 0, "\n");
			TESTVAL(st.st_atim.tv_sec, ==, 1LL<<32);
			TESTVAL(st.st_mtim.tv_sec, ==, 1LL<<32);
		}
	}

	fclose(f);

	return t_status;
}
