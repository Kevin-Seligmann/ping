#include "ft_ping.h"

#include <stdarg.h>

void usage(int exit_code){
printf("\
Usage: ./ft_ping [OPTION...] HOST ...\n\
Send ICMP ECHO_REQUEST packets to network hosts.\n\
\n\
  -n, --numeric              do not resolve host addresses\n\
  -r, --ignore-routing       send directly to a host on an attached network\n\
      --ttl=N                specify N as time-to-live\n\
  -T, --tos=NUM              set type of service (TOS) to NUM\n\
  -v, --verbose              verbose output\n\
  -w, --timeout=N            stop after N seconds\n\
  -W, --linger=N             number of seconds to wait for response\n\
  -f, --flood                flood ping (root only)\n\
      --ip-timestamp=FLAG    IP timestamp of type FLAG, which is one of\n\
                             \"tsonly\" and \"tsaddr\"\n\
  -l, --preload=NUMBER       send NUMBER packets as fast as possible before\n\
                             falling into normal mode of behavior (root only)\n\
  -p, --pattern=PATTERN      fill ICMP packet with given pattern (hex)\n\
  -s, --size=NUMBER          send NUMBER data octets\n\
\n\
  -?, -h, --help             give this help list\n\
\n\
Mandatory or optional arguments to long options are also mandatory or optional\n\
for any corresponding short options.\n\
\n\
School project made by Kevin Seligmann for 42. Based on ineutils-2.0's ping.\n\
");
	exit(exit_code);
}

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
