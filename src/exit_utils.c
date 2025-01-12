#include "ft_ping.h"

#include <stdarg.h>

static void print_exit_message(const char *exit_msg, va_list args)
{
	fprintf(stderr, "%s: ", PROGRAM_NAME);
	vfprintf(stderr, exit_msg, args);
}

void exit_with_help(int exit_code, const char *exit_msg, ...)
{
	va_list args;

	va_start(args, exit_msg);
	print_exit_message(exit_msg, args);
	va_end(args);
	fprintf(stderr, "\nTry 'ft_ping --help' for more information.\n");
	exit (exit_code);
}

int return_with_message(int return_code, const char *exit_msg, ...)
{
    va_list args;

	va_start(args, exit_msg);
	print_exit_message(exit_msg, args);
    va_end(args);
    fprintf(stderr, "\n");
    return (return_code);
}

void exit_with_message(int exit_code, const char *exit_msg, ...)
{
    va_list args;

	va_start(args, exit_msg);
	print_exit_message(exit_msg, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(exit_code);
}

void exit_wmsg_and_free(struct s_config *config, int exit_code, char *exit_msg, ...)
{
    va_list args;

	va_start(args, exit_msg);
	print_exit_message(exit_msg, args);
    va_end(args);
    fprintf(stderr, "\n");
	freeaddrinfo(config->ping.addr);
	free(config->ping.received_packet_buffer);
	free(config->ping.sent_packet_buffer);
    close(config->params.socketfd);
    exit(exit_code);
}
