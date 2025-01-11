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

// Program flow
# define END_CURR_PINGING 1
# define END_ALL_PINGING 2
# define PINGING 0


// Options
										// Help -? --help
										// Preload -l --preload=NUMBER 
 										// Timeout -w --timenout=N
										// Linger -W --linger=N
										// Pattern -p --pattern=PATTERN
										// Packet size  -s --size=NUMBER
										// -T --tos=N
										// --ttl=N
# define FTP_VERBOSE 0x1				// -v --verbose
# define FTP_FLOOD 0x2					// -f --flood
# define FTP_NUMERIC 0x4				// -n --numeric 
# define FTP_BYPASS_ROUTING_TABLE 0x8	// -r --ignore-routing
# define FTP_IP_TIMESTAMP 0x10			// --ip-timestamp=FLAG

// Ip timestamp options
# define TSONLY_TS 0
# define TSADDR_TS 1

# define IPHDR_TSOPT_TYPE_CODE 0x44
# define IP_TIMESTAMP_OPTION_OFFSET 5

// Header sizes
# define ICMP_HDR_SIZE 8
# define IP_HDR_MAX_SIZE 40 // 20 from header, 20 from options.

// Headers
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

// Ping timing information
struct s_timing {
	struct timeval starting_time;
};

// Information about a specific ping routine (of each address passed as argument)
struct s_ping {
	struct addrinfo *addr;
	int sequence;
	int answer_count;
	void *received_packet_buffer;
	void *sent_packet_buffer;
	char *address;
};

// Information about all ping routines
struct s_program_param {
	struct addrinfo hints;
	unsigned int flags;
	int linger;
	int socketfd;
	int preload;
	int timeout;
	int size;
	int interval;
	int ttl;
	int tos;
	int iptimestamp;
	int destinations;
	char *pattern;
};

struct s_config {
	struct s_program_param params;
	struct s_ping ping;
};

void print_meta(struct s_config *config);
void print_reply(struct s_config *config);
void exit_with_message(int exit_code, const char *exit_msg, ...);
void exit_with_help(int exit_code, const char *exit_msg, ...);
int return_with_message(int return_code, const char *exit_msg, ...);
void free_resources(struct s_config *config);

#endif
