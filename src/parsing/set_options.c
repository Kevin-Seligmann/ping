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
	char *ptr;
	unsigned long preload;

	config->flags.preload = 1;
	preload = strtoul(arg, &ptr, 0);

	if (*ptr || preload > PRELOAD_MAX)
	{
		fprintf(stderr, "invalid preload value (%s)", arg);
		exit(1);
	}
	config->preload = preload;
}

void	get_timeout_option(char *arg, struct s_config *config)
{
	config->flags.timeout = 1;
	config->timeout = get_numeric(arg);
	if (config->timeout > TIMEOUT_MAX)
	{
		fprintf(stderr, "option value too big: %s", arg);
		exit(1);
	}
	if (config->timeout < TIMEOUT_MIN)
	{
		fprintf(stderr, "option value too small: %s", arg);
		exit(1);
	}
}

void	get_linger_option(char *arg, struct s_config *config)
{
	config->flags.linger = 1;
	config->linger = get_numeric(arg);
	if (config->linger > LINGER_MAX)
	{
		fprintf(stderr, "option value too big: %s", arg);
		exit(1);
	}
	if (config->linger < LINGER_MIN)
	{
		fprintf(stderr, "option value too small: %s", arg);
		exit(1);
	}
}

void	get_size_option(char *arg, struct s_config *config)
{
	config->flags.packetsize = 1;
	config->size = get_numeric(arg);
	if (config->size > SIZE_MAX)
	{
		fprintf(stderr, "option value too big: %s", arg);
		exit(1);
	}
	if (config->size < SIZE_MIN)
	{
		fprintf(stderr, "option value too small: %s", arg);
		exit(1);
	}
}

void	get_tos_option(char *arg, struct s_config *config)
{
	config->flags.tos = 1;
	config->tos = get_numeric(arg);
	if (config->tos > TOS_MAX)
	{
		fprintf(stderr, "option value too big: %s", arg);
		exit(1);
	}
	if (config->tos < TOS_MIN)
	{
		fprintf(stderr, "option value too small: %s", arg);
		exit(1);
	}
}

void	get_ttl_option(char *arg, struct s_config *config)
{
	config->flags.ttl = 1;
	config->ttl = get_numeric(arg);
	if (config->ttl > TTL_MAX)
	{
		fprintf(stderr, "option value too big: %s", arg);
		exit(1);
	}
	if (config->ttl < TTL_MIN)
	{
		fprintf(stderr, "option value too small: %s", arg);
		exit(1);
	}
}

void	get_pattern_option(char *arg, struct s_config *config)
{
	config->flags.pattern = 1;

	config->pattern = arg;
	while (*arg)
	{
		if ((*arg >= '0' && *arg <= '9') || (*arg >= 'a' && *arg <= 'z')  || (*arg >= 'A' && *arg <= 'Z'))
			arg ++;
		else
		{
			fprintf(stderr, "error in pattern near %s\n", arg);
			exit(1);
		}
	}
}

void	get_iptimestamp_option(char *arg, struct s_config *config)
{
	config->flags.iptimestamp = 1;

	if (strcasecmp (arg, "tsonly") == 0)
	{
		config->iptimestamp = TSONLY_TS;
	}
	else if (strcasecmp (arg, "tsaddr") == 0)
	{
		config->iptimestamp = TSADDR_TS;
	}
	else
	{
		fprintf(stderr, "unsupported timestamp type: %s", arg);
		exit (1);
	}
}

