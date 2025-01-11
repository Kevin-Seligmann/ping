#include "ft_ping_main.h"

extern volatile sig_atomic_t status;

void free_resources(struct s_config *config)
{
	free(config->received_packet_buffer);
	free(config->sent_packet_buffer);
	close(config->socketfd);
}

static int is_address(char *str)
{
	if (strlen(str) > 1 && str[0] == '-')
		return 0;
	return 1;
}

static int run_pings(char **args, struct s_config *config)
{
	int rta;

	gettimeofday(&(config->starting_time), 0);
	while (config->address_count > 0)
	{
		if (is_address(*args))
		{
			config->address = *args;
			(ping(config) == END_ALL_PINGING);
				return EXIT_FAILURE;
			config->address_count --;
		}
		args ++;
	}
	return EXIT_SUCCESS;
}

void handle_sigint(int signum)
{
    (void)signum;

    status = END_ALL_PINGING;
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
}

int main(int argc, char **argv)
{
	int exit_code;
	static struct s_config config;

	set_signal();
	set_defaults(&config);
	parse(argc, argv, &config);
	// configurate_socket(&config);
	// allocate_packet_buffers(&config);
	// fill_received_packet_buffer(&config);
	// exit_code = run_pings(argv + 1, &config); // Before here is safe to end with exit(), here it gets the exit from the program.
	// free_resources(&config);

	return EXIT_SUCCESS;
}
