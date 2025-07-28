#include <pthread.h>
#include <string.h>
#include "test.h"

#define TEST(r, f, m) ( \
	((r) = (f)) == 0 || (t_error("%s failed: %s (" m ")\n", #f, strerror(r)), 0) )


static void *start_signal(void *arg)
{
	void **args = arg;
	pthread_mutex_lock(args[1]);
	pthread_cond_signal(args[0]);
	pthread_mutex_unlock(args[1]);
	return 0;
}

static void *start_wait(void *arg)
{
	void **args = arg;
	pthread_mutex_t *m = args[1];
	pthread_cond_t *c = args[0];
	int *x = args[2];

	pthread_mutex_lock(m);
	while (*x) pthread_cond_wait(c, m);
	pthread_mutex_unlock(m);

	return 0;
}

int main(void)
{
	rtrace_printf_init();
	pthread_t td, td1, td2, td3;
	int r;
	void *res;
	pthread_mutex_t mtx;
	pthread_cond_t cond;
	int foo[1];

	/* Condition variables */
rtrace_printf_begin("0x9f710");
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &mtx);
rtrace_printf(TYPE_ARG, TYPE_INT, 1, 0);
int rc;
	TEST(r, (rc=pthread_mutex_init(&mtx, 0)), "");
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
rtrace_printf_end("0x9f710");

rtrace_printf_begin("0x9af80");
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &cond);
rtrace_printf(TYPE_ARG, TYPE_INT, 1, 0);
	TEST(r, (rc=pthread_cond_init(&cond, 0)), "");
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
rtrace_printf_end("0x9af80");


rtrace_printf_begin("0x9fff0");
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &mtx);
	TEST(r, (rc=pthread_mutex_lock(&mtx)), "");
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
rtrace_printf_end("0x9fff0");

	TEST(r, pthread_create(&td, 0, start_signal, (void *[]){ &cond, &mtx }), "");
rtrace_printf_begin("0x9b5e0");
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &cond);
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, &mtx);
	TEST(r, (rc=pthread_cond_wait(&cond, &mtx)), "");
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
rtrace_printf_end("0x9b5e0");

	TEST(r, pthread_join(td, &res), "");
rtrace_printf_begin("0xa1a70");
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &mtx);
	TEST(r, (rc=pthread_mutex_unlock(&mtx)), "");
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
rtrace_printf_end("0xa1a70");

rtrace_printf_begin("0x9f6c0");
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &mtx);
	TEST(r, (rc=pthread_mutex_destroy(&mtx)), "");
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
rtrace_printf_end("0x9f6c0");

rtrace_printf_begin("0x9aef0");
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &cond);
	TEST(r, (rc=pthread_cond_destroy(&cond)), "");
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
rtrace_printf_end("0x9aef0");

	/* Condition variables with multiple waiters */
	TEST(r, pthread_mutex_init(&mtx, 0), "");
	TEST(r, pthread_cond_init(&cond, 0), "");
	TEST(r, pthread_mutex_lock(&mtx), "");
	foo[0] = 1;
	TEST(r, pthread_create(&td1, 0, start_wait, (void *[]){ &cond, &mtx, foo }), "");
	TEST(r, pthread_create(&td2, 0, start_wait, (void *[]){ &cond, &mtx, foo }), "");
	TEST(r, pthread_create(&td3, 0, start_wait, (void *[]){ &cond, &mtx, foo }), "");
	TEST(r, pthread_mutex_unlock(&mtx), "");
	nanosleep(&(struct timespec){.tv_nsec=1000000}, 0);
	foo[0] = 0;
	TEST(r, pthread_mutex_lock(&mtx), "");
	TEST(r, pthread_cond_signal(&cond), "");
	TEST(r, pthread_mutex_unlock(&mtx), "");
	TEST(r, pthread_mutex_lock(&mtx), "");
	TEST(r, pthread_cond_signal(&cond), "");
	TEST(r, pthread_mutex_unlock(&mtx), "");
	TEST(r, pthread_mutex_lock(&mtx), "");
	TEST(r, pthread_cond_signal(&cond), "");
	TEST(r, pthread_mutex_unlock(&mtx), "");
	TEST(r, pthread_join(td1, 0), "");
	TEST(r, pthread_join(td2, 0), "");
	TEST(r, pthread_join(td3, 0), "");
	TEST(r, pthread_mutex_destroy(&mtx), "");
	TEST(r, pthread_cond_destroy(&cond), "");

	/* Condition variables with broadcast signals */
	TEST(r, pthread_mutex_init(&mtx, 0), "");
	TEST(r, pthread_cond_init(&cond, 0), "");
	TEST(r, pthread_mutex_lock(&mtx), "");
	foo[0] = 1;
	TEST(r, pthread_create(&td1, 0, start_wait, (void *[]){ &cond, &mtx, foo }), "");
	TEST(r, pthread_create(&td2, 0, start_wait, (void *[]){ &cond, &mtx, foo }), "");
	TEST(r, pthread_create(&td3, 0, start_wait, (void *[]){ &cond, &mtx, foo }), "");
	TEST(r, pthread_mutex_unlock(&mtx), "");
	nanosleep(&(struct timespec){.tv_nsec=1000000}, 0);
	TEST(r, pthread_mutex_lock(&mtx), "");
	foo[0] = 0;
	TEST(r, pthread_mutex_unlock(&mtx), "");
	TEST(r, pthread_cond_broadcast(&cond), "");
	TEST(r, pthread_join(td1, 0), "");
	TEST(r, pthread_join(td2, 0), "");
	TEST(r, pthread_join(td3, 0), "");
	TEST(r, pthread_mutex_destroy(&mtx), "");
	TEST(r, pthread_cond_destroy(&cond), "");

	return t_status;
}
