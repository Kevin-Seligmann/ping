#include "ft_ping_parse.h"

static unsigned long bounded_strtoul_wrapper(char *arg, unsigned int min, unsigned int max)
{
	char *ptr;
	unsigned long result;

	ptr = NULL;
	result = strtoul(arg, &ptr, 0);
	if (*ptr)
	{
		exit_with_message(EXIT_FAILURE, "invalid value (`%s' near `%s')", arg, ptr);
	}
	if (result > max)
	{
		exit_with_message(EXIT_FAILURE, "invalid preload value (%s)", arg);
	}
	if (result < min)
	{
		exit_with_message(EXIT_FAILURE, "option value too small: %s", arg);
	}
	return result;
}

void	get_preload_option(char *arg, struct s_config *config)
{
	char *ptr;
	unsigned long preload;

	config->flags.preload = 1;
	preload = strtoul(arg, &ptr, 0);

	if (*ptr || preload > PRELOAD_MAX)
	{
		exit_with_message(EXIT_FAILURE, "invalid preload value (%s)", arg);
	}
	config->preload = preload;
}

void	get_timeout_option(char *arg, struct s_config *config)
{
	config->flags.timeout = 1;
	config->timeout = bounded_strtoul_wrapper(arg, TIMEOUT_MAX, TIMEOUT_MIN);
}

void	get_linger_option(char *arg, struct s_config *config)
{
	config->flags.linger = 1;
	config->linger = bounded_strtoul_wrapper(arg, LINGER_MAX, LINGER_MIN);
}

void	get_size_option(char *arg, struct s_config *config)
{
	config->flags.packetsize = 1;
	config->size = bounded_strtoul_wrapper(arg, PACKET_SIZE_MAX, PACKET_SIZE_MIN);
}

void	get_tos_option(char *arg, struct s_config *config)
{
	config->flags.tos = 1;
	config->tos = bounded_strtoul_wrapper(arg, TOS_MAX, TOS_MIN);
}

void	get_ttl_option(char *arg, struct s_config *config)
{
	config->flags.ttl = 1;
	config->ttl = bounded_strtoul_wrapper(arg, TTL_MAX, TTL_MIN);
}

void	get_pattern_option(char *arg, struct s_config *config)
{
	config->flags.pattern = 1;

	while (*arg)
	{
		if ((*arg >= '0' && *arg <= '9') || (*arg >= 'a' && *arg <= 'z')  || (*arg >= 'A' && *arg <= 'Z'))
		{
			arg ++;
		}
		else
		{
			exit_with_message( "error in pattern near %s", arg);
		}
	}
	config->pattern = arg;
}

void	get_iptimestamp_option(char *arg, struct s_config *config)
{
	config->flags.iptimestamp = 1;

	if (strcasecmp(arg, "tsonly") == 0)
	{
		config->iptimestamp = TSONLY_TS;
	}
	else if (strcasecmp(arg, "tsaddr") == 0)
	{
		config->iptimestamp = TSADDR_TS;
	}
	else
	{
		exit_with_message(EXIT_FAILURE, "unsupported timestamp type: %s", arg);
	}
}

