/* unnamed semaphore sanity check */
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include "test.h"

#define T(f) if(f) t_error(#f" failed: %s\n", strerror(errno))
#define T2(r,f) if((r=(f))) t_error(#f" failed: %s\n", strerror(r))

static void *start(void *arg)
{
	struct timespec ts;
	sem_t *s = arg;
	T(clock_gettime(CLOCK_REALTIME, &ts));
	ts.tv_sec += 1;
	T(sem_post(s));
	T(sem_timedwait(s+1, &ts));
	return 0;
}

static void many_waiters()
{
	pthread_t t[3];
	sem_t s[2];
	int r;
	void *p;

rtrace_printf_begin("0xa43a0"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, s); 
rtrace_printf(TYPE_ARG, TYPE_INT, 1, 0); 
rtrace_printf(TYPE_ARG, TYPE_INT, 2, 0); 
int rc;
	T((rc=sem_init(s, 0, 0)));
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0xa43a0"); 


	T(sem_init(s+1, 0, 0));
	T2(r,pthread_create(t, 0, start, s));
	T2(r,pthread_create(t+1, 0, start, s));
	T2(r,pthread_create(t+2, 0, start, s));
rtrace_printf_begin("0xa5000"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0,s); 
	T((rc=sem_wait(s)));
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0xa5000"); 
	T(sem_wait(s));
	T(sem_wait(s));

rtrace_printf_begin("0xa4390"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, s); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, &r); 
	T((rc=sem_getvalue(s, &r)));
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0xa4390"); 

	if (r)
		t_error("sem value should be 0, got %d\n", r);
rtrace_printf_begin("0xa47e0"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, s+1); 
	T((rc=sem_post(s+1)));
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0xa47e0"); 

	T(sem_post(s+1));
	T(sem_post(s+1));
	T2(r,pthread_join(t[0],&p));
	T2(r,pthread_join(t[1],&p));
	T2(r,pthread_join(t[2],&p));
	T(sem_getvalue(s+1, &r));
	if (r)
		t_error("sem value should be 0, got %d\n", r);
rtrace_printf_begin("0xa4380"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, s); 
	T((rc=sem_destroy(s)));
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0xa4380"); 
	T(sem_destroy(s+1));
}

static void single_thread()
{
	struct timespec ts;
	sem_t s;
	int r;


	T(sem_init(&s, 0, 1));
	T(sem_wait(&s));
	T(sem_getvalue(&s, &r));
	if (r)
		t_error("sem value should be 0, got %d\n", r);
rtrace_printf_begin("0xa5040"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &s); 
int rc;
	if ((rc=sem_trywait(&s)) != -1 || errno != EAGAIN)
		t_error("sem_trywait should fail with EAGAIN, got %s\n", strerror(errno));
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0xa5040"); 

	errno = 0;
	T(clock_gettime(CLOCK_REALTIME, &ts));
rtrace_printf_begin("0xa4de0"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &s); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, &ts); 
	if ((rc=sem_timedwait(&s, &ts))!=-1 || errno != ETIMEDOUT)
		t_error("sem_timedwait should fail with ETIMEDOUT, got %s\n", strerror(errno));
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0xa4de0"); 
	T(sem_destroy(&s));
}

int main(void)
{
	rtrace_printf_init();
	single_thread();
	many_waiters();
	return t_status;
}
