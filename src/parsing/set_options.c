#include "parsing.h"

void set_defaults(struct s_config *input)
{
	struct protoent *protocol;

	input->interval = 1;
	input->size = 56;
	input->ttl = 64;
	input->sequence = 1;

	protocol = getprotobyname("icmp");
	if (!protocol)
	{
    	fprintf(stderr, "ft_ping: Unable to find protocol 'icmp'\n");
   		exit(1);
	}
    input->hints.ai_flags = AI_CANONNAME;

    input->hints.ai_family = AF_INET;
    input->hints.ai_socktype = SOCK_RAW;
    input->hints.ai_protocol = protocol->p_proto;
}


void	get_preload_option(char *arg, struct s_config *config)
{
	config->flags.preload = 1;
}

void	get_timeout_option(char *arg, struct s_config *config)
{
	config->flags.timeout = 1;
}

void	get_linger_option(char *arg, struct s_config *config)
{
	config->flags.linger = 1;
}

void	get_pattern_option(char *arg, struct s_config *config)
{
	config->flags.pattern = 1;
}

void	get_size_option(char *arg, struct s_config *config)
{
	config->flags.packetsize = 1;

}

void	get_tos_option(char *arg, struct s_config *config)
{
	config->flags.tos = 1;

}

void	get_ttl_option(char *arg, struct s_config *config)
{
	config->flags.ttl = 1;

}

void	get_iptimestamp_option(char *arg, struct s_config *config)
{
	config->flags.iptimestamp = 1;

}

