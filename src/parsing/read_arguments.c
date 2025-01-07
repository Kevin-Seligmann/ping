#include "ft_ping_parse.h"

void exit_w_help(int exit_code)
{
	fprintf(stderr, "Try 'ft_ping --help' for more information.\n");
	exit (exit_code);
}

char *get_argument(char **argv, int *i, char** option)
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
	fprintf(stderr, "ft_ping: option requires an argument -- '%c'\n", **option);
	exit_w_help(64);
	return NULL;
}

void parse_option(char **argv, int *i, struct s_config *input)
{
	char *option;

	option = argv[*i] + 1;
	while (option)
	{
		if (*option == '?' || *option == 'h')
			usage(0);
		else if (*option == 'v')
			input->flags.verbose = 1;
		else if (*option ==  'f')
			input->flags.floodping = 1;
		else if (*option == 'n')
			input->flags.numericOutput = 1;
		else if (*option == 'r')
			input->flags.bypassNormalRoutingTables = 1;
		else if (*option ==  'l')
			return get_preload_option(get_argument(argv, i, &option), input);
		else if (*option == 'w')
			return get_linger_option(get_argument(argv, i, &option), input);
		else if (*option == 'W')
			return get_timeout_option(get_argument(argv, i, &option), input);
		else if (*option == 'p')
			return get_pattern_option(get_argument(argv, i, &option), input);
		else if (*option ==  's')
			return get_size_option(get_argument(argv, i, &option), input);
		else if (*option ==  'T')
			return get_iptimestamp_option(get_argument(argv, i, &option), input);
		else if (*option ==  't')
			return get_ttl_option(get_argument(argv, i, &option), input);
		else
		{
			fprintf(stderr, "ft_ping: invalid option -- '%c'\n", *option);
			exit_w_help(64);
		}
		option ++;
	}
}

int matches_full_option(char *arg, char *option, int needs_argument)
{
	int ind;

	ind = 0;
	while(arg[ind] == option[ind])
		ind ++;
	if (arg[ind])
		return 0;
	if (!option[ind])
		return 1;
	if (!needs_argument && option[ind] == '=')
	{
		fprintf(stderr, "ft_ping: option '--%s' doesn't allow an argument\n", option);
		exit_w_help(64);
	}
	if (needs_argument && option[ind] == '=')
		return 1;
	return 0;
}


char *get_full_argument(char **argv, int *i, char *option)
{
	char *arg;

	arg = argv[*i] + strlen(option);
	if (*arg == '=')
		return (arg + 1);
	(*i) ++;
	if (argv[*i])
		return argv[*i];
	fprintf(stderr, "ft_ping: option requires an argument -- '%s'\n", option);
	exit_w_help(64);
	return NULL;
}


void parse_full_option(char **argv, int *i, struct s_config *input)
{
	char *arg;

	arg = argv[*i] + 2;
	if (matches_full_option(arg, "help", 0))
		usage(0);
	else if (matches_full_option(arg, "verbose", 0))
		input->flags.verbose = 1;
	else if (matches_full_option(arg, "flood", 0))
		input->flags.floodping = 1;
	else if (matches_full_option(arg, "numeric", 0))
		input->flags.numericOutput = 1;
	else if (matches_full_option(arg, "ignore-routing", 0))
		input->flags.bypassNormalRoutingTables = 1;
	else if (matches_full_option(arg, "preload", 1))
		get_preload_option(get_full_argument(argv, i, "preload"), input);
	else if (matches_full_option(arg, "timeout", 1))
		get_timeout_option(get_full_argument(argv, i, "timeout"), input);
	else if (matches_full_option(arg, "linger", 1))
		get_linger_option(get_full_argument(argv, i, "linger"), input);
	else if (matches_full_option(arg, "pattern", 1))
		get_pattern_option(get_full_argument(argv, i, "pattern"), input);
	else if (matches_full_option(arg, "size", 1))
		get_size_option(get_full_argument(argv, i, "size"), input);
	else if (matches_full_option(arg, "tos", 1))
		get_tos_option(get_full_argument(argv, i, "tos"), input);
	else if (matches_full_option(arg, "ttl", 1))
		get_ttl_option(get_full_argument(argv, i, "ttl"), input);
	else if (matches_full_option(arg, "ip-timestamp", 1))
		get_iptimestamp_option(get_full_argument(argv, i, "ip-timestamp"), input);
	else
	{
		fprintf(stderr, "ft_ping: unrecognized option '--%s'\n", arg);
		exit_w_help(64);
	}
}

void parse(int argc, char **argv, struct s_config *input)
{
	int i;

	i = 1;
	while (i < argc)
	{
		if (strlen(argv[i]) > 2 && argv[i][0] == '-' && argv[i][1] == '-')
		{
			parse_full_option(argv, &i, input);
		}
		else if (argv[i][0] == '-' && strlen(argv[i]) > 1)
		{
			parse_option(argv, &i, input);
		}
		else
		{
			input->address_count ++;
		}
		i ++;
	}
	if (input->address_count == 0)
	{
		fprintf(stderr, "ft_ping: missing host operand\n");
		exit_w_help(64);
	}
}
