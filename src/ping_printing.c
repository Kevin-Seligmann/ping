#include "ft_ping.h"

void print_reply(struct s_program_param *params, struct s_ping *ping)
{
	unsigned long time;

	// void *options;
	// if ((ipheader->version_and_ihl & 0xF) > 5)
	// 	options =  (char *) config->received_packet_buffer + 5 * 4;
	// else
	//	options = NULL;
	printf ("%lu bytes from %s: icmp_seq=%u ttl=%d",
		ping->received_bytes - (ping->ip_hdr->version_and_ihl & 0xf) * WORD_SIZE_ON_BYTES,
		inet_ntoa(ping->ip_hdr->source),
		ping->recv_icmp_hdr->sequence,
		ping->ip_hdr->ttl
	);
	if ((long unsigned) params->size >= sizeof(struct timeval))
	{
		printf (" time=%.3f ms", ping->time.answer_time);
	}
	printf ("\n");
}

// char *get_ai_family(int family)
// {
// 	switch (family)
// 	{
// 		case AF_INET: return "AF_INET";
// 		case AF_UNSPEC: return "AF_UNSPEC";
// 		case AF_INET6: return "AF_INET6";
// 		default: return "0";
// 	}
// }

void print_meta(struct s_program_param *params, struct s_ping *ping)
{
	char address[INET_ADDRSTRLEN];
	struct sockaddr_in *socket_address;

	socket_address = (struct sockaddr_in *) ping->addr->ai_addr;
	inet_ntop(AF_INET, &(socket_address->sin_addr), address, INET_ADDRSTRLEN);
	printf ("PING %s (%s): %u data bytes", ping->destination, address, params->size);
	if (params->flags & FTP_VERBOSE)
		printf (", id 0x%04x = %u", ping->sent_icmp_hdr->identifier, ping->sent_icmp_hdr->identifier);
	printf ("\n");
}

void print_result(struct s_program_param *params, struct s_ping *ping)
{
	double average;
	double stddev;

	printf("--- %s ping statistics ---\n", ping->destination);
	printf("%u packets transmitted, ", ping->sequence);
	printf("%u packets received, ", ping->answer_count);
	if (ping->answer_count)
	{
		if (ping->answer_count > ping->sequence)
			printf ("-- somebody is printing forged packets!");
		else
			printf ("%d%% packet loss", (((ping->sequence - ping->answer_count) * 100) / ping->sequence));
	}
	printf ("\n");
	if (ping->answer_count &&params->size >=  (int) sizeof(struct timeval)) // TODO: Add duplicates count. And //    printf ("+%zu duplicates, ", duplicates);
	{
		average = ping->time.total / ping->answer_count;
		stddev = sqrt(ping->time.total_squared / ping->answer_count - average * average);
		printf ("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", 
			ping->time.min, 
			average, 
			ping->time.max, 
			stddev
		);
	}
}

// // Debugging tools

// void print_icmp_header(struct s_icmp_header *sent_icmp_hdr, int size)
// {
//     printf("ICMP Header. ");
//     printf(" Type: %u", sent_icmp_hdr->type);
//     printf(" Code: %u", sent_icmp_hdr->code);
//     printf(" Checksum: 0x%04x", sent_icmp_hdr->checksum);
//     printf(" Identifier: 0x%04x", sent_icmp_hdr->identifier);
//     printf(" Sequence: 0x%04x\n", sent_icmp_hdr->sequence);
// 	printf("PAYLOAD: ");

// 	char *ptr;

// 	ptr = (char *) icmp_hdr + 8;
// 	while (size > 0) {
// 		unsigned char byte = *ptr;
// 		for (int i = 7; i >= 0; i--) { 
// 			printf("%d", (byte >> i) & 1);
// 		}
// 		printf(" ");
// 		ptr++;
// 		size--;
// 	}
// }

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
