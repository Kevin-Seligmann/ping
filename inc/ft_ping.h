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
# include <math.h>

# define PROGRAM_NAME "ft_ping"

// Program flow
# define LISTEN_ONCE 1
# define END_ALL_PINGING 2
# define END_CURR_PINGING 3
# define PINGING 0

// Options
# define OPTIONS_COUNT 18 - 2

										// Help -? --help 						- IMPLEMENTED
										// Version -V --version 				- IMPLEMENTED
										// Preload -l --preload=NUMBER 			- IMPLEMENTED
 										// Timeout -w --timenout=N				- IMPLEMENTED
										// Linger -W --linger=N					- IMPLEMENTED
										// Pattern -p --pattern=PATTERN			- IMPLEMENTED
										// Packet size  -s --size=NUMBER		- IMPLEMENTED
										// Count -c --count=NUMBER				- IMPLEMENTED
										// -T --tos=N							- IMPLEMENETD
										// --ttl=N								- IMPLEMENTED
# define FTP_VERBOSE 0x1				// -v --verbose							- IMPLEMENTED
# define FTP_FLOOD 0x2					// -f --flood							- IMPLEMENETD
# define FTP_BYPASS_ROUTING_TABLE 0x8	// -r --ignore-routing					- IMPLEMENTED
# define FTP_QUIET 0x20					// -q --quiet							- IMPLEMENTED
# define FTP_DEBUG 0x40					// -d --debug							- IMPLEMENTED
# define FTP_INTERVAL 0x80				// Interval -i --interval=NUMBER 		- IMPLEMENTED

// To be fair it's a little out of scope. I dont mind managing the different ip header sizes and options.
// But there are more interesting projects than figuring out when and what to print about the timestamps and addresses.
// The former is already implemented anyways, I will leave it commented.
// # define FTP_IP_TIMESTAMP 0x10			// --ip-timestamp=FLAG					- NO
// # define FTP_NUMERIC 0x4				// -n --numeric							- NO (Useless w.o timestamp)

// Defaults
# define DEF_LINGER 10
# define DEF_SIZE 56
# define DEF_INTERVAL 1

// Ip timestamp options
// # define TSONLY_TS 0
// # define TSADDR_TS 1

// # define IPHDR_TSOPT_TYPE_CODE 0x44
// # define IP_TIMESTAMP_OPTION_OFFSET 5

// Header sizes
# define ICMP_HDR_SIZE 8
# define IP_HDR_MAX_SIZE 40 // 20 from header, 20 from options.
# define WORD_SIZE_ON_BYTES 4

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
	struct timeval select_timeout;
	struct timeval received_timestamp;
	struct timeval last_sent;
	struct timeval present;
	ssize_t usec_to_echo;
	double effective_interval;
	double answer_time;
	double min;
	double max;
	double total;
	double total_squared;
};

// Information about a specific ping routine (of each destination passed as argument)
struct s_ping {
	struct s_timing time;
	struct addrinfo *addr;
	struct s_icmp_header *sent_icmp_hdr;
	struct s_icmp_header *recv_icmp_hdr;
	struct s_ip_header *ip_hdr;
	size_t sequence;
	size_t answer_count;
	long received_bytes;
	long sent_bytes;
	void *received_packet_buffer;
	void *sent_packet_buffer;
	char *destination;
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
	double interval;
	int ttl;
	int tos;
	size_t count;
	int iptimestamp;
	int destinations;
	char *pattern;
};

// Struct holding options utils
struct s_options {
	int char_name;
	char *str_name;
	void (*action)(void);
	void (*argument_getter)(char *, struct s_program_param *);
	int flag_value;
};

struct s_config {
	struct s_program_param params;
	struct s_ping ping;
	struct s_options *options;
};

void print_meta(struct s_program_param *params, struct s_ping *ping);
void print_reply(struct s_program_param *params, struct s_ping *ping);
void print_result(struct s_program_param *params, struct s_ping *ping);
void exit_with_message(int exit_code, const char *exit_msg, ...);
void exit_with_help(int exit_code, const char *exit_msg, ...);
int return_with_message(int return_code, const char *exit_msg, ...);
void free_resources(struct s_config *config);
void exit_wmsg_and_free(struct s_config *config, int exit_code, char *exit_msg, ...);
void set_options(struct s_options **options);

#endif
