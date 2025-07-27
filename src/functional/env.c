#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "test.h"

extern char **environ;

int main()
{
	rtrace_printf_init();
	char *s;
	int r;
	int rec=0;
	if (!environ)
		t_error("environ is NULL\n");
	rtrace_printf_begin("0x4aeb0");
	if (rec=clearenv() || (environ && *environ))
		t_error("clrearenv: %s\n", strerror(errno));
	rtrace_printf(TYPE_RET, TYPE_INT, 0, rec);
	rtrace_printf_end("0x4aeb0");

	rtrace_printf_begin("0x49310");
	if (rec=putenv("TEST=1"))
		t_error("putenv: %s\n", strerror(errno));
	rtrace_printf(TYPE_RET, TYPE_INT, 0, rec);
	rtrace_printf_begin("0x49310");

	if (strcmp(environ[0],"TEST=1") != 0)
		t_error("putenv failed: environ[0]: %s, wanted \"TEST=1\"\n", environ[0]);
	if ((s=environ[1]))
		t_error("environ[1]: %p, wanted 0\n", s);
	
	rtrace_printf_begin("0x487b0");
	if (!(s=getenv("TEST")))
		t_error("getenv(\"TEST\"): 0, wanted \"1\"\n");
	rtrace_printf(TYPE_RET, TYPE_POINTER, 0, s);
	rtrace_printf_begin("0x487b0");

	if (strcmp(s,"1") != 0)
		t_error("getenv(\"TEST\"): \"%s\", wanted \"1\"\n", s);

	rtrace_printf_begin("0x04ada0");
	if (rec=unsetenv("TEST"))
		t_error("unsetenv: %s\n", strerror(errno));
	rtrace_printf(TYPE_RET, TYPE_INT, 0, rec);
	rtrace_printf_begin("0x04ada0");
	if ((s=*environ))
		t_error("*environ: %p != 0\n", s);
	if ((s=getenv("TEST")))
		t_error("getenv(\"TEST\"): %p, wanted 0\n", s);
	errno = 0;
	if (setenv("TEST", "2", 0))
		t_error("setenv: %s\n", strerror(errno));
	if (strcmp(s=getenv("TEST"),"2") != 0)
		t_error("getenv(\"TEST\"): \"%s\", wanted \"2\"\n", s);
	if (strcmp(environ[0], "TEST=2") != 0)
		t_error("setenv failed: environ[0]: %s, wanted \"TEST=2\"\n", environ[0]);
	errno = 0;
	if (setenv("TEST", "3", 0))
		t_error("setenv: %s\n", strerror(errno));
	if (strcmp(s=getenv("TEST"),"2") != 0)
		t_error("getenv(\"TEST\"): \"%s\", wanted \"2\"\n", s);
	errno = 0;
	if (setenv("TEST", "3", 1))
		t_error("setenv: %s\n", strerror(errno));
	if (strcmp(s=getenv("TEST"),"3") != 0)
		t_error("getenv(\"TEST\"): \"%s\", wanted \"3\"\n", s);
	/* test failures */
	errno = 0;
	if ((r=setenv("","",0)) != -1 || errno != EINVAL)
		t_error("setenv(\"\",\"\",0): %d, errno: %d (%s), wanted -1, %d (EINVAL)\n", r, errno, strerror(errno), EINVAL);
	return t_status;
}
