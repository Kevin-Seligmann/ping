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

// T2 > T1 (T2 after T1)
static unsigned long get_time_diff_us(struct timeval t1, struct timeval t2)
{
	return (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
}

static void set_successful_ping_data(struct s_ping *ping, struct timeval timestamp)
{
	ping->answer_count ++;
	ping->time.received_timestamp = timestamp;
	ping->time.answer_time = get_time_diff_us(* ((struct timeval *) ((char *) ping->recv_icmp_hdr + ICMP_HDR_SIZE)), ping->time.received_timestamp) / 1000.;
	ping->time.max = fmax(ping->time.max, ping->time.answer_time);
	if (ping->time.min)
		ping->time.min = fmin(ping->time.min, ping->time.answer_time);
	else
		ping->time.min = ping->time.answer_time;
	ping->time.total += ping->time.answer_time;
	ping->time.total_squared += ping->time.answer_time * ping->time.answer_time;
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
		return ;
	set_successful_ping_data(ping, timestamp);
	if (!(params->flags & FTP_QUIET))
		print_reply(params, ping);
}

int do_select(struct s_program_param *params, struct s_ping *ping)
{
	fd_set fdset;

	FD_ZERO(&fdset);
	FD_SET(params->socketfd, &fdset);

	if (params->flags & FTP_FLOOD)
	{
		ping->time.select_timeout.tv_sec = 0;
		ping->time.select_timeout.tv_usec = 10000;
	}
	else if (status == LISTEN_ONCE)
	{
		ping->time.select_timeout.tv_sec = params->linger;
		status = END_CURR_PINGING;
	}
	else if (ping->time.usec_to_echo > 0)
		ping->time.select_timeout.tv_usec = ping->time.usec_to_echo;
	else
		ping->time.select_timeout.tv_usec = 0;
	return select(params->socketfd + 1, &fdset, 0, 0, &ping->time.select_timeout);
}

static void configure_ping(struct s_config *config, struct s_program_param *params, struct s_ping *ping)
{
	ping->answer_count = 0;
	ping->sequence = 0;
	ping->time.max = 0;
	ping->time.min = 0;
	ping->time.total = 0;
	ping->time.total_squared = 0;
	ping->time.effective_interval = 0;
	ping->time.usec_to_echo = 0;
	ping->time.select_timeout.tv_sec = 0;
	if (getaddrinfo(ping->destination, 0, &(params->hints), &(ping->addr)))
		exit_wmsg_and_free(config, EXIT_FAILURE, "unknown host");
}

static void build_icmp_header(struct s_program_param *params, struct s_ping *ping)
{
	ping->sent_icmp_hdr->checksum = 0;
	ping->sent_icmp_hdr->sequence = ping->sequence;
	gettimeofday(&ping->time.last_sent, NULL);
	if (params->size >= (int) sizeof(struct timeval))
	{
    	*(struct timeval *) (ping->sent_packet_buffer + ICMP_HDR_SIZE) = ping->time.last_sent;
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
	if (params->flags & FTP_FLOOD && !(params->flags & FTP_QUIET))
		putchar('.');
}

int interval_passed(struct s_timing *time)
{
	size_t usec_since_last;

	usec_since_last = (time->present.tv_sec - time->last_sent.tv_sec) * 1000000 + (time->present.tv_usec - time->last_sent.tv_usec);
	time->usec_to_echo = time->effective_interval * 1000000 - usec_since_last;
	return time->usec_to_echo <= 0;
}

void check_timeout(struct s_program_param *params, struct s_ping *ping)
{
	size_t usec_since_start;

	if (!params->timeout)
		return ;
	usec_since_start = (ping->time.present.tv_sec - ping->time.starting_time.tv_sec) * 1000000 + ping->time.present.tv_usec - ping->time.starting_time.tv_usec;
	if (usec_since_start > (unsigned int) params->timeout * 1000000)
		status = END_ALL_PINGING;
}

void do_ping_loop(struct s_config *config)
{
	int select_reply;

	select_reply = do_select(&config->params, &config->ping);
	gettimeofday(&config->ping.time.present, 0);
	if (select_reply > 0)
	{
		listen_echo_reply(&config->params, &config->ping);
	}
	else if (select_reply < 0)
	{
		if (errno != EINTR)
	 		exit_wmsg_and_free(config, EXIT_FAILURE, "select failed");
	}
	else
	{
		if (status == PINGING && (config->params.flags & FTP_FLOOD || interval_passed(&config->ping.time)))
			send_echo_request(config, &config->params, &config->ping);
		if (config->params.count && config->params.count <= config->ping.sequence)
			status = LISTEN_ONCE;
	}
	check_timeout(&config->params, &config->ping);
}

void send_preload(struct s_config *config, struct s_program_param *params, struct s_ping *ping)
{
	int count;

	count = 0;
	while (count < params->preload)
	{
		send_echo_request(config, params, ping);
		count ++;
	}
}

void ping(struct s_config *config)
{
	configure_ping(config, &config->params, &config->ping);
	print_meta(&config->params, &config->ping);
	send_preload(config, &config->params, &config->ping);
	send_echo_request(config, &config->params, &config->ping);
	config->ping.time.effective_interval = config->params.interval;
 	while (status == PINGING || status == LISTEN_ONCE)
		do_ping_loop(config);
	print_result(&config->params, &config->ping);
	freeaddrinfo(config->ping.addr);
}
