#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#include <stdlib.h>
#include <search.h>
#include "test.h"

struct q {
	struct q *n;
	struct q *p;
	int i;
};

static struct q *new(int i)
{
	struct q *q = malloc(sizeof *q);
	q->i = i;
	return q;
}

int main()
{
	rtrace_printf_init();
	struct q *q = new(0);
	struct q *p;
	int i;

	insque(q, 0);
	for (i = 1; i < 10; i++) {

		struct q *input = new(i);
rtrace_printf_begin("0x124d60");
rtrace_printf(TYPE_ARG, TYPE_POINTER, 0, input);
rtrace_printf(TYPE_ARG, TYPE_POINTER, 1, q);
		insque(input, q);
rtrace_printf_end("0x124d60");
		q = q->n;
	}
	p = q;
	while (q) {
		if (q->i != --i)
			t_error("walking queue: got %d, wanted %d\n", q->i, i);
		q = q->p;
	}
	remque(p->p);
	if (p->p->i != p->i-2)
		t_error("remque: got %d, wanted %d\n", p->p->i, p->i-2);
	if (p->p->n->i != p->i)
		t_error("remque: got %d, wanted %d\n", p->p->n->i, p->i);
	return t_status;
}
