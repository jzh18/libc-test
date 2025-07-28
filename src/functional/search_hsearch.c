#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <errno.h>
#include "test.h"

// do not trace hsearch as it calls underlying hsearc_r
#define set(k,v) do{ \
	e = hsearch((ENTRY){.key = k, .data = (void*)v}, ENTER); \
	if (!e || strcmp(e->key, k) != 0) \
		t_error("hsearch ENTER %s %d failed\n", k, v); \
}while(0)

#define get(k) hsearch((ENTRY){.key = k, .data = 0}, FIND)

#define getdata(e) ((intptr_t)(e)->data)

int main()
{
	rtrace_printf_init();
	ENTRY *e;

rtrace_printf_begin("0x1208f0");
rtrace_printf(TYPE_ARG, TYPE_INT, 0, -1);
int rc;
	if (rc=hcreate(-1) || errno != ENOMEM)
		t_error("hcreate((size_t)-1) should fail with ENOMEM got %s\n", strerror(errno));
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
rtrace_printf_end("0x1208f0");
	if (!hcreate(13))
		t_error("hcreate(13) failed\n");
	set("", 0);
	set("a", 1);
	set("b", 2);
	set("abc", 3);
	set("cd", 4);
	set("e", 5);
	set("ef", 6);
	set("g", 7);
	set("h", 8);
	set("iiiiiiiiii", 9);
	if (!get("a"))
		t_error("hsearch FIND a failed\n");
	if (get("c"))
		t_error("hsearch FIND c should fail\n");
	set("g", 10);
	if (e && getdata(e) != 7)
		t_error("hsearch ENTER g 10 returned data %d, wanted 7\n", getdata(e));
	set("g", 10);
	if (e && getdata(e) != 7)
		t_error("hsearch ENTER g 10 returned data %d, wanted 7\n", getdata(e));
	set("j", 10);
	if (e && getdata(e) != 10)
		t_error("hsearch ENTER j 10 returned data %d, wanted 10\n", getdata(e));
rtrace_printf_begin("0x1208e0");
	hdestroy();
rtrace_printf_end("0x1208e0");
	return t_status;
}
