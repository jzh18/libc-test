#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include "test.h"

volatile int t_status = 0;

int t_printf(const char *s, ...)
{
	va_list ap;
	char buf[512];
	int n;

	t_status = 1;
	va_start(ap, s);
	n = vsnprintf(buf, sizeof buf, s, ap);
	va_end(ap);
	if (n < 0)
		n = 0;
	else if (n >= sizeof buf)
	{
		n = sizeof buf;
		buf[n - 1] = '\n';
		buf[n - 2] = '.';
		buf[n - 3] = '.';
		buf[n - 4] = '.';
	}
	return write(1, buf, n);
}


void rtrace_printf_init()
{
	// Open log file in write mode to clear previous content
	FILE *log_file = fopen("rtrace.log", "w");
	if (!log_file)
		return;

	// Write initial message to the log file
	fprintf(log_file, "[RTRACE] Initialized\n");
	fclose(log_file);
}

void rtrace_printf_begin(const char* func_addr)
{
	// Open log file in append mode
	FILE *log_file = fopen("rtrace.log", "a");
	if (!log_file)
		return;

	// Write header to the log file
	fprintf(log_file, "[RTRACE] Trace started: %s\n", func_addr);
	fclose(log_file);
}

void rtrace_printf_end(const char* func_addr)
{
	// Open log file in append mode
	FILE *log_file = fopen("rtrace.log", "a");
	if (!log_file)
		return;

	// Write footer to the log file
	fprintf(log_file, "[RTRACE] Trace ended: %s\n", func_addr);
	fclose(log_file);
}

void rtrace_printf(ExeType exe_arg, ValType type, int index, void *value)
{
	const char *prefix = "[RTRACE]";
	const char *entry_exit = (exe_arg == TYPE_ARG) ? "ARG" : "RET";

	// Open log file in append mode
	FILE *log_file = fopen("rtrace.log", "a");
	if (!log_file)
		return;

	switch (type)
	{
	case TYPE_INT:
		fprintf(log_file, "%s-%s-%d-INT: %ld\n", prefix, entry_exit, index, value);
		break;
	case TYPE_FLOAT:
		fprintf(log_file, "%s-%s-%d-FLOAT: %f\n", prefix, entry_exit, index, *(float *)value);
		break;
	case TYPE_POINTER:
		fprintf(log_file, "%s-%s-%d-POINTER: %lu\n", prefix, entry_exit, index, value);
		break;
	default:
		fprintf(log_file, "%s-%s-%d: UNKNOWN TYPE\n", prefix, entry_exit, index);
		break;
	}

	fclose(log_file);
}