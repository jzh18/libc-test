#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include "test.h"

#define T(path, want) \
{ \
	char tmp[100]; \
	rtrace_printf_begin("0x11d970"); \
	rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, tmp); \
	char *got = dirname(strcpy(tmp, path)); \
	rtrace_printf(TYPE_RET, TYPE_POINTER, 0, got); \
	rtrace_printf_end("0x11d970"); \
	if (strcmp(want, got) != 0) \
		t_error("dirname(\"%s\") got \"%s\" want \"%s\"\n", path, got, want); \
}

int main()
{
	rtrace_printf_init();
	if (strcmp(dirname(0), ".") != 0)
		t_error("dirname(0) returned \"%s\"; expected \".\"\n", dirname(0));
	T("", ".");
	T("/usr/lib", "/usr");
	T("/usr/", "/");
	T("usr", ".");
	T("usr/", ".");
	T("/", "/");
	T("///", "/");
	T(".", ".");
	T("..", ".");
	return t_status;
}
