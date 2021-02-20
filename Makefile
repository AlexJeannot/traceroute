# VARIABLES
GREEN = \033[38;5;40m
RESET = \033[0m
NAME = ft_traceroute

# COMPILATION
CC = gcc
FLAGS = -Wall -Wextra -Werror
RM = rm -rf


# DIRECTORIES
DSRCS	= ./srcs/
DOBJS	= ./comp/

# SOURCES
SRCS =		annexes.c		\
			args.c			\
			control.c		\
			display.c		\
			error.c			\
			exit.c			\
			ipv4.c			\
			ipv6.c			\
			manage_io.c		\
			network.c		\
			node.c			\
			options.c		\
			settings.c		\
			signal.c		\
			traceroute.c	

OBJS = $(SRCS:%.c=$(DOBJS)%.o)

HEADERS = ./inc/traceroute.h

all: $(NAME)
	echo "$(GREEN)DONE ✔$(RESET)"

$(NAME): $(OBJS) $(HEADERS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

$(DOBJS):
	mkdir $(DOBJS)

$(DOBJS)%.o: $(DSRCS)%.c
	$(CC) -c $< -o $@


# CLEAR
clean:
		$(RM) $(OBJS)
	
fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
.SILENT: all
