/* named semaphore sanity check */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "test.h"

#define TEST(c, ...) \
	( (c) || (t_error(#c " failed: " __VA_ARGS__),0) )

int main(void)
{
	rtrace_printf_init();
	char buf[100];
	struct timespec ts;
	sem_t *sem, *sem2;
	int val;

	clock_gettime(CLOCK_REALTIME, &ts);
	snprintf(buf, sizeof buf, "/testsuite-%d-%d", (int)getpid(), (int)ts.tv_nsec);

rtrace_printf_begin("0xa43e0"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, buf); 
rtrace_printf(TYPE_ARG, TYPE_INT, 1, O_CREAT|O_EXCL); 
rtrace_printf(TYPE_ARG, TYPE_INT, 2, 0700); 
rtrace_printf(TYPE_ARG, TYPE_INT, 3, 1); 
	TEST((sem=sem_open(buf, O_CREAT|O_EXCL, 0700, 1)) != SEM_FAILED,
		"could not open sem: %s\n", strerror(errno));
rtrace_printf(TYPE_RET, TYPE_POINTER, 0, sem); 
rtrace_printf_end("0xa43e0"); 
	errno = 0;
	TEST(sem_open(buf, O_CREAT|O_EXCL, 0700, 1) == SEM_FAILED,
		"reopening should fail with O_EXCL\n");
	TEST(errno == EEXIST,
		"after reopen failure errno is \"%s\" (%d); want EEXIST (%d)\n", strerror(errno), errno, EEXIST);

	TEST(sem_getvalue(sem, &val) == 0, "failed to get sem value\n");
	TEST(val == 1, "wrong initial semaphore value: %d\n", val);

	TEST((sem2=sem_open(buf, 0)) == sem,
		"could not reopen sem: got %p, want %p\n", sem2, sem);

	errno = 0;
	TEST(sem_wait(sem) == 0, "%s\n", strerror(errno));
	TEST(sem_getvalue(sem2, &val) == 0, "%s\n", strerror(errno));
	TEST(val == 0, "wrong semaphore value on second handle: %d\n", val);

	errno = 0;
	TEST(sem_trywait(sem) == -1 && errno == EAGAIN,
		"trywait on locked sem: got errno \"%s\" (%d), want EAGAIN (%d)\n", strerror(errno), errno, EAGAIN);

	TEST(sem_post(sem) == 0, "%s\n", strerror(errno));
	TEST(sem_getvalue(sem2, &val) == 0, "%s\n", strerror(errno));
	TEST(val == 1, "wrong semaphore value on second handle: %d\n", val);

	TEST(sem_close(sem) == 0, "%s\n", strerror(errno));
	TEST(sem_close(sem) == 0, "%s\n", strerror(errno));
	TEST(sem_unlink(buf) == 0, "%s\n", strerror(errno));
	return t_status;
}
