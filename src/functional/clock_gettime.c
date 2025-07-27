#include <time.h>
#include <errno.h>
#include <string.h>
#include "test.h"

#define TEST(c, ...) \
	( (c) || (t_error(#c " failed: " __VA_ARGS__),0) )

int main()
{
	rtrace_printf_init();
	struct timespec ts;
	rtrace_printf_begin("0xec950");
	rtrace_printf(TYPE_ARG, TYPE_INT, 0, CLOCK_REALTIME);
	rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, &ts);
	int ret=0;
	TEST((ret=clock_gettime(CLOCK_REALTIME, &ts)) == 0 && errno == 0, "%s\n", strerror(errno));
	rtrace_printf(TYPE_RET, TYPE_POINTER, 0, ret);
	rtrace_printf_end("0xec950");
	return t_status;
}
