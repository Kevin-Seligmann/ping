#ifndef FT_PING_H

# define FT_PING_H

# include <stdint.h>
# include <netdb.h>
# include <string.h>
# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <netdb.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <sys/time.h>
# include <limits.h>
# include <unistd.h>
# include <signal.h>
# include <sys/select.h>

# define PROGRAM_NAME "ft_ping"
# define TSONLY_TS 0
# define TSADDR_TS 1

# define END_CURR_PINGING 1
# define END_ALL_PINGING 2
# define PINGING 0
	
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
	struct in_addr source;
	uint32_t destination;
};

struct s_icmp_header {
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint16_t identifier;
	uint16_t sequence;
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
	int answers;
	void *sent_packet_buffer;
	void *received_packet_buffer;
	char *address;
	char *pattern;
	struct timeval starting_time;
	struct addrinfo hints;
	struct addrinfo *addr;
};

void print_meta(struct s_config *config);
void print_reply(struct s_config *config);
void exit_with_message(int exit_code, const char *exit_msg, ...);
void exit_with_help(int exit_code, const char *exit_msg, ...);
int return_with_message(int return_code, const char *exit_msg, ...);

#endif
