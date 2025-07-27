#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <stdlib.h>
#include <locale.h>
#include <langinfo.h>
#include <limits.h>
#include "test.h"

int main(void)
{
	rtrace_printf_init();
	int i, j;
	mbstate_t st, st2;
	wchar_t wc, map[257], wtmp[257];
	char s[MB_LEN_MAX*256];
	size_t rv;
	int c;
	int ni_errors=0;

	setlocale(LC_CTYPE, "C");

	if (MB_CUR_MAX != 1) t_error("MB_CUR_MAX = %d, expected 1\n", (int)MB_CUR_MAX);

	// for (i=0; i<256; i++) {
	// 	st = (mbstate_t){0};
	// 	if (mbrtowc(&wc, &(char){i}, 1, &st) != !!i)
	// 		t_error("mbrtowc failed to convert byte %.2x to wchar_t\n", i);
	// 	if ((map[i]=btowc(i)) == WEOF) {
	// 		t_error("btowc failed to convert byte %.2x to wchar_t\n", i);
	// 		continue;
	// 	}
	// 	for (j=0; j<i; j++) {
	// 		if (map[j]==map[i])
	// 			t_error("bytes %.2x and %.2x map to same wchar_t %.4x\n", j, i, (unsigned)map[i]);
	// 	}
	// }

	// for (i=0; i<256; i++) {
	// 	if (map[i]==WEOF) continue;
	// 	if (wctob(map[i]) != i)
	// 		t_error("wctob failed to convert wchar_t %.4x back to byte %.2x\n", (unsigned)map[i], i);
	// }

	// /* covering whole 32-bit range would be too slow... maybe add random high tests? */
	// for (i=0; i<0x110000; i++) {
	// 	if (wcschr(map+1, i)) continue;
	// 	if ((c=wctob(i)) != WEOF && ni_errors++ < 50)
	// 		t_error("wctob accepted non-image wchar_t %.4x as byte %.2x\n", i, c);
	// 	st = (mbstate_t){0};
	// 	if (wcrtomb(s, i, &st) != -1  && ni_errors++ < 50)
	// 		t_error("wcrtomb accepted non-image wchar_t %.4x\n", i);
	// }
	// if (ni_errors > 50)
	// 	t_error("additional %d non-image errors (not printed)\n", ni_errors);

	map[256] = 0;
	st = (mbstate_t){0};
	if ((rv=wcsrtombs(s, &(const wchar_t *){map+1}, sizeof s, &st)) != 255)
		t_error("wcsrtombs returned %zd, expected 255\n", rv);
	if ((rv=mbsrtowcs(wtmp, &(const char *){s}, 256, &st)) != 255)
		t_error("mbsrtowcs returned %zd, expected 255\n", rv);
	if (memcmp(map+1, wtmp, 256*sizeof(*map)))
		t_error("wcsrtombs/mbsrtowcs round trip failed\n");
	int ret_val;
	for (i=128; i<129; i++) {
		rtrace_printf_begin("0x12e970");
		rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, map[i]);
		if (ret_val=iswalnum(map[i])) t_error("iswalnum returned true for %.4x (%.2x)\n", map[i], i);
		rtrace_printf(TYPE_RET, TYPE_POINTER, 0, ret_val);
		rtrace_printf_end("0x12e970");

		// rtrace_printf_begin("0x12e9f0");
		// rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, map[i]);
		// if (ret_val=iswalpha(map[i])) t_error("iswalpha returned true for %.4x (%.2x)\n", map[i], i);
		// rtrace_printf(TYPE_RET, TYPE_POINTER, 0, ret_val);
		// rtrace_printf_end("0x12e9f0");

		// rtrace_printf_begin("0x12e110");
		// rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, map[i]);
		// if (ret_val=iswblank(map[i])) t_error("iswblank returned true for %.4x (%.2x)\n", map[i], i);
		// rtrace_printf(TYPE_RET, TYPE_POINTER, 0, ret_val);
		// rtrace_printf_end("0x12e110");

		// rtrace_printf_begin("0x12e1a0");
		// rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, map[i]);
		// if (ret_val=iswcntrl(map[i])) t_error("iswcntrl returned true for %.4x (%.2x)\n", map[i], i);
		// rtrace_printf(TYPE_RET, TYPE_POINTER, 0, ret_val);
		// rtrace_printf_end("0x12e1a0");

		// rtrace_printf_begin("0x12eb70");
		// rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, map[i]);
		// if (ret_val=iswdigit(map[i])) t_error("iswdigit returned true for %.4x (%.2x)\n", map[i], i);
		// rtrace_printf(TYPE_RET, TYPE_POINTER, 0, ret_val);
		// rtrace_printf_end("0x12eb70");

		// rtrace_printf_begin("0x012e350");
		// rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, map[i]);
		// if (ret_val=iswgraph(map[i])) t_error("iswgraph returned true for %.4x (%.2x)\n", map[i], i);
		// rtrace_printf(TYPE_RET, TYPE_POINTER, 0, ret_val);
		// rtrace_printf_end("0x012e350");

		// rtrace_printf_begin("0x12ebf0");
		// rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, map[i]);
		// if (ret_val=iswlower(map[i])) t_error("iswlower returned true for %.4x (%.2x)\n", map[i], i);
		// rtrace_printf(TYPE_RET, TYPE_POINTER, 0, ret_val);
		// rtrace_printf_end("0x12ebf0 ");

		// rtrace_printf_begin("0x12ecf0");
		// rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, map[i]);
		// if (ret_val=iswprint(map[i])) t_error("iswprint returned true for %.4x (%.2x)\n", map[i], i);
		// rtrace_printf(TYPE_RET, TYPE_POINTER, 0, ret_val);
		// rtrace_printf_end("0x12ecf0");

		// rtrace_printf_begin("0x12ed70 ");
		// rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, map[i]);
		// if (ret_val=iswpunct(map[i])) t_error("iswpunct returned true for %.4x (%.2x)\n", map[i], i);
		// rtrace_printf(TYPE_RET, TYPE_POINTER, 0, ret_val);
		// rtrace_printf_end("0x12ed70 ");

		// rtrace_printf_begin("0x12edf0");
		// rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, map[i]);
		// if (ret_val=iswspace(map[i])) t_error("iswspace returned true for %.4x (%.2x)\n", map[i], i);
		// rtrace_printf(TYPE_RET, TYPE_POINTER, 0, ret_val);
		// rtrace_printf_end("0x12edf0");

		// rtrace_printf_begin("0x12ee70");
		// rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, map[i]);
		// if (ret_val=iswupper(map[i])) t_error("iswupper returned true for %.4x (%.2x)\n", map[i], i);
		// rtrace_printf(TYPE_RET, TYPE_POINTER, 0, ret_val);
		// rtrace_printf_end("0x12ee70");

		// rtrace_printf_begin("0x12e620");
		// rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, map[i]);
		// if (ret_val=iswxdigit(map[i])) t_error("iswxdigit returned true for %.4x (%.2x)\n", map[i], i);
		// rtrace_printf(TYPE_RET, TYPE_POINTER, 0, ret_val);
		// rtrace_printf_end("0x12e620");
	}

	return 0;
}
