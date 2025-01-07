#ifndef FT_PING_H

# define FT_PING_H

# include <stdio.h>
# include <netdb.h>
# include <string.h>
# include <errno.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <sys/time.h>
# include <stdlib.h>
# include <unistd.h>

# define TSONLY_TS 1
# define TSADDR_TS 2
	
struct s_flags {
	int verbose; // -v --verbose 
	int help; // -? --help
	int floodping; // -f --flood
	int preload; // -l --preload=NUMBER 
	int numericOutput; // -n --numeric
	int timeout; // -w --timenout=N
	int linger; // -W --linger=N
	int pattern; // -p --pattern=PATTERN
	int bypassNormalRoutingTables; // -r --ignore-routing
	int packetsize; // -s --size=NUMBER
	int tos; // -T --tos=NUM 
	int ttl; // --ttl=N
	int iptimestamp; // --ip-timestamp=FLAG
};


struct s_ip_header {
	uint8_t version_and_ihl;
	uint8_t tos;
	uint16_t length;
	uint16_t identification;
	uint16_t flags_and_offset;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t checksum;
	uint32_t source;
	uint32_t destination;
}; 

struct s_icmp_header {
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint16_t identifier;
	uint16_t sequence;
};

struct s_packet {
	struct s_ip_header ip_header;
	struct s_icmp_header icmp_header;
	uint8_t data[];
};

struct s_config {
	struct s_flags flags;
	int preload;
	int timeout;
	int size;
	int interval;
	int ttl;
	int tos;
	int sequence;
	int linger;
	int socketfd;
	int address_count;
	int iptimestamp;
	char *pattern;
	struct addrinfo hints;
	struct addrinfo *addr;
};

void parse(int argc, char **argv, struct s_config *input);

void ping(struct s_config *config);

void print_reply(struct s_config *config, struct s_packet *packet);

void print_meta(struct s_config *config);

void print_result(struct s_config *config);

void print_icmp_header(struct s_icmp_header *icmp_hdr, int size);

void print_packet(struct s_packet *pkt);

void usage(int exit_code);

#endif
