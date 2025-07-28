#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/resource.h>
#include "test.h"

int main(void)
{
	rtrace_printf_init();
	enum {n = 8};
	char *s = malloc(n);
	int i;
	float f;
	char c;

	if (!s)
		return t_error("out of memory");
	t_setrlim(RLIMIT_STACK, 100*1024);

	for (i = 0; i < n; i++) s[i] = '1';
	s[n-3] = ' ';
	s[n-1] = 0;

	/*
	 * stack overflow if scanf copies s on the stack (glibc)
	 * same issue with %d except then storing the conversion
	 * result is undefined behaviour
	 */
rtrace_printf_begin("0x5fee0"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, s); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, "%f %c"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 2, &f); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 2, &c); 
	i = sscanf(s, "%f %c", &f, &c);
rtrace_printf(TYPE_RET, TYPE_INT, 0, i); 
rtrace_printf_end("0x5fee0"); 

	if (i != 2)
		t_error("sscanf returned %d, want 2\n", i);
	if (f != INFINITY)
		t_error("sscanf(longnum, \"%%f\") read %f, want inf\n", f);
	if (c != '1')
		t_error("sscanf(\"1\", %%c) read '%c', want '1'\n", c);
	free(s);
	return t_status;
}
