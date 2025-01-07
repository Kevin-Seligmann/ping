#include "pre_ping_config.h"

void set_defaults(struct s_config *input)
{
	struct protoent *protocol;

	input->interval = 1;
	input->size = 56;
	input->sequence = 1;
	// input->linger??

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

void configurate_timestamp_option(struct s_config *config)
{
	uint8_t option[4];

	option[0] = 0x44; // Type = TS;
	option[1] = 40; // Length (Max- 40 bytes)
	option[2] = 5; // Where to start writting (5th byte after option[0])
	option[3] = config->iptimestamp; // 0 or 1, meaning TS or TS and ADDR
    if (setsockopt(config->socketfd, IPPROTO_IP, IP_OPTIONS, &option, sizeof(option)) < 0)
	{
		fprintf(stderr, "ft_ping: %s: setsockopt(TIMESTAMP-OPTIONS)\n", strerror(errno));
        close(config->socketfd);
        exit(1);
    }
}

void configurate_socket(struct s_config *config)
{
	int aux = 1;

	config->socketfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (errno == EPERM || errno == EACCES || errno == EPROTONOSUPPORT)
	{
		fprintf (stderr, "ft_ping: Lacking privilege for icmp socket.\n");
		exit(1);
	}
	else
	{
		fprintf (stderr, "ft_ping: %s\n", strerror (errno));
		exit(1);
	}
	if(setsockopt(config->socketfd, SOL_SOCKET, SO_BROADCAST, &aux, sizeof(aux)) < 0) 
	{
		fprintf(stderr, "ft_ping: %s: setsockopt(BROADCAST)\n", strerror(errno));
		close(config->socketfd);
		exit(1);
	}
	if (config->flags.bypassNormalRoutingTables)
	{
		if(setsockopt(config->socketfd, SOL_SOCKET, SO_DONTROUTE, &aux, sizeof(aux)) < 0) 
		{
			fprintf(stderr, "ft_ping: %s: setsockopt(DONTROUTE)\n", strerror(errno));
			close(config->socketfd);
			exit(1);
		}
	}
	if (config->flags.ttl)
	{	
		if(setsockopt(config->socketfd, IPPROTO_IP, IP_TTL, (void *) &(config->ttl), sizeof(config->ttl)) < 0) 
		{
			fprintf(stderr, "ft_ping: %s: setsockopt(TTL)\n", strerror(errno));
			close(config->socketfd);
			exit(1);
		}
	}
	if (config->flags.tos)
	{
		if(setsockopt(config->socketfd, IPPROTO_IP, IP_TOS, (void *) &(config->tos), sizeof(config->tos)) < 0) 
		{
			fprintf(stderr, "ft_ping: %s: setsockopt(TOS)\n", strerror(errno));
			close(config->socketfd);
			exit(1);
		}
	}
	if (config->flags.iptimestamp)
	{
		configurate_timestamp_option(config);
	}
}

void allocate_packet_buffers(struct s_config *config)
{
	config->received_packet_buffer = malloc(config->size + 8 + 40); //Size + icmp header + max ip header size
	if (config->received_packet_buffer)
		config->sent_packet_buffer = malloc(config->size + 8); // Without ip header
	if (!config->received_packet_buffer || !config->sent_packet_buffer)
	{
		free(config->received_packet_buffer);
		free(config->sent_packet_buffer);
		close(config->socketfd);
		fprintf(stderr, "ft_ping %s: malloc\n", strerror(errno));
		exit(1);
	}
}

uint8_t get_byte_from_char(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else
		return c - 'A' + 10;
}

void pad_packet(void *buffer, char *pattern, uint8_t size)
{
	uint8_t *buffptr;
	int	side;
	char *patptr;
	
	buffptr = buffer;
	side = 1;
	while (size > 0)
	{
		patptr = pattern;
		while (*patptr && size > 0)
		{
			*buffptr |= get_byte_from_char(*patptr) << side * 4;
			side = !side;
			patptr ++;
			if (!side)
			{
				buffptr ++;
				size --;
			}
		}
	}
}

void fill_received_packet_buffer(struct s_config *config)
{
	if (config->pattern)
	{
		pad_packet(config->sent_packet_buffer + 8, config->pattern, config->size);
	}
	else
	{
		memset(config->sent_packet_buffer + 8, 0, config->size);
	}
}
