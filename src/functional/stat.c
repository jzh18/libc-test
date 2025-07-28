#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include "test.h"

#define TEST(c, ...) ((c) ? 1 : (t_error(#c" failed: " __VA_ARGS__),0))

int main(void)
{
	rtrace_printf_init();
	struct stat st;
	FILE *f;
	time_t t;

rtrace_printf_begin("0x11bbc0"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, "."); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, &st); 
int rc;
	if (TEST((rc=stat(".",&st))==0, "errno = %s\n", strerror(errno))) {
		TEST(S_ISDIR(st.st_mode), "\n");
		TEST(st.st_nlink>0, "%ju\n", (uintmax_t)st.st_nlink);
		t = time(0);
		TEST(st.st_ctime<=t, "%jd > %jd\n", (intmax_t)st.st_ctime, (intmax_t)t);
		TEST(st.st_mtime<=t, "%jd > %jd\n", (intmax_t)st.st_mtime, (intmax_t)t);
		TEST(st.st_atime<=t, "%jd > %jd\n", (intmax_t)st.st_atime, (intmax_t)t);
	}
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x11bbc0"); 

	if (TEST(stat("/dev/null",&st)==0, "errno = %s\n", strerror(errno))) {
		TEST(S_ISCHR(st.st_mode), "\n");
	}

	if ((f = tmpfile())) {
		fputs("hello", f);
		fflush(f);
		if (TEST(fstat(fileno(f),&st)==0, "errnp = %s\n", strerror(errno))) {
			TEST(st.st_uid==geteuid(), "%d vs %d\n", (int)st.st_uid, (int)geteuid());
			TEST(st.st_gid==getegid(), "%d vs %d\n", (int)st.st_uid, (int)geteuid());
			TEST(st.st_size==5, "%jd vs 5\n", (intmax_t)st.st_size);
		}
		fclose(f);
	}

	return t_status;
}
