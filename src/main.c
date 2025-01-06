#include "ft_ping.h"

int main(int argc, char **argv)
{
	static struct s_config input;

	parse(argc, argv, &input);
	
	ping(&input);

	return 0;
}
