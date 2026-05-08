NAME		= minishell

CC		= cc
CFLAGS		= -Wall -Wextra -Werror
INCLUDES	= -I includes -I libft

LIBFT_DIR	= libft
LIBFT		= $(LIBFT_DIR)/libft.a
TESTS_DIR	= tests

SRCS		= src/main.c \
		  src/env/env_init.c \
		  src/env/env_ops.c \
		  src/env/env_utils.c \
		  src/env/env_export.c

OBJS		= $(SRCS:.c=.o)

all: $(LIBFT) $(NAME)

test:
	$(MAKE) -C $(TESTS_DIR)

env_test:
	$(MAKE) -C $(TESTS_DIR) env_test

run_env_test:
	$(MAKE) -C $(TESTS_DIR) run_env_test

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -L $(LIBFT_DIR) -lft -lreadline -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(MAKE) -C $(TESTS_DIR) clean
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -f $(OBJS)

fclean: clean
	$(MAKE) -C $(TESTS_DIR) fclean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean env_test fclean re run_env_test test
