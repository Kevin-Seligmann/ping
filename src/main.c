#include "ft_ping_main.h"

extern volatile sig_atomic_t ft_ping_status;

void free_resources(struct s_config *config)
{
	free(config->ping.sent_packet_buffer);
	free(config->ping.received_packet_buffer);
	close(config->params.socketfd);
}

static void run_pings(char **args, struct s_config *config)
{
	gettimeofday(&config->ping.time.starting_time, 0);
	while (config->params.destinations > 0)
	{
		if (is_address(&args, *args, config->options))
		{
			config->ping.destination = *args;
			ping(config);
			if (ft_ping_status == END_CURR_PINGING)
				ft_ping_status = PINGING;
			config->params.destinations --;
		}
		args ++;
	}
}

int main(int argc, char **argv)
{
	static struct s_config config;

	set_signal();
	set_defaults(&config.params);
	parse(argc, argv, &config.params, config.options);
	prepare_pinging(&config);
	run_pings(argv + 1, &config);
	free_resources(&config);
	return EXIT_SUCCESS;
}
