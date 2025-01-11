#include "ft_ping_main.h"

void free_resources(struct s_config *config)
{
	free(config->ping.sent_packet_buffer);
	free(config->ping.received_packet_buffer);
	close(config->params.socketfd);
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

	while (config->params.destinations > 0)
	{
		if (is_address(*args))
		{
			config->ping.address = *args;
			(ping(config) == END_ALL_PINGING);
				return EXIT_FAILURE;
			config->params.destinations --;
		}
		args ++;
	}
	return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	int exit_code;
	static struct s_config config;

	set_signal();
	set_defaults(&config.params);
	parse(argc, argv, &config.params);
	prepare_pinging(&config);
	exit_code = run_pings(argv + 1, &config); // Before here is safe to end with exit(), here it gets the exit from the program.
	free_resources(&config);

	return EXIT_SUCCESS;
}
