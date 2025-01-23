#include "ft_ping_parse.h"

void set_options(struct s_options **options)
{
	static struct s_options opt[OPTIONS_COUNT] = {
	{.char_name = 'c', .str_name = "count", .action = NULL, .argument_getter = get_count_option, .flag_value = 0},
	{.char_name = 'd', .str_name = "debug", .action = NULL, .argument_getter = NULL, .flag_value = FTP_DEBUG},
	{.char_name = 'l', .str_name = "preload", .action = NULL, .argument_getter = get_preload_option, .flag_value = 0},
	{.char_name = 'w', .str_name = "timeout", .action = NULL, .argument_getter = get_timeout_option, .flag_value = 0},
	{.char_name = 'W', .str_name = "linger", .action = NULL, .argument_getter = get_linger_option, .flag_value = 0},
	{.char_name = 'p', .str_name = "pattern", .action = NULL, .argument_getter = get_pattern_option, .flag_value = 0},
	{.char_name = 's', .str_name = "size", .action = NULL, .argument_getter = get_size_option, .flag_value = 0},
	{.char_name = 'T', .str_name = "tos", .action = NULL, .argument_getter = get_tos_option, .flag_value = 0},
	{.char_name = 0, .str_name = "ttl", .action = NULL, .argument_getter = get_ttl_option, .flag_value = 0},
	{.char_name = 'v', .str_name = "verbose", .action = NULL, .argument_getter = NULL, .flag_value = FTP_VERBOSE},
	{.char_name = 'f', .str_name = "flood", .action = NULL, .argument_getter = NULL, .flag_value = FTP_FLOOD},
//	{.char_name = 'n', .str_name = "numeric", .action = NULL, .argument_getter = NULL, .flag_value = FTP_NUMERIC},
	{.char_name = 'r', .str_name = "ignore-routing", .action = NULL, .argument_getter = NULL, .flag_value = FTP_BYPASS_ROUTING_TABLE},
//	{.char_name = 0, .str_name = "ip-timestamp", .action = NULL, .argument_getter = get_iptimestamp_option, .flag_value = FTP_IP_TIMESTAMP},
	{.char_name = 'q', .str_name = "quiet", .action = NULL, .argument_getter = NULL, .flag_value = FTP_QUIET},
	{.char_name = 'i', .str_name = "interval", .action = NULL, .argument_getter = get_interval_option, .flag_value = FTP_INTERVAL},
	{.char_name = '?', .str_name = "help", .action = usage, .argument_getter = NULL, .flag_value = 0},
	{.char_name = 'V', .str_name = "version", .action = version, .argument_getter = NULL, .flag_value = 0},
	};
	*options = opt;
}

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

static void parse_option(char **argv, int *i, struct s_program_param *params, struct s_options *opts)
{
	char *option;
	int opt_ind;
	struct s_options *opt;

	option = argv[*i] + 1;
	opt_ind = 0;
	while (*option)
	{
		for (opt_ind = 0; opt_ind < OPTIONS_COUNT; opt_ind ++)
		{
			opt = &(opts[opt_ind]);
			if (opt->char_name != *option)
				continue ;
			if (opt->flag_value)
				params->flags |= opt->flag_value;
			if (opt->argument_getter)
				return opt->argument_getter(get_argument(argv, i, &option), params);
			if (opt->action)
				opt->action();
			break ;
		}
		if (opt_ind == OPTIONS_COUNT)
			exit_with_help(USAGE_FAILURE, "invalid option -- '%c'", *option);
		option ++;
	}
}

static int matches_full_option(char *arg, char *option, void (*argument_getter)(char *, struct s_program_param *))
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
		if (!argument_getter)
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

static void parse_full_option(char **argv, int *i, struct s_program_param *params, struct s_options *opts)
{
	char *arg;
	int opt_ind;
	struct s_options *opt;

	arg = argv[*i] + 2;
	for (opt_ind = 0; opt_ind < OPTIONS_COUNT; opt_ind ++)
	{
		opt = &(opts[opt_ind]);
		if (matches_full_option(arg, opt->str_name, opt->argument_getter))
		{
			if (opt->flag_value)
				params->flags |= opt->flag_value;
			if (opt->argument_getter)
				opt->argument_getter(get_full_argument(argv, i, opt->str_name), params);
			if (opt->action)
				opt->action();
			return ;
		}
	}
	exit_with_help(USAGE_FAILURE, "unrecognized option '--%s'", arg);
}

static int should_skip(char *str, struct s_options *opts)
{
	char last_char;
	int	opt_ind;
	struct s_options *opt;

	if (str[0] == '-' && str[1] != '-')
	{
		last_char = str[strlen(str) - 1];
		for (opt_ind = 0; opt_ind < OPTIONS_COUNT; opt_ind ++)
		{
			opt = &(opts[opt_ind]);
			if (opt->char_name == last_char && opt->argument_getter)
				return 1;
		}
		return 0;
	}
	for (opt_ind = 0; opt_ind < OPTIONS_COUNT; opt_ind ++)
	{
		opt = &(opts[opt_ind]);
		if (!opt->argument_getter && matches_full_option(str + 2, opt->str_name, 0))
			return 0;
	}
	return 1;
}

int is_address(char ***args, char *str, struct s_options *opts)
{
	if (strlen(str) > 1 && str[0] == '-')
	{
		if (should_skip(str, opts))
			(*args) ++;
		return 0;
	}
	return 1;
}

void parse(int argc, char **argv, struct s_config *config)
{
	int i;

	set_options(&config->options);
	i = 1;
	while (i < argc)
	{
		if (strlen(argv[i]) > 2 && argv[i][0] == '-' && argv[i][1] == '-')
		{
			parse_full_option(argv, &i, &config->params, config->options);
		}
		else if (argv[i][0] == '-' && strlen(argv[i]) > 1)
		{
			parse_option(argv, &i, &config->params, config->options);
		}
		else
		{
			config->params.destinations ++;
		}
		i ++;
	}
	if (config->params.destinations == 0)
	{
		exit_with_help(USAGE_FAILURE, "missing host operand");
	}
	if (config->params.flags & FTP_FLOOD && config->params.flags & FTP_INTERVAL)
	{
		exit_with_message(EXIT_FAILURE, "-f and -i incompatible options");
	}
}
