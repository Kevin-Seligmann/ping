#include "ft_ping.h"

// When it finds something that requires an argument, everything after is part of the argument (Or the next inpt)
// Multiple flags on an option is possible
// When it finds help, immediately does it and exits

	// struct s_flags {
	// 	int verbose; // -v
	// 	int help; // -? or --help or -h
	// 	int floodping; // -f 
	// 	int preload; // -l 
	// 	int numericOutput; // -n 
	// 	int deadline; // -w 
	// 	int timeout; // -W 
	// 	int pattern; // -p 
	// 	int bypassNormalRoutingTables; // -r 
	// 	int packetsize; // -s 
	// 	int timestamp; // -T 
	// 	int timeToLive; // --ttl or -t
	// 	// --ip-timestamp
	// };

void usage(int exit_code){
	printf("Based on PING, implementation from inetutils-2.0 (man (8))\n\n");
	printf("\
Usage \n\
ft_ping [options] <destination> \n\
\n\
Options: \n\
<destination>      dns name or ip address \n\
-f                 flood ping \n\
-h -? --help       print help and exit \n\
-l <preload>       send <preload> number of packages while waiting replies\n\
-n                 no dns name resolution\n\
-p <pattern>       contents of padding byte\n\
-s <size>          use <size> as number of data bytes to be sent\n\
-t <ttl>           define time to live\n\
-v                 verbose output\n\
-w <deadline>      reply wait <deadline> in seconds\n\
-W <timeout>       time to wait for response\n\
-T <timestamp>     define timestamp, can be one of <tsonly|tsandaddr|tsprespec>\n");
	printf("\nFT_PING: Warning: Until further development, this program is in good-faith mode\n");
	printf("Which means that every flag that requires an attribute, will have it provided on the next input\n");
	printf("And that anything that is not strictly a flag, will be the destination, which will be overriden if multiple are given\n");
	printf("This is due to the moral dificulty of code parsing on an exciting networking project!\n");
	exit(exit_code);
}

int is_decimal(char *str)
{
	while (*str == ' ')
		str ++;
	if (*str ==  '+' || *str == '-')
		str ++;
	if (!*str)
		return 0;
	while (*str)
	{
		if (*str < '0' || * str > '9')
		{
			return 0;
		}
		str ++;
	}
	return 1;
}

void init(struct s_config *input){
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

void get_preload_option(char *arg, struct s_config *input)
{
	input->flags.preload = 1;
	if (!is_decimal(arg))
	{
		fprintf(stderr, "ft_ping: invalid argument: '%s'\n", arg);
		exit(1);
	}
	input->preload = atoi(arg);
	if (strlen(arg) > 5 || input->preload < 1 || input->preload > 65536)
	{
		fprintf(stderr, "ft_ping: invalid argument: '%s': out of range: 1 <= value <= 65536\n", arg);
		exit(1);
	}	
}

void get_deadline_option(char *arg, struct s_config *input)
{
	input->flags.deadline = 1;

}

void get_timeout_option(char *arg, struct s_config *input)
{
	input->flags.timeout = 1;

}

void get_pattern_option(char *arg, struct s_config *input)
{
	input->flags.pattern = 1;

}

void get_packetsize_option(char *arg, struct s_config *input)
{
	ssize_t size;

	input->flags.packetsize = 1;
	if (!is_decimal(arg))
	{
		fprintf(stderr, "ft_ping: invalid argument: '%s'\n", arg);
		exit(1);
	}
	size = atoi(arg);
	if (strlen(arg) > 10 || size < 0 || size > 2147483647)
	{
		fprintf(stderr, "ft_ping: invalid argument: '%s': out of range: 0 <= value <= 2147483647\n", arg);
		exit(1);
	}
	input->size = (int) size;
}

void get_timestamp_option(char *arg, struct s_config *input)
{
	input->flags.timestamp = 1;

}

void get_ttl_option(char *arg, struct s_config *input)
{
	input->flags.ttl = 1;

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
	usage(2);
	return NULL;
}

void parse_option(int argc, char **argv, int *i, struct s_config *input)
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
			return get_deadline_option(get_argument(argv, i, &option), input);
		else if (*option == 'W')
			return get_timeout_option(get_argument(argv, i, &option), input);
		else if (*option == 'p')
			return get_pattern_option(get_argument(argv, i, &option), input);
		else if (*option ==  's')
			return get_packetsize_option(get_argument(argv, i, &option), input);
		else if (*option ==  'T')
			return get_timestamp_option(get_argument(argv, i, &option), input);
		else if (*option ==  't')
			return get_ttl_option(get_argument(argv, i, &option), input);
		else
		{
			fprintf(stderr, "ft_ping: invalid option -- '%c'", *option);
			usage(2);
		}
		option ++;
	}
}

void parse(int argc, char **argv, struct s_config *input)
{
	int i;

	init(input);
	i = 1;
	while (i < argc)
	{
		if (argv[i][0] == '-' && strlen(argv[i]) > 1)
		{
			parse_option(argc, argv, &i, input);
		}
		else
		{
			input->destination = argv[i];
		}
		i ++;
	}
}
