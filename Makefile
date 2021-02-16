GREEN = \033[38;5;40m
RESET = \033[0m

NAME = ft_traceroute
CC = gcc
FLAGS = -Wall -Wextra -Werror
RM = rm -rf

SRCS = traceroute.c args.c annexes.c
OBJS = $(SRCS:%.c=%.o)

INC = traceroute.h

all: $(NAME)
	echo "$(GREEN)DONE ✔$(RESET)"

$(NAME): $(OBJS) $(INC)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

clean:
		$(RM) $(OBJS)
	
fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
.SILENT: all
