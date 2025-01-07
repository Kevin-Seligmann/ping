#ifndef FT_PING_MAIN_H

# define FT_PING_MAIN_H

#include "ft_ping.h"

void set_defaults(struct s_config *input);
void configurate_socket(struct s_config *config);
void allocate_packet_buffers(struct s_config *config);
void fill_received_packet_buffer(struct s_config *config);
void parse(int argc, char **argv, struct s_config *input);
void ping(struct s_config *config);

#endif
