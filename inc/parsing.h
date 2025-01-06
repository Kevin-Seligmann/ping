#ifndef PARSING_H

#define PARSING_H

# include "ft_ping.h"

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
