#include "ft_ping.h"

void set_defaults(struct s_program_param *params)
{
	struct protoent *protocol;

	protocol = getprotobyname("icmp");
	if (!protocol)
		exit_with_message(EXIT_FAILURE, "Unable to find protocol 'icmp'");
    params->hints.ai_flags = AI_CANONNAME;
    params->hints.ai_family = AF_INET;
    params->hints.ai_socktype = SOCK_RAW;
    params->hints.ai_protocol = protocol->p_proto;
	params->interval = 1;
	params->size = 56;
	params->interval = 1000;
	// input->linger??
}

static void allocate_packet_buffers(struct s_config *config)
{
	int err;

	config->ping.received_packet_buffer = malloc(config->params.size + ICMP_HDR_SIZE + IP_HDR_MAX_SIZE);
	if (config->ping.received_packet_buffer)
		config->ping.sent_packet_buffer = malloc(config->params.size + ICMP_HDR_SIZE);
	if (!config->ping.received_packet_buffer || !config->ping.sent_packet_buffer)
	{
		err = errno;
		free_resources(config);
		exit_with_message(EXIT_FAILURE, "%s: malloc\n", strerror(err));
	}
}

static uint8_t get_byte_from_char(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else
		return c - 'A' + 10;
}

static void pad_packet(void *buffer, char *pattern, uint8_t size)
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

static void fill_received_packet_buffer(struct s_config *config)
{
	if (config->params.pattern)
		pad_packet(config->ping.sent_packet_buffer + 8, config->params.pattern, config->params.size);
	else
		memset(config->ping.sent_packet_buffer + 8, 0, config->params.size);
}

static void configurate_socket_failure_w_strerror(struct s_program_param *params, char *err_msg)
{
	int err;

	err = errno;
	close(params->socketfd);
	exit_with_message(EXIT_FAILURE, err_msg, strerror(err));
}

static void configurate_timestamp_option(struct s_program_param *params)
{
	uint8_t option[4];

	option[0] = IPHDR_TSOPT_TYPE_CODE; // Type = TS; 0x44
	option[1] = IP_HDR_MAX_SIZE; // Length (Max- 40 bytes)
	option[2] = IP_TIMESTAMP_OPTION_OFFSET; // 5
	option[3] = params->iptimestamp; // 0 or 1, meaning TS or TS and ADDR
    if (setsockopt(params->socketfd, IPPROTO_IP, IP_OPTIONS, &option, sizeof(option)) < 0)
		configurate_socket_failure_w_strerror(params, "%s: setsockopt(TIMESTAMP-OPTIONS)");
}

static void configurate_socket(struct s_program_param *params)
{
	int aux;
	int err;

	aux = 1;
	params->socketfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (errno == EPERM || errno == EACCES || errno == EPROTONOSUPPORT)
		exit_with_message(EXIT_FAILURE, "Lacking privilege for icmp socket.");
	else if (params->socketfd < 0)
		exit_with_message(EXIT_FAILURE, "%s", strerror (errno));
	if(setsockopt(params->socketfd, SOL_SOCKET, SO_BROADCAST, &aux, sizeof(aux)) < 0)
		configurate_socket_failure_w_strerror(params, "%s: setsockopt(BROADCAST)");
	if(params->flags & FTP_BYPASS_ROUTING_TABLE && setsockopt(params->socketfd, SOL_SOCKET, SO_DONTROUTE, &aux, sizeof(aux)) < 0)
		configurate_socket_failure_w_strerror(params, "%s: setsockopt(DONTROUTE)");
	if(params->ttl > 0 && setsockopt(params->socketfd, IPPROTO_IP, IP_TTL, (void *) &(params->ttl), sizeof(params->ttl)) < 0)
		configurate_socket_failure_w_strerror(params, "%s: setsockopt(TTL)");
	if(params->tos > 0 && setsockopt(params->socketfd, IPPROTO_IP, IP_TOS, (void *) &(params->tos), sizeof(params->tos)) < 0)
		configurate_socket_failure_w_strerror(params, "%s: setsockopt(TOS)");
	if (params->flags & FTP_IP_TIMESTAMP)
		configurate_timestamp_option(params);
}

static void set_ping_shared_configuration(struct s_ping *ping)
{
	ping->sent_icmp_hdr = (struct s_icmp_header *) ping->sent_packet_buffer;
	ping->ip_hdr = (struct s_ip_header *) ping->received_packet_buffer;
	ping->sent_icmp_hdr->type = 8;
	ping->sent_icmp_hdr->code = 0;
	ping->sent_icmp_hdr->identifier = getpid();
}

void prepare_pinging(struct s_config *config)
{
	configurate_socket(&config->params);
	allocate_packet_buffers(config);
	fill_received_packet_buffer(config);
	set_ping_shared_configuration(&config->ping);
}

