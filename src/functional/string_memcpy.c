#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "test.h"

static char buf[512];

static void *(*volatile pmemcpy)(void *restrict, const void *restrict, size_t);

static void *aligned(void *p) {
	return (void*)(((uintptr_t)p + 63) & -64);
}

#define N 80
static void test_align(int dalign, int salign, int len)
{
	char *src = aligned(buf);
	char *dst = aligned(buf + 128);
	char *want = aligned(buf + 256);
	char *p;
	int i;

	if (salign + len > N || dalign + len > N)
		abort();
	for (i = 0; i < N; i++) {
		src[i] = '#';
		dst[i] = want[i] = ' ';
	}
	for (i = 0; i < len; i++)
		src[salign+i] = want[dalign+i] = '0'+i;
rtrace_printf_begin("0xb1720"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, dst+dalign); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, src+salign); 
rtrace_printf(TYPE_ARG, TYPE_INT, 2, len); 
	p = memcpy(dst+dalign, src+salign, len);
rtrace_printf(TYPE_RET, TYPE_POINTER, 0, p); 
rtrace_printf_end("0xb1720"); 
	if (p != dst+dalign)
		t_error("memcpy(%p,...) returned %p\n", dst+dalign, p);
	for (i = 0; i < N; i++)
		if (dst[i] != want[i]) {
			t_error("memcpy(align %d, align %d, %d) failed\n", dalign, salign, len);
			t_printf("got : %.*s\n", dalign+len+1, dst);
			t_printf("want: %.*s\n", dalign+len+1, want);
			break;
		}
}

int main(void)
{
	rtrace_printf_init();
	int i,j,k;

	pmemcpy = memcpy;

	for (i = 0; i < 2; i++)
		for (j = 0; j < 2; j++)
			for (k = 0; k < 2; k++)
				test_align(i,j,k);

	return t_status;
}
