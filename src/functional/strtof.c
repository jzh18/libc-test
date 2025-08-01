#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "test.h"

#define length(x) (sizeof(x) / sizeof *(x))

static struct {
	char *s;
	float f;
} t[] = {
	// 2^-149 * 0.5 - eps
	{".7006492321624085354618647916449580656401309709382578858785341419448955413429303e-45", 0},
	// 2^-149 * 0.5 + eps
	{".7006492321624085354618647916449580656401309709382578858785341419448955413429304e-45", 0x1p-149},
	// 2^-149 * 0.5 - eps
	{".2101947696487225606385594374934874196920392912814773657635602425834686624028790e-44", 0x1p-149},
	// 2^-149 * 0.5 + eps
	{".2101947696487225606385594374934874196920392912814773657635602425834686624028791e-44", 0x1p-148},
	// 2^-126 + 2^-150 - eps
	{".1175494420887210724209590083408724842314472120785184615334540294131831453944281e-37", 0x1p-126},
	// 2^-126 + 2^-150 + eps
	{".1175494420887210724209590083408724842314472120785184615334540294131831453944282e-37", 0x1.000002p-126},
	// 2^128 - 2^103 - eps
	{"340282356779733661637539395458142568447.9999999999999999999", 0x1.fffffep127},
	// 2^128 - 2^103
	{"340282356779733661637539395458142568448", INFINITY},
};

int main(void)
{
	rtrace_printf_init();
	int i;
	float x;
	char *p;

	for (i = 0; i < length(t); i++) {
rtrace_printf_begin("0x50e40"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, t[i].s); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, &p); 
		x = strtof(t[i].s, &p);
rtrace_printf(TYPE_RET, TYPE_INT, 0, (int)x); 
rtrace_printf_end("0x50e40"); 
		if (x != t[i].f)
			t_error("strtof(\"%s\") want %a got %a\n", t[i].s, t[i].f, x);
	}
	return t_status;
}
