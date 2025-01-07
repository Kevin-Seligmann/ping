#include "ft_ping_main.h"

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

static void run_pings(char **args, struct s_config *config)
{
	args ++;
	while (*args)
	{
		if (is_address(*args))
		{
			config->address = *args;
			ping(config);
		}
		args ++;
	}
}

int main(int argc, char **argv)
{
	static struct s_config config;

	set_defaults(&config);
	parse(argc, argv, &config);
	configurate_socket(&config);
	allocate_packet_buffers(&config);
	fill_received_packet_buffer(&config);
	run_pings(argv + 1, &config);
	free_resources(&config);

	return 0;
}
