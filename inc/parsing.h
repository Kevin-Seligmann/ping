#ifndef PARSING_H

#define PARSING_H

# include "ft_ping.h"
# include <limits.h>

# define TIMEOUT_MAX INT_MAX
# define TIMEOUT_MIN 0

# define LINGER_MAX INT_MAX
# define LINGER_MIN 0

# define SIZE_MAX 60000
# define SIZE_MIN 0

# define TTL_MAX 255
# define TTL_MIN 0

# define TOS_MAX 255
# define TOS_MIN 1

# define PRELOAD_MAX INT_MAX


unsigned long get_numeric(char *arg);

void	set_defaults(struct s_config *input);
void	get_preload_option(char *arg, struct s_config *config);
void	get_timeout_option(char *arg, struct s_config *config);
void	get_linger_option(char *arg, struct s_config *config);
void	get_pattern_option(char *arg, struct s_config *config);
void	get_size_option(char *arg, struct s_config *config);
void	get_tos_option(char *arg, struct s_config *config);
void	get_ttl_option(char *arg, struct s_config *config);
void	get_iptimestamp_option(char *arg, struct s_config *config);

#endif
