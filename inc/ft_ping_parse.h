#ifndef FT_PING_PARSE

#define FT_PING_PARSE

# include "ft_ping.h"

# define TIMEOUT_MAX INT_MAX
# define TIMEOUT_MIN 1

# define LINGER_MAX INT_MAX
# define LINGER_MIN 1

# define PACKET_SIZE_MAX 60000
# define PACKET_SIZE_MIN 0

# define TTL_MAX 255
# define TTL_MIN 1

# define TOS_MAX 255
# define TOS_MIN 1

# define INTERVAL_MAX 200000
# define INTERVAL_MIN 0

# define COUNT_MAX 0 // No max
# define COUNT_MIN 0

# define PRELOAD_MAX INT_MAX

# define USAGE_FAILURE 64

unsigned long get_numeric(char *arg);

void	set_defaults(struct s_program_param *params);
void	get_preload_option(char *arg, struct s_program_param *params);
void	get_timeout_option(char *arg, struct s_program_param *params);
void	get_linger_option(char *arg, struct s_program_param *params);
void	get_pattern_option(char *arg, struct s_program_param *params);
void	get_size_option(char *arg, struct s_program_param *params);
void	get_tos_option(char *arg, struct s_program_param *params);
void	get_ttl_option(char *arg, struct s_program_param *params);
void	get_iptimestamp_option(char *arg, struct s_program_param *params);
void	get_count_option(char *arg, struct s_program_param *params);
void 	get_interval_option(char *arg, struct s_program_param *params);
void	usage(int exit_code);

#endif
