#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"

int main(void)
{
	rtrace_printf_init();
	double x, want = .1111111111111111111111;
	char buf[40000];

	memset(buf, '1', sizeof buf);
	buf[0] = '.';
	buf[sizeof buf - 1] = 0;


rtrace_printf_begin("0x4db70"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, buf); 
rtrace_printf(TYPE_ARG, TYPE_INT, 1, 0); 
	if ((x=strtod(buf, 0)) != want)
		t_error("strtod(.11[...]1) got %a want %a\n", x, want);
rtrace_printf(TYPE_RET, TYPE_INT, 0, (int)x); 
rtrace_printf_end("0x4db70"); 
	return t_status;
}

