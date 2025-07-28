#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "test.h"

#define TEST(r, f, x, m) ( \
	((r) = (f)) == (x) || (t_error("%s failed (" m ")\n", #f, r, x), 0) )

#define TEST_E(f) ( \
	(errno = 0), \
	(f) || (t_error("%s failed (errno = %d)\n", #f, errno), 0) )

#define TEST_S(s, x, m) ( \
	!strcmp((s),(x)) || \
		(t_error("[%s] != [%s] (%s)\n", s, x, m), 0) )

static sig_atomic_t got_sig;

static void handler(int sig) {
	got_sig = 1;
}

int main(void)
{
	rtrace_printf_init();
	int i;
	char foo[6];
	char cmd[64];
	FILE *f;

rtrace_printf_begin("0x878b0");
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, "echo hello");
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, "r");
	TEST_E(f = popen("echo hello", "r"));
rtrace_printf(TYPE_RET, TYPE_INT, 0, f);
rtrace_printf_end("0x878b0");

	if (f) {
		TEST_E(fgets(foo, sizeof foo, f));
		TEST_S(foo, "hello", "child process did not say hello");
rtrace_printf_begin("0x8f460");
rtrace_printf(TYPE_ARG, TYPE_INT, 0, 0);
int rc;
		TEST(i, (rc=pclose(f)), 0, "exit status %04x != %04x");
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
rtrace_printf_end("0x8f460");
	}

	signal(SIGUSR1, handler);
	snprintf(cmd, sizeof cmd, "read a ; test \"x$a\" = xhello && kill -USR1 %d", getpid());
	TEST_E(f = popen(cmd, "w"));
	if (f) {
		TEST_E(fputs("hello", f) >= 0);
		TEST(i, pclose(f), 0, "exit status %04x != %04x");
		TEST(i, got_sig, 1, "child process did not send signal");
	}
	signal(SIGUSR1, SIG_DFL);
	return t_status;
}
