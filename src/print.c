#include "ft_ping_print.h"

void usage(int exit_code){
printf('\
Usage: ./ft_ping [OPTION...] HOST ...\n\
Send ICMP ECHO_REQUEST packets to network hosts.\n\
\n\
  -n, --numeric              do not resolve host addresses\n\
  -r, --ignore-routing       send directly to a host on an attached network\n\
      --ttl=N                specify N as time-to-live\n\
  -T, --tos=NUM              set type of service (TOS) to NUM\n\
  -v, --verbose              verbose output\n\
  -w, --timeout=N            stop after N seconds\n\
  -W, --linger=N             number of seconds to wait for response\n\
  -f, --flood                flood ping (root only)\n\
      --ip-timestamp=FLAG    IP timestamp of type FLAG, which is one of\n\
                             "tsonly" and "tsaddr"\n\
  -l, --preload=NUMBER       send NUMBER packets as fast as possible before\n\
                             falling into normal mode of behavior (root only)\n\
  -p, --pattern=PATTERN      fill ICMP packet with given pattern (hex)\n\
  -s, --size=NUMBER          send NUMBER data octets\n\
\n\
  -?, -h, --help             give this help list\n\
\n\
Mandatory or optional arguments to long options are also mandatory or optional\n\
for any corresponding short options.\n\
\n\
School project made by Kevin Seligmann for 42. Based on ineutils-2.0\'s ping.\n\
');
	exit(exit_code);
}

// void print_reply(struct s_config *config, struct s_packet *packet){
// 	char address[INET_ADDRSTRLEN];
// 	char hostname[NI_MAXHOST];
// 	struct timeval received_timestamp;
// 	struct sockaddr_in source_address;
// 	size_t received_us;
// 	size_t sent_us;

// 	gettimeofday(&received_timestamp, 0);
// 	source_address.sin_family = AF_INET;
// 	source_address.sin_port = 0;
// 	source_address.sin_addr.s_addr = packet->ip_header.source;
// 	inet_ntop(AF_INET, (void *) &(packet->ip_header.source), address, INET_ADDRSTRLEN);
//     getnameinfo((struct sockaddr *)&source_address, sizeof(source_address), hostname, NI_MAXHOST, 0, 0, 0);

// 	printf("%u bytes from ", ntohs(packet->ip_header.length) - 20);
// 	if (config->flags.numericOutput)
// 		printf("%s: icmp_seq=%u ", address, packet->icmp_header.sequence);
// 	else
// 		printf("%s (%s): icmp_seq=%u ", hostname, address, packet->icmp_header.sequence);
// 	if (config->flags.verbose)
// 		printf("ident=%u ", packet->icmp_header.identifier);
// 	if (config->size >= (int) sizeof(struct timeval))
// 	{
// 		sent_us = ((struct timeval *) packet->data)->tv_sec * 1000000 + ((struct timeval *) packet->data)->tv_usec;
// 		received_us = received_timestamp.tv_sec * 1000000 + received_timestamp.tv_usec;
// 		printf("ttl=%u time=%g ms\n", packet->ip_header.ttl, (received_us - sent_us) / 1000.);
// 	} else
// 	{
// 		printf("ttl=%u\n", packet->ip_header.ttl);
// 	}
// }

char *get_ai_family(int family)
{
	switch (family)
	{
		case AF_INET: return "AF_INET";
		case AF_UNSPEC: return "AF_UNSPEC";
		case AF_INET6: return "AF_INET6";
		default: return "0";
	}
}

void print_verbose_meta(struct s_config *config){
	struct addrinfo *addresses;

	printf("ping: sock4.fd: %d (socktype: SOCK_RAW), sock6.fd: -1 (socktype: 0), hints.ai_family: %s\n\n", config->socketfd, get_ai_family(config->hints.ai_family));
	addresses = config->addr;

	while (addresses)
	{
		if (addresses->ai_canonname)
			printf("ai->ai_family: %s, ai->ai_canonname: '%s'\n", get_ai_family(addresses->ai_family), addresses->ai_canonname);
		else 
			printf("ai->ai_family: %s, ai->ai_canonname: ''\n", get_ai_family(addresses->ai_family));
		addresses = addresses->ai_next;
	}
}

void print_meta(struct s_config *config)
{
	char address[INET_ADDRSTRLEN];
	struct sockaddr_in *socket_address;
	
	socket_address = (struct sockaddr_in *)config->addr->ai_addr;
	inet_ntop(AF_INET, &(socket_address->sin_addr), address, INET_ADDRSTRLEN);
	if (config->flags.verbose)
		print_verbose_meta(config);
	//printf("PING %s (%s) %u(%u) bytes of data.\n", config->destination, address, config->size, config->size + 8 + 20);
}

void print_result(struct s_config *config)
{
	printf("No implementado!\n");
}

// Debugging tools

void print_icmp_header(struct s_icmp_header *icmp_hdr, int size)
{
    printf("ICMP Header. ");
    printf(" Type: %u", icmp_hdr->type);
    printf(" Code: %u", icmp_hdr->code);
    printf(" Checksum: 0x%04x", icmp_hdr->checksum);
    printf(" Identifier: 0x%04x", icmp_hdr->identifier);
    printf(" Sequence: 0x%04x\n", icmp_hdr->sequence);
	printf("PAYLOAD: ");

	char *ptr;

	ptr = (char *) icmp_hdr + 8;
	while (size > 0) {
		unsigned char byte = *ptr;
		for (int i = 7; i >= 0; i--) { 
			printf("%d", (byte >> i) & 1);
		}
		printf(" ");
		ptr++;
		size--;
	}
}

// void print_packet(struct s_packet *pkt)
// {
//     printf("IP Header. ");
//     printf("Version: %u", pkt->ip_header.version_and_ihl >> 4);
//     printf(" IHL: %u", pkt->ip_header.version_and_ihl & 0x0F); 
//     printf(" TOS: 0x%02x", pkt->ip_header.tos); 
//     printf(" Length: %u", ntohs(pkt->ip_header.length));
//     printf(" ID: 0x%04x", ntohs(pkt->ip_header.identification));  
//     printf(" Flags: 0x%02x", pkt->ip_header.flags_and_offset >> 13);
//     printf(" Offset: 0x%03x", pkt->ip_header.flags_and_offset & 0x1FFF);
//     printf(" TTL: %u", pkt->ip_header.ttl);
//     printf(" Protocol: %u", pkt->ip_header.protocol);
//     printf(" Checksum: 0x%04x", ntohs(pkt->ip_header.checksum)); 
//     printf(" Source IP: 0x%08x", ntohl(pkt->ip_header.source)); 
//     printf(" Destination IP: 0x%08x\n", ntohl(pkt->ip_header.destination));  

//     print_icmp_header(&pkt->icmp_header, ntohs(pkt->ip_header.length) - 8 - 20);
// }
