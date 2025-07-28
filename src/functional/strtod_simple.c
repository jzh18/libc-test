#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "test.h"

/* r = place to store result
 * f = function call to test (or any expression)
 * x = expected result
 * m = message to print on failure (with formats for r & x)
 */

#define TEST(r, f, x, m) ( \
	((r) = (f)) == (x) || \
	(t_error("%s failed (" m ")\n", #f, r, x, r-x), 0) )

int main(void)
{
	rtrace_printf_init();
	int i;
	double d, d2;
	char buf[1000];

	for (i=0; i<100; i++) {
		d = sin(i);
		snprintf(buf, sizeof buf, "%.300f", d);
rtrace_printf_begin("0x4db70"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, buf); 
rtrace_printf(TYPE_ARG, TYPE_INT, 1, 0); 
double rec;
		TEST(d2, (rec=strtod(buf, 0)), d, "round trip fail %a != %a (%a)");
rtrace_printf(TYPE_RET, TYPE_INT, 0, (int)rec); 
rtrace_printf_end("0x4db70"); 
	}

	TEST(d, strtod("0x1p4", 0), 16.0, "hex float %a != %a");
	TEST(d, strtod("0x1.1p4", 0), 17.0, "hex float %a != %a");
	return t_status;
}

