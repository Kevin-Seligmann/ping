VPATH = src src/parsing

# Files
OBJ = main.o read_arguments.o set_options.o configuration.o ping.o ping_printing.o exit_utils.o signal.o

# Target
NAME = ft_ping

# Project
PROJ = ft_ping

# Directories
OBJ_DIR = obj

INC_DIR = inc

OBJ_PATH = $(addprefix $(OBJ_DIR)/, $(OBJ))

DEPS = $(OBJ_PATH:.o=.d)

# Include
INCLUDES = -I./$(INC_DIR)

# Flags
FLAGS = -Wall -Wextra -Werror -g -Wno-unused

# Compiler
CC = cc

# Colors
YELLOW = "\e[33m"
GREEN = "\e[32m"
NO_COLOR = "\e[0m"

# Linking
all: $(OBJ_DIR) $(NAME)

$(NAME): $(OBJ_PATH) Makefile
	@$(CC) $(FLAGS) $(OBJ_PATH) -o $(NAME) -lm
	sudo setcap cap_net_raw=eip ./ft_ping
	@echo $(YELLOW)$(PROJ) - Creating exec:$(NO_COLOR) $(NAME)

# Compilation
$(OBJ_DIR)/%.o:%.c
	@$(CC) -MMD $(INCLUDES) $(FLAGS) -c $< -o $@
	@echo $(YELLOW)$(PROJ) - Compiling object file:$(NO_COLOR) $(notdir $@)

# Utils
-include $(DEPS)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)
	@echo $(YELLOW)$(PROJ) - Removing:$(NO_COLOR) Object and dependency files

fclean: clean
	@rm -rf $(NAME) $(NAME_B)
	@echo $(YELLOW)$(PROJ) - Removing:$(NO_COLOR) $(NAME) $(NAME_B)

re: fclean all

.PHONY: clean fclean all re $(OBJ_DIR)
