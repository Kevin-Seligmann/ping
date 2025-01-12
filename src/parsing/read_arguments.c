#include "ft_ping_parse.h"

static char *get_argument(char **argv, int *i, char** option)
{
	(*option) ++;
	if (**option)
	{
		return *option;
	}
	(*i) ++;
	if (argv[*i])
	{
		return argv[*i];
	}
	(*option) --;
	exit_with_help(USAGE_FAILURE, "option requires an argument -- '%c'", **option);
	return NULL;
}

static void parse_option(char **argv, int *i, struct s_program_param *params)
{
	char *option;

	option = argv[*i] + 1;
	while (*option)
	{
		if (*option == '?' || *option == 'h')
			usage(0);
		else if (*option == 'v')
			params->flags |= FTP_VERBOSE;
		else if (*option ==  'f')
			params->flags |= FTP_FLOOD;
		else if (*option == 'n')
			params->flags |= FTP_NUMERIC;
		else if (*option == 'r')
			params->flags |= FTP_BYPASS_ROUTING_TABLE;
		else if (*option == 'q')
			params->flags |= FTP_QUIET;
		else if (*option == 'd')
			params->flags |= FTP_DEBUG;
		else if (*option ==  'l')
			return get_preload_option(get_argument(argv, i, &option), params);
		else if (*option == 'w')
			return get_linger_option(get_argument(argv, i, &option), params);
		else if (*option == 'W')
			return get_timeout_option(get_argument(argv, i, &option), params);
		else if (*option == 'p')
			return get_pattern_option(get_argument(argv, i, &option), params);
		else if (*option ==  's')
			return get_size_option(get_argument(argv, i, &option), params);
		else if (*option ==  'T')
			return get_tos_option(get_argument(argv, i, &option), params);
		else if (*option ==  't')
			return get_ttl_option(get_argument(argv, i, &option), params);
		else
			exit_with_help(USAGE_FAILURE, "invalid option -- '%c'", *option);
		option ++;
	}
}

static int matches_full_option(char *arg, char *option, int needs_argument)
{
	int ind;

	ind = 0;
	while (option[ind] && arg[ind] && option[ind] == arg[ind])
		ind ++;
	if (!option[ind] && !arg[ind])
		return 1;
	if (option[ind])
		return 0;
	if (arg[ind] == '=')
	{
		if (!needs_argument)
		{
			exit_with_help(USAGE_FAILURE, "option '--%s' doesn't allow an argument", option);
		}
		return 1;
	}
	return 0;
}

static char *get_full_argument(char **argv, int *i, char *option)
{
	char *arg;

	arg = argv[*i] + strlen(option) + 2;
	if (*arg == '=')
		return (arg + 1);
	(*i) ++;
	if (argv[*i])
		return argv[*i];
	exit_with_help(USAGE_FAILURE, "option requires an argument -- '%s'", option);
	return NULL;
}

static void parse_full_option(char **argv, int *i, struct s_program_param *params)
{
	char *arg;

	arg = argv[*i] + 2;
	if (matches_full_option(arg, "help", 0))
		usage(0);
	else if (matches_full_option(arg, "verbose", 0))
		params->flags |= FTP_VERBOSE;
	else if (matches_full_option(arg, "flood", 0))
		params->flags |= FTP_FLOOD;
	else if (matches_full_option(arg, "numeric", 0))
		params->flags |= FTP_NUMERIC;
	else if (matches_full_option(arg, "ignore-routing", 0))
		params->flags |= FTP_BYPASS_ROUTING_TABLE;
	else if (matches_full_option(arg, "quiet", 0))
		params->flags |= FTP_QUIET;
	else if (matches_full_option(arg, "debug", 0))
		params->flags |= FTP_DEBUG;
	else if (matches_full_option(arg, "preload", 1))
		get_preload_option(get_full_argument(argv, i, "preload"), params);
	else if (matches_full_option(arg, "timeout", 1))
		get_timeout_option(get_full_argument(argv, i, "timeout"), params);
	else if (matches_full_option(arg, "linger", 1))
		get_linger_option(get_full_argument(argv, i, "linger"), params);
	else if (matches_full_option(arg, "pattern", 1))
		get_pattern_option(get_full_argument(argv, i, "pattern"), params);
	else if (matches_full_option(arg, "size", 1))
		get_size_option(get_full_argument(argv, i, "size"), params);
	else if (matches_full_option(arg, "tos", 1))
		get_tos_option(get_full_argument(argv, i, "tos"), params);
	else if (matches_full_option(arg, "ttl", 1))
		get_ttl_option(get_full_argument(argv, i, "ttl"), params);
	else if (matches_full_option(arg, "ip-timestamp", 1))
		get_iptimestamp_option(get_full_argument(argv, i, "ip-timestamp"), params);
	else
		exit_with_help(USAGE_FAILURE, "unrecognized option '--%s'", arg);
}

void parse(int argc, char **argv, struct s_program_param *params)
{
	int i;

	i = 1;
	while (i < argc)
	{
		if (strlen(argv[i]) > 2 && argv[i][0] == '-' && argv[i][1] == '-')
		{
			parse_full_option(argv, &i, params);
		}
		else if (argv[i][0] == '-' && strlen(argv[i]) > 1)
		{
			parse_option(argv, &i, params);
		}
		else
		{
			params->destinations ++;
		}
		i ++;
	}
	if (params->destinations == 0)
	{
		exit_with_help(USAGE_FAILURE, "missing host operand");
	}
}
