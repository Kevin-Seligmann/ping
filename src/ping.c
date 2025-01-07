#include "ft_ping.h"

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

uint8_t get_byte_from_char(char c){
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else
		return c - 'A' + 10;
}

void pad_packet(void *buffer, char *pattern, uint8_t size){
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

void build_icmp_packet(struct s_config *config, struct s_packet **packet){
	*packet = calloc(config->size + 8 + 20, 1);
	(*packet)->icmp_header.type = 8;
	(*packet)->icmp_header.code = 0;
	(*packet)->icmp_header.identifier = getpid();
	(*packet)->icmp_header.sequence = config->sequence ++;
	
	if (config->size >= (int) sizeof(struct timeval))
	{
    	gettimeofday((struct timeval *)&(*packet)->data, NULL);
		if (config->pattern)
			pad_packet((void *)((char *)&(*packet)->data) + sizeof(struct timeval), config->pattern, config->size - sizeof(struct timeval));
	} else if (config->pattern)
	{
		pad_packet((void *)((char *)&(*packet)->data), config->pattern, config->size);
	}

	(*packet)->icmp_header.checksum = checksum((void *) &((*packet)->icmp_header), 8 + config->size);
}

void send_icmp_packet(struct s_config *config, struct s_packet *packet){
	ssize_t bytesSent = sendto(config->socketfd, &(packet->icmp_header), config->size + 8, 0, config->addr->ai_addr, config->addr->ai_addrlen);
//	printf("SENT %lu: ", bytesSent);
//	print_icmp_header(&(packet->icmp_header), config->size);
}

void send_echo_request(struct s_config *config){
	struct s_packet *packet;

	packet = NULL;
	build_icmp_packet(config, &packet);
	send_icmp_packet(config, packet);
}

void listen_echo_reply(struct s_config *config){
	struct s_packet *packet;
	ssize_t received;

	packet = calloc(config->size + 28, 1);
	while (1)
	{
		received = recvfrom(config->socketfd, packet, config->size + 28 + 8, 0, config->addr->ai_addr, &config->addr->ai_addrlen);
		if (packet->icmp_header.type == 0)
		{
			print_reply(config, packet);
			break ;
		}
		else 
		{
			printf("Received else: ");
			print_packet(packet);
		}
	}
//	printf("RECEIVED %lu: ", received);
//	print_packet(packet);
//	printf("\n");
}

void set_ttl(struct s_config *config)
{
	setsockopt(config->socketfd, IPPROTO_IP, IP_TTL, (void *) &(config->ttl), sizeof(config->ttl));
}

void send_preload(struct s_config *config)
{
	int count;

	count = 0;
	while (count < config->preload)
	{
		send_echo_request(config);
		count ++;
	}
	while (count > 0)
	{
		listen_echo_reply(config);
		count --;
	}
	sleep(1);
}

void ping(struct s_config *config)
{
	if (config->ttl)
		set_ttl(config);

	getaddrinfo(config->address, 0, &(config->hints), &(config->addr));
	print_meta(config);
	if (config->flags.preload)
		send_preload(config);
	while (1){
		send_echo_request(config);

		listen_echo_reply(config);

		sleep(1);
	}
	
//	print_result(config);
}
