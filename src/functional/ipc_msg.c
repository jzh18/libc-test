#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include "test.h"

static const char path[] = ".";
static const int id = 'm';

#define T(f) do{ \
	if ((f)+1 == 0) \
		t_error("%s failed: %s\n", #f, strerror(errno)); \
}while(0)

#define EQ(a,b,fmt) do{ \
	if ((a) != (b)) \
		t_error("%s == %s failed: " fmt "\n", #a, #b, a, b); \
}while(0)

static void snd()
{
	time_t t;
	key_t k;
	int qid;
	struct msqid_ds qid_ds;
	struct {
		long type;
		char data[20];
	} msg = {1, "test message"};

	T(t = time(0));
	T(k = ftok(path, id));

	/* make sure we get a clean message queue id */
rtrace_printf_begin("0x12c570");
rtrace_printf(TYPE_ARG, TYPE_INT, 0, k);
rtrace_printf(TYPE_ARG, TYPE_INT, 1, IPC_CREAT|0666);
	T(qid = msgget(k, IPC_CREAT|0666));
rtrace_printf(TYPE_RET, TYPE_INT, 0, qid);
rtrace_printf_end("0x12c570");

rtrace_printf_begin("0x12c5a0");
rtrace_printf(TYPE_ARG, TYPE_INT, 0, qid);
rtrace_printf(TYPE_ARG, TYPE_INT, 1, IPC_RMID);
rtrace_printf(TYPE_ARG, TYPE_INT, 2, 0);
int rc;
	T((rc=msgctl(qid, IPC_RMID, 0)));
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
rtrace_printf_end("0x12c5a0");
	T(qid = msgget(k, IPC_CREAT|IPC_EXCL|0666));

	if (t_status)
		exit(t_status);

	/* check IPC_EXCL */
	errno = 0;
	if (msgget(k, IPC_CREAT|IPC_EXCL|0666) != -1 || errno != EEXIST)
		t_error("msgget(IPC_CREAT|IPC_EXCL) should have failed with EEXIST, got %s\n", strerror(errno));

	/* check if msgget initilaized the msqid_ds structure correctly */
	T(msgctl(qid, IPC_STAT, &qid_ds));
	EQ(qid_ds.msg_perm.cuid, geteuid(), "got %d, want %d");
	EQ(qid_ds.msg_perm.uid, geteuid(), "got %d, want %d");
	EQ(qid_ds.msg_perm.cgid, getegid(), "got %d, want %d");
	EQ(qid_ds.msg_perm.gid, getegid(), "got %d, want %d");
	EQ(qid_ds.msg_perm.mode & 0x1ff, 0666, "got %o, want %o");
	EQ(qid_ds.msg_qnum, 0, "got %d, want %d");
	EQ(qid_ds.msg_lspid, 0, "got %d, want %d");
	EQ(qid_ds.msg_lrpid, 0, "got %d, want %d");
	EQ((long long)qid_ds.msg_stime, 0, "got %lld, want %d");
	EQ((long long)qid_ds.msg_rtime, 0, "got %lld, want %d");
	if (qid_ds.msg_ctime < t)
		t_error("qid_ds.msg_ctime >= t failed: got %lld, want >= %lld\n", (long long)qid_ds.msg_ctime, (long long)t);
	if (qid_ds.msg_ctime > t+5)
		t_error("qid_ds.msg_ctime <= t+5 failed: got %lld, want <= %lld\n", (long long)qid_ds.msg_ctime, (long long)t+5);
	if (qid_ds.msg_qbytes <= 0)
		t_error("qid_ds.msg_qbytes > 0 failed: got %d, want > 0\n", qid_ds.msg_qbytes, t);

	/* test send */
rtrace_printf_begin("0x12c420");
rtrace_printf(TYPE_ARG, TYPE_INT, 0, qid);
rtrace_printf(TYPE_ARG, TYPE_INT, 1, sizeof msg.data);
rtrace_printf(TYPE_ARG, TYPE_INT, 2, IPC_NOWAIT);
	T((rc=msgsnd(qid, &msg, sizeof msg.data, IPC_NOWAIT)));
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
rtrace_printf_end("0x12c420");
	T(msgctl(qid, IPC_STAT, &qid_ds));
	EQ(qid_ds.msg_qnum, 1, "got %d, want %d");
	EQ(qid_ds.msg_lspid, getpid(), "got %d, want %d");
	if (qid_ds.msg_stime < t)
		t_error("msg_stime is %lld want >= %lld\n", (long long)qid_ds.msg_stime, (long long)t);
	if (qid_ds.msg_stime > t+5)
		t_error("msg_stime is %lld want <= %lld\n", (long long)qid_ds.msg_stime, (long long)t+5);
}

static void rcv()
{
	key_t k;
	int qid;
	struct {
		long type;
		char data[20];
	} msg;
	long msgtyp = 0;

	T(k = ftok(path, id));
	T(qid = msgget(k, 0));

	errno = 0;
	if (msgrcv(qid, &msg, 0, msgtyp, 0) != -1 || errno != E2BIG)
		t_error("msgrcv should have failed when msgsize==0 with E2BIG, got %s\n", strerror(errno));

	/* test receive */
rtrace_printf_begin("0x12c4c0");
rtrace_printf(TYPE_ARG, TYPE_INT, 0, qid);
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, &msg);
rtrace_printf(TYPE_ARG, TYPE_INT, 2, sizeof msg.data);
rtrace_printf(TYPE_ARG, TYPE_INT, 3, IPC_NOWAIT);
int rc;
	T((rc=msgrcv(qid, &msg, sizeof msg.data, msgtyp, IPC_NOWAIT)));
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc);
rtrace_printf_end("0x12c4c0");
	if (strcmp(msg.data,"test message") != 0)
		t_error("received \"%s\" instead of \"%s\"\n", msg.data, "test message");

	errno = 0;
	if (msgrcv(qid, &msg, sizeof msg.data, msgtyp, MSG_NOERROR|IPC_NOWAIT) != -1 || errno != ENOMSG)
		t_error("msgrcv should have failed when ther is no msg with ENOMSG, got %s\n", strerror(errno));

	/* cleanup */
	T(msgctl(qid, IPC_RMID, 0));
}

int main(void)
{
	rtrace_printf_init();
	int p;
	int status;

	snd();
	p = fork();
	if (p == -1)
		t_error("fork failed: %s\n", strerror(errno));
	else if (p == 0)
		rcv();
	else {
		T(waitpid(p, &status, 0));
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
			t_error("child exit status: %d\n", status);
	}
	return t_status;
}
