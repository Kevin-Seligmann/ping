#include "ft_ping.h"

volatile sig_atomic_t ft_ping_status;

void handle_sigint(int signum)
{
	(void)signum;

	ft_ping_status = END_ALL_PINGING;
}

void set_signal()
{
	struct sigaction sa;

	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
	exit_with_message(EXIT_FAILURE, "%s: signal", strerror(errno));
	}
	ft_ping_status = PINGING;
}
