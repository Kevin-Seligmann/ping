#include "ft_ping.h"

volatile sig_atomic_t status;

uint16_t checksum(void *header, size_t size){
    uint32_t sum = 0;
    uint16_t *ptr = (uint16_t *)header;

    while (size > 1) {
        sum += *ptr;
		ptr ++;
        size -= 2;
    }

    if (size == 1) {
        uint16_t odd = 0;
        *(uint8_t *)&odd = *(uint8_t *)ptr;
        sum += odd;
    }

	while (sum >> 16) {
		sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}

int send_echo_request(struct s_config *config)
{
	struct s_icmp_header *header;
	ssize_t bytes_sent;

	header = (struct s_icmp_header *) config->sent_packet_buffer;
	header->type = 8;
	header->code = 0;
	header->checksum = 0;
	header->identifier = getpid();
	header->sequence = config->sequence;
	if (config->size >= (int) sizeof(struct timeval))
	{
    	gettimeofday((struct timeval *) (config->sent_packet_buffer + sizeof(*header)), NULL);
	}
	header->checksum = checksum(config->sent_packet_buffer, config->size + sizeof(*header));
	bytes_sent = sendto(config->socketfd, config->sent_packet_buffer, config->size + sizeof(*header), 0, config->addr->ai_addr, config->addr->ai_addrlen);
	if (bytes_sent == -1)
	{
		fprintf(stderr, "ft_ping: %s: sending packet\n", strerror(errno));
		return END_ALL_PINGING;
	}
	config->sequence ++;
	if ((long unsigned) bytes_sent < config->size + sizeof(*header)) // Plus size of ip header?
	{
		printf ("ping: wrote %s %lu chars, ret=%lu\n", config->address, config->size + sizeof(*header), bytes_sent);
	}
	return 0;
}


void listen_echo_reply(struct s_config *config){
	struct s_packet *packet;
	ssize_t received;

	packet = calloc(config->size + 28, 1);
	while (1)
	{
		received = recvfrom(config->socketfd, packet, config->size + 28 + 8, 0, config->addr->ai_addr, &config->addr->ai_addrlen);
	}
}

int send_preload(struct s_config *config)
{
	int count;

	count = 0;
	while (count < config->preload)
	{
		if (send_echo_request(config) == END_ALL_PINGING)
			return (END_ALL_PINGING);
		count ++;
	}
	return 0;
}

int exit_pinging(struct s_config *config, int code)
{
	freeaddrinfo(config->addr);
	return code;
}

int do_select(struct s_config *config)
{
	struct timeval linger;
	fd_set fdset;

	FD_ZERO(&fdset);
	FD_SET(config->socketfd, &fdset);

	linger.tv_sec = 0;
	linger.tv_usec = config->linger;
	return select(config->socketfd + 1, &fdset, 0, 0, &linger);
}

int check_timeout(struct s_config *config)
{
	struct timeval timestamp;
	ssize_t us_since_start;

	if (config->flags.timeout == 0)
		return 0;
	gettimeofday(&timestamp, 0);
	us_since_start = (timestamp.tv_sec - config->starting_time.tv_sec) * 1000000 + timestamp.tv_usec - config->starting_time.tv_usec;
	return (us_since_start >= config->timeout * 1000000);
}

int ping(struct s_config *config)
{
	int select_reply;

	status = PINGING;
	if (getaddrinfo(config->address, 0, &(config->hints), &(config->addr)))
	{
		fprintf(stderr, "ft_ping: unknown host\n");
		return (END_ALL_PINGING);
	}
	print_meta(config);
	if (send_preload(config) == END_ALL_PINGING)
		return exit_pinging(config, END_ALL_PINGING);
	if (send_echo_request(config) == END_ALL_PINGING)
		return exit_pinging(config, END_ALL_PINGING);
	while (status == PINGING)
	{
		if (check_timeout(config))
			return exit_pinging(config, 0);
		select_reply = do_select(config);
		if (select_reply > 0)
			listen_echo_reply(config);
		else if (select_reply < 0)
		{
			fprintf(stderr, "ft_ping: select failed\n");
			return exit_pinging(config, END_ALL_PINGING);
		}
		else 
		{
			if (send_echo_request(config) == END_ALL_PINGING)
				return exit_pinging(config, END_ALL_PINGING);
			usleep(config->interval);
		}
	}
//	print_result(config);
	return exit_pinging(config, 0);
}
