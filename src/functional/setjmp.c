#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include "test.h"

#define TEST(c, ...) ((c) ? 1 : (t_error(#c" failed: " __VA_ARGS__),0))

int main(void)
{
	rtrace_printf_init();
	volatile int x = 0, r;
	jmp_buf jb;
	sigjmp_buf sjb;
	volatile sigset_t oldset;
	sigset_t set, set2;


	if (!setjmp(jb)) {
		x = 1;
		longjmp(jb, 1);
	}
	TEST(x==1, "setjmp/longjmp seems to have been bypassed\n");

	x = 0;
rtrace_printf_begin("0x44ed0"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, jb); 
	r = setjmp(jb);
rtrace_printf(TYPE_RET, TYPE_INT, 0, r); 
rtrace_printf_end("0x44ed0"); 
	if (!x) {
		x = 1;
rtrace_printf_begin("0x44fb0"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, jb); 
rtrace_printf(TYPE_ARG, TYPE_INT, 1, 0); 
		longjmp(jb, 0);
rtrace_printf_end("0x44fb0"); 
	}
	TEST(r==1, "longjmp(jb, 0) caused setjmp to return %d\n", r);

rtrace_printf_begin("0x45c20"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &set); 
	int rc=sigemptyset(&set);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x45c20"); 

rtrace_printf_begin("0x45c80"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &set); 
rtrace_printf(TYPE_ARG, TYPE_INT, 1, SIGUSR1); 
	rc=sigaddset(&set, SIGUSR1);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x45c80"); 


rtrace_printf_begin("0x45500"); 
rtrace_printf(TYPE_ARG, TYPE_INT, 0, SIG_UNBLOCK); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, &set); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 2, &set2); 
	rc=sigprocmask(SIG_UNBLOCK, &set, &set2);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x45500"); 
	oldset = set2;

	/* Improve the chances of catching failure of sigsetjmp to
	 * properly save the signal mask in the sigjmb_buf. */
	memset(&sjb, -1, sizeof sjb);

	if (!sigsetjmp(sjb, 1)) {
		sigemptyset(&set);
		sigaddset(&set, SIGUSR1);
		sigprocmask(SIG_BLOCK, &set, 0);
		siglongjmp(sjb, 1);
	}
	set = oldset;
	sigprocmask(SIG_SETMASK, &set, &set2);

rtrace_printf_begin("0x45d20"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &set2); 
rtrace_printf(TYPE_ARG, TYPE_INT, 0, SIGUSR1); 
	TEST((rc=sigismember(&set2, SIGUSR1))==0, "siglongjmp failed to restore mask\n");
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x45d20"); 

	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigprocmask(SIG_UNBLOCK, &set, &set2);
	oldset = set2;

	if (!sigsetjmp(sjb, 0)) {
		sigemptyset(&set);
		sigaddset(&set, SIGUSR1);
		sigprocmask(SIG_BLOCK, &set, 0);
		siglongjmp(sjb, 1);
	}
	set = oldset;
	sigprocmask(SIG_SETMASK, &set, &set2);
	TEST(sigismember(&set2, SIGUSR1)==1, "siglongjmp incorrectly restored mask\n");

	return t_status;
}
