#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <fcntl.h>
#include "test.h"

#define TEST(c, ...) ((c) ? 1 : (t_error(#c" failed: " __VA_ARGS__),0))
#define TESTE(c) (errno=0, TEST(c, "errno = %s\n", strerror(errno)))

int main(void)
{
	rtrace_printf_init();
	struct sockaddr_in sa = { .sin_family = AF_INET };
	int s, c, t;
	char buf[100];

rtrace_printf_begin("0x12c2e0"); 
rtrace_printf(TYPE_ARG, TYPE_INT, 0, PF_INET); 
rtrace_printf(TYPE_ARG, TYPE_INT, 1, SOCK_DGRAM); 
rtrace_printf(TYPE_ARG, TYPE_INT, 2, IPPROTO_UDP); 
	TESTE((s=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))>=0);
rtrace_printf(TYPE_RET, TYPE_INT, 0, s); 
rtrace_printf_end("0x12c2e0"); 

rtrace_printf_begin("0x12b930"); 
rtrace_printf(TYPE_ARG, TYPE_INT, 0, s); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, &sa); 
rtrace_printf(TYPE_ARG, TYPE_INT, 2, sizeof sa); 
int rc;
	TESTE((rc=bind(s, (void *)&sa, sizeof sa))==0);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x12b930"); 

socklen_t* arg2={sizeof sa};
rtrace_printf_begin("0x12ba30"); 
rtrace_printf(TYPE_ARG, TYPE_INT, 0, s); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, &sa); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 2, arg2); 
	TESTE((rc=getsockname(s, (void *)&sa, arg2))==0);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x12ba30"); 


struct timeval arg0=(struct timeval){.tv_usec=1};
rtrace_printf_begin("0x12c150"); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, &arg0); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, sizeof(struct timeval)); 
	TESTE((rc=setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,
		&arg0, sizeof(struct timeval)))==0);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x12c150"); 


	TESTE((c=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))>=0);
rtrace_printf_begin("0x139560"); 
rtrace_printf(TYPE_ARG, TYPE_INT, 0, 0x7f000001); 
	sa.sin_addr.s_addr = htonl(0x7f000001);
rtrace_printf(TYPE_RET, TYPE_INT, 0, sa.sin_addr.s_addr); 
rtrace_printf_end("0x139560"); 

rtrace_printf_begin("0x12c090"); 
rtrace_printf(TYPE_ARG, TYPE_INT, 0, c); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, "x");
rtrace_printf(TYPE_ARG, TYPE_INT, 2, 1);
rtrace_printf(TYPE_ARG, TYPE_INT, 3, 0);
rtrace_printf(TYPE_ARG, TYPE_POINTER, 4, &sa); 
rtrace_printf(TYPE_ARG, TYPE_INT, 5, sizeof sa);
	TESTE((rc=sendto(c, "x", 1, 0, (void *)&sa, sizeof sa))==1);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x12c090"); 

rtrace_printf_begin("0x12bc70"); 
rtrace_printf(TYPE_ARG, TYPE_INT, 0, s); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, buf); 
rtrace_printf(TYPE_ARG, TYPE_INT, 2, sizeof buf);
rtrace_printf(TYPE_ARG, TYPE_INT, 3, 0); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 4, &sa); 
socklen_t* recv_argv5={sizeof sa};
rtrace_printf(TYPE_ARG, TYPE_POINTER, 5, recv_argv5); 
	TESTE((rc=recvfrom(s, buf, sizeof buf, 0, (void *)&sa, recv_argv5))==1);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x12bc70"); 

	TEST(buf[0]=='x', "'%c'\n", buf[0]);

rtrace_printf_begin("0x1166e0"); 
rtrace_printf(TYPE_ARG, TYPE_INT, 0, c); 
	rc=close(c);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x1166e0"); 

	close(s);

	memset(&sa, 0, sizeof sa);
	sa.sin_family = AF_INET;
	TESTE((s=socket(PF_INET, SOCK_STREAM|SOCK_CLOEXEC, IPPROTO_TCP))>=0);
	TEST(fcntl(s, F_GETFD)&FD_CLOEXEC, "SOCK_CLOEXEC did not work\n");
rtrace_printf_begin("0x12b930"); 
rtrace_printf(TYPE_ARG, TYPE_INT, 0, s); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, &sa); 
rtrace_printf(TYPE_ARG, TYPE_INT, 2, sizeof sa); 
	TESTE((rc=bind(s, (void *)&sa, sizeof sa))==0);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x12b930"); 

	TESTE(getsockname(s, (void *)&sa, (socklen_t[]){sizeof sa})==0);
	sa.sin_addr.s_addr = htonl(0x7f000001);

rtrace_printf_begin("0x12bb20"); 
rtrace_printf(TYPE_ARG, TYPE_INT, 0, s); 
rtrace_printf(TYPE_ARG, TYPE_INT, 1, 1); 
	TESTE((rc=listen(s, 1))==0);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x12bb20"); 

	TESTE((c=socket(PF_INET, SOCK_STREAM|SOCK_NONBLOCK, IPPROTO_TCP))>=0);
	TEST(fcntl(c, F_GETFL)&O_NONBLOCK, "SOCK_NONBLOCK did not work\n");

rtrace_printf_begin("0x12b960"); 
rtrace_printf(TYPE_ARG, TYPE_INT, 0, c); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, &sa); 
rtrace_printf(TYPE_ARG, TYPE_INT, 2, sizeof sa); 
	TESTE((rc=connect(c, (void *)&sa, sizeof sa))==0 || errno==EINPROGRESS);
rtrace_printf(TYPE_RET, TYPE_INT, 0, rc); 
rtrace_printf_end("0x12b960"); 

rtrace_printf_begin("0x12b7f0"); 
rtrace_printf(TYPE_ARG, TYPE_INT, 0, s); 
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, &sa); 
socklen_t* acc_argv2={sizeof sa};
rtrace_printf(TYPE_ARG, TYPE_POINTER, 2, acc_argv2); 
	TESTE((t=accept(s, (void *)&sa, acc_argv2))>=0);
rtrace_printf(TYPE_RET, TYPE_INT, 0, t); 
rtrace_printf_end("0x12b7f0"); 

	close(t);
	close(c);
	close(s);

	return t_status;
}
