#ifndef FT_PING_MAIN_H

# define FT_PING_MAIN_H

#include "ft_ping.h"

void set_defaults(struct s_program_param *params);
void configurate_socket(struct s_config *config);
void allocate_packet_buffers(struct s_config *config);
void fill_received_packet_buffer(struct s_config *config);
void parse(int argc, char **argv, struct s_program_param *params, struct s_options *opts);
int	ping(struct s_config *config);
void prepare_pinging(struct s_config *config);
void set_signal();
int is_address(char ***args, char *str, struct s_options *opts);

#endif
