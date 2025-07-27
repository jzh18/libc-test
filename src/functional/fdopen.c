#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "test.h"

#define TEST(c)                                             \
	do                                                      \
	{                                                       \
		errno = 0;                                          \
		if (!(c))                                           \
			t_error("%s failed (errno = %d)\n", #c, errno); \
	} while (0)

int main(void)
{
	rtrace_printf_init();
	char tmp[] = "/tmp/testsuite-XXXXXX";
	char foo[6];
	int fd;
	FILE *f;

	int rc = 0;
	rtrace_printf_begin("0x125220");
	rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, tmp);
	TEST((fd = mkstemp(tmp)) > 2);
	rtrace_printf(TYPE_RET, TYPE_INT, 0, fd);
	rtrace_printf_end("0x125220");

	rtrace_printf_begin("0x11c560");
	rtrace_printf(TYPE_ARG, TYPE_INT, 0, fd);
	rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, "hello");
	rtrace_printf(TYPE_ARG, TYPE_INT, 2, 6);
	TEST((rc = write(fd, "hello", 6)) == 6);
	rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
	rtrace_printf_end("0x11c560");

	rtrace_printf_begin("0x854f0");
	rtrace_printf(TYPE_ARG, TYPE_INT, 0, fd);
	rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, "rb");
	TEST(f = fdopen(fd, "rb"));
	rtrace_printf(TYPE_RET, TYPE_INT, 0, f);
	rtrace_printf_end("0x854f0");

	if (f)
	{
		rtrace_printf_begin("0x90220");
		rtrace_printf(TYPE_ARG, TYPE_INT, 0, f);
		TEST((rc = ftello(f)) == 6);
		rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
		rtrace_printf_end("0x90220");

		rtrace_printf_begin("0x900e0");
		rtrace_printf(TYPE_ARG, TYPE_INT, 0, f);
		rtrace_printf(TYPE_ARG, TYPE_INT, 1, 0);
		rtrace_printf(TYPE_ARG, TYPE_INT, 2, SEEK_SET);
		TEST((rc = fseeko(f, 0, SEEK_SET)) == 0);
		rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
		rtrace_printf_end("0x900e0");

		rtrace_printf_begin("0x85b30");
		rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, foo);
		rtrace_printf(TYPE_ARG, TYPE_INT, 1, sizeof foo);
		char *rs;
		TEST((rs = fgets(foo, sizeof foo, f)));
		rtrace_printf(TYPE_RET, TYPE_POINTER, 0, rs);
		rtrace_printf_end("0x85b30");

		if (strcmp(foo, "hello") != 0)
			t_error("fgets read back: \"%s\"; wanted: \"hello\"\n", foo);

		rtrace_printf_begin("0x85290");
		rtrace_printf(TYPE_ARG, TYPE_INT, 0, f);
		rc = fclose(f);
		rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
		rtrace_printf_end("0x85290");
	}
	if (fd > 2)
		TEST(unlink(tmp) != -1);
	return t_status;
}
