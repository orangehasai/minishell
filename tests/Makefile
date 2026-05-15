NAME_ENV	= bin/env_test

CC		= cc
CFLAGS		= -Wall -Wextra -Werror
INCLUDES	= -I ../includes -I ../libft -I include

LIBFT_DIR	= ../libft
LIBFT		= $(LIBFT_DIR)/libft.a
OBJ_DIR		= obj

ENV_OBJS	= $(OBJ_DIR)/common/test_output.o \
		  $(OBJ_DIR)/env/env_test_main.o \
		  $(OBJ_DIR)/env/env_test_utils.o \
		  $(OBJ_DIR)/env/env_test_case_init.o \
		  $(OBJ_DIR)/env/env_test_case_ops.o \
		  $(OBJ_DIR)/env/env_test_case_utils.o \
		  $(OBJ_DIR)/src/env/env_init.o \
		  $(OBJ_DIR)/src/env/env_ops.o \
		  $(OBJ_DIR)/src/env/env_utils.o \
		  $(OBJ_DIR)/src/env/env_export.o

all: env_test

env_test: $(LIBFT) $(NAME_ENV)

run_env_test: env_test
	./$(NAME_ENV)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME_ENV): $(ENV_OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(ENV_OBJS) -L $(LIBFT_DIR) -lft -o $(NAME_ENV)

$(OBJ_DIR)/common/%.o: common/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/env/%.o: env/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/src/env/%.o: ../src/env/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	rm -f env_test *.o

fclean: clean
	rm -rf bin

re: fclean all

.PHONY: all clean env_test fclean re run_env_test
