#include "ft_ping.h"

void print_reply(struct s_config *config)
{
	void *options;
	struct s_ip_header *ipheader = (struct s_ip_header *) config->received_packet_buffer;
	if ((ipheader->version_and_ihl & 0xF) > 5)
		options =  (char *) config->received_packet_buffer + 5 * 4;
	else
		options = NULL;
	struct s_icmp_header *icmpheader = (struct s_icmp_header *) ((char *) config->received_packet_buffer + (ipheader->version_and_ihl & 0xF) * 4);

	printf ("%d bytes from %s: icmp_seq=%u ttl=%d",
	config->size + 8, // Wrong, should use actual size
	inet_ntoa(ipheader->source),
	icmpheader->sequence,
	ipheader->ttl);
	if ((long unsigned) config->size >= sizeof(struct timeval))
	{
		printf (" time= ms"); // =%.3f ms
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

void print_meta(struct s_config *config)
{
	struct s_icmp_header *header;
	char address[INET_ADDRSTRLEN];
	struct sockaddr_in *socket_address;

	header = (struct s_icmp_header *) config->sent_packet_buffer;
	socket_address = (struct sockaddr_in *)config->addr->ai_addr;
	inet_ntop(AF_INET, &(socket_address->sin_addr), address, INET_ADDRSTRLEN);
	printf ("PING %s (%s): %u data bytes", config->address, address, config->size);
	if (config->flags.verbose)
		printf (", id 0x%04x = %u", header->identifier, header->identifier);
	printf ("\n");
}

// void print_result(struct s_config *config)
// {
// 	printf("No implementado!\n");
// }

// // Debugging tools

// void print_icmp_header(struct s_icmp_header *icmp_hdr, int size)
// {
//     printf("ICMP Header. ");
//     printf(" Type: %u", icmp_hdr->type);
//     printf(" Code: %u", icmp_hdr->code);
//     printf(" Checksum: 0x%04x", icmp_hdr->checksum);
//     printf(" Identifier: 0x%04x", icmp_hdr->identifier);
//     printf(" Sequence: 0x%04x\n", icmp_hdr->sequence);
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
