#include "ft_ping_parse.h"

static double bounded_strtod_wrapper(char *arg, size_t max, size_t min)
{
	char *ptr;
	double result;

	ptr = NULL;
	result = strtod(arg, &ptr);
	if (*ptr)
	{
		exit_with_message(EXIT_FAILURE, "invalid value (`%s' near `%s')", arg, ptr);
	}
	if (max > 0 && result > max)
	{
		exit_with_message(EXIT_FAILURE, "option value too big: %s", arg);
	}
	if (result < min)
	{
		exit_with_message(EXIT_FAILURE, "option value too small: %s", arg);
	}
	return result;
}

static unsigned long bounded_strtoul_wrapper(char *arg, size_t max, size_t min)
{
	char *ptr;
	unsigned long result;

	ptr = NULL;
	result = strtoul(arg, &ptr, 0);
	if (*ptr)
	{
		exit_with_message(EXIT_FAILURE, "invalid value (`%s' near `%s')", arg, ptr);
	}
	if (max > 0 && result > max)
	{
		exit_with_message(EXIT_FAILURE, "option value too big: %s", arg);
	}
	if (result < min)
	{
		exit_with_message(EXIT_FAILURE, "option value too small: %s", arg);
	}
	return result;
}

void	get_preload_option(char *arg, struct s_program_param *params)
{
	char *ptr;
	unsigned long preload;

	preload = strtoul(arg, &ptr, 0);
	if (*ptr || preload > PRELOAD_MAX)
	{
		exit_with_message(EXIT_FAILURE, "invalid preload value (%s)", arg);
	}
	params->preload = preload;
}

void	get_timeout_option(char *arg, struct s_program_param *params)
{
	params->timeout = bounded_strtoul_wrapper(arg, TIMEOUT_MAX, TIMEOUT_MIN);
}

void	get_linger_option(char *arg, struct s_program_param *params)
{
	params->linger = bounded_strtoul_wrapper(arg, LINGER_MAX, LINGER_MIN);
}

void	get_size_option(char *arg, struct s_program_param *params)
{
	params->size = bounded_strtoul_wrapper(arg, PACKET_SIZE_MAX, PACKET_SIZE_MIN);
}

void	get_tos_option(char *arg, struct s_program_param *params)
{
	params->tos = bounded_strtoul_wrapper(arg, TOS_MAX, TOS_MIN);
}

void	get_ttl_option(char *arg, struct s_program_param *params)
{
	params->ttl = bounded_strtoul_wrapper(arg, TTL_MAX, TTL_MIN);
}

void	get_count_option(char *arg, struct s_program_param *params)
{
	params->count = bounded_strtoul_wrapper(arg, COUNT_MAX, COUNT_MIN);
}

void	get_interval_option(char *arg, struct s_program_param *params)
{
	params->flags |= FTP_INTERVAL;
	params->interval = bounded_strtod_wrapper(arg, INTERVAL_MAX, INTERVAL_MIN);
}

void	get_pattern_option(char *arg, struct s_program_param *params)
{
	params->pattern = arg;
	while (*arg)
	{
		if ((*arg >= '0' && *arg <= '9') || (*arg >= 'a' && *arg <= 'z')  || (*arg >= 'A' && *arg <= 'Z'))
		{
			arg ++;
		}
		else
		{
			exit_with_message(EXIT_FAILURE, "error in pattern near %s", arg);
		}
	}
}

// void	get_iptimestamp_option(char *arg, struct s_program_param *params)
// {
// 	params->flags != FTP_IP_TIMESTAMP;

// 	if (strcasecmp(arg, "tsonly") == 0)
// 	{
// 		params->iptimestamp = TSONLY_TS;
// 	}
// 	else if (strcasecmp(arg, "tsaddr") == 0)
// 	{
// 		params->iptimestamp = TSADDR_TS;
// 	}
// 	else
// 	{
// 		exit_with_message(EXIT_FAILURE, "unsupported timestamp type: %s", arg);
// 	}
// }

