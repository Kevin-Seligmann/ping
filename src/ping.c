#include "ft_ping.h"

extern volatile sig_atomic_t status;

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

int validate_ping(struct s_ping *ping)
{
	if (ping->received_bytes < 0)
		return 0;
	ping->ip_hdr = (struct s_ip_header *) ping->received_packet_buffer;
	ping->recv_icmp_hdr = (struct s_icmp_header *) (ping->received_packet_buffer + (ping->ip_hdr->version_and_ihl & 0xF) * WORD_SIZE_ON_BYTES);
	if (ping->recv_icmp_hdr->type != 0)
		return 0;
	// "packet too short (%d bytes) from %s\n"
	// "invalid checksum"
	// Identity check
	return 1;
}

void listen_echo_reply(struct s_program_param *params, struct s_ping *ping)
{
	struct timeval timestamp;

	ping->received_bytes = recvfrom(
		params->socketfd, 
		ping->received_packet_buffer, 
		params->size + ICMP_HDR_SIZE + IP_HDR_MAX_SIZE, 
		0, 
		ping->addr->ai_addr, 
		&ping->addr->ai_addrlen);
	gettimeofday(&timestamp, 0);
	if (!validate_ping(ping))
	{
		listen_echo_reply(params, ping);
		return ;
	}
	ping->answer_count ++;
	ping->time.received_timestamp = timestamp;
	ping->time.asnwer_time = get_time_diff_us(* ((struct timeval *) ((char *) ping->recv_icmp_hdr + ICMP_HDR_SIZE)), ping->time.received_timestamp) / 1000.;
	ping->time.max = fmax(ping->time.max, ping->time.asnwer_time);
	if (ping->time.min)
		ping->time.min = fmin(ping->time.min, ping->time.asnwer_time);
	else
		ping->time.min = ping->time.asnwer_time;
	ping->time.total += ping->time.asnwer_time;
	ping->time.total_squared += ping->time.asnwer_time * ping->time.asnwer_time;
	print_reply(params, ping);
	sleep(1);
}

// int send_preload(struct s_config *config)
// {
// 	int count;

// 	count = 0;
// 	while (count < config->preload)
// 	{
// 		if (send_echo_request(config) == END_ALL_PINGING)
// 			return (END_ALL_PINGING);
// 		count ++;
// 	}
// 	return 0;
// }

// int do_select(struct s_config *config)
// {
// 	struct timeval linger;
// 	fd_set fdset;

// 	FD_ZERO(&fdset);
// 	FD_SET(config->socketfd, &fdset);

// 	linger.tv_sec = 0;
// 	linger.tv_usec = config->linger;
// 	return select(config->socketfd + 1, &fdset, 0, 0, &linger);
// }

// int check_timeout(struct s_config *config)
// {
// 	struct timeval timestamp;
// 	ssize_t us_since_start;

// 	if (config->flags.timeout == 0)
// 		return 0;
// 	gettimeofday(&timestamp, 0);
// 	us_since_start = (timestamp.tv_sec - config->starting_time.tv_sec) * 1000000 + timestamp.tv_usec - config->starting_time.tv_usec;
// 	return (us_since_start >= config->timeout * 1000000);
// }

static void configure_ping(struct s_config *config, struct s_program_param *params, struct s_ping *ping)
{
	ping->answer_count = 0;
	ping->sequence = 0;
	ping->time.max = 0;
	ping->time.min = 0;
	ping->time.total = 0;
	ping->time.total_squared = 0;
	if (getaddrinfo(ping->destination, 0, &(params->hints), &(ping->addr)))
		exit_wmsg_and_free(config, EXIT_FAILURE, "unknown host");
}

static void build_icmp_header(struct s_program_param *params, struct s_ping *ping)
{
	ping->sent_icmp_hdr->checksum = 0;
	ping->sent_icmp_hdr->sequence = ping->sequence;
	if (params->size >= (int) sizeof(struct timeval))
	{
		memset(ping->sent_packet_buffer + ICMP_HDR_SIZE, 0, sizeof(struct timeval));
    	gettimeofday((struct timeval *) (ping->sent_packet_buffer + ICMP_HDR_SIZE), NULL);
	}
	ping->sent_icmp_hdr->checksum = checksum(ping->sent_packet_buffer, params->size + ICMP_HDR_SIZE);
}


static void send_echo_request(struct s_config *config, struct s_program_param *params, struct s_ping *ping)
{
	build_icmp_header(params, ping);
	ping->sent_bytes = sendto(
		params->socketfd, 
		ping->sent_packet_buffer, 
		params->size + sizeof(*ping->sent_icmp_hdr),
		0, ping->addr->ai_addr, 
		ping->addr->ai_addrlen
	);
	if (ping->sent_bytes == -1)
		exit_wmsg_and_free(config, EXIT_FAILURE, "%s: sending packet", strerror(errno));
	config->ping.sequence ++;
	if (ping->sent_bytes < params->size + ICMP_HDR_SIZE)
	{
 		printf ("ft_ping: wrote %s %d chars, ret=%lu\n", ping->destination, params->size + ICMP_HDR_SIZE, ping->sent_bytes);
	}
}

void do_ping_loop(struct s_config *config)
{
	// if (check_timeout(config))
	// 	return exit_pinging(config, 0);
	// select_reply = do_select(config);
	// if (select_reply > 0)
	listen_echo_reply(&config->params, &config->ping);
	// else if (select_reply < 0)
	// {
	// 	fprintf(stderr, "ft_ping: select failed\n");
	// 	return exit_pinging(config, END_ALL_PINGING);
	// }
	// else 
	// {
	if (status == PINGING)
		send_echo_request(config, &config->params, &config->ping);
}

void ping(struct s_config *config)
{
	configure_ping(config, &config->params, &config->ping);
	print_meta(&config->params, &config->ping);
	// send_preload(config);
	send_echo_request(config, &config->params, &config->ping);
 	while (status == PINGING)
		do_ping_loop(config);
	print_result(&config->params, &config->ping);
	freeaddrinfo(config->ping.addr);
}
