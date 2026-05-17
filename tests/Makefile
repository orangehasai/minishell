NAME_ENV	= bin/env_test
NAME_EXEC	= bin/executor_test

CC		= cc
CFLAGS		= -Wall -Wextra -Werror
INCLUDES	= -I ../includes -I ../libft -I include
TEST_HEADERS	:= $(wildcard include/*.h ../includes/*.h)

LIBFT_DIR	= ../libft
LIBFT		= $(LIBFT_DIR)/libft.a
OBJ_DIR		= obj

ENV_OBJS	= $(OBJ_DIR)/common/test_output.o \
			  $(OBJ_DIR)/env/env_test_report.o \
			  $(OBJ_DIR)/env/env_test_main.o \
			  $(OBJ_DIR)/env/env_test_utils.o \
			  $(OBJ_DIR)/env/env_test_case_init.o \
			  $(OBJ_DIR)/env/env_test_case_ops.o \
			  $(OBJ_DIR)/env/env_test_case_utils.o \
			  $(OBJ_DIR)/env/env_test_case_builtin.o \
			  $(OBJ_DIR)/src/env/env_init.o \
			  $(OBJ_DIR)/src/env/env_ops.o \
			  $(OBJ_DIR)/src/env/env_utils.o \
			  $(OBJ_DIR)/src/env/env_export.o \
			  $(OBJ_DIR)/src/builtins/builtin_env.o \
			  $(OBJ_DIR)/src/builtins/builtin_utils.o

EXEC_OBJS	= $(OBJ_DIR)/common/test_output.o \
			  $(OBJ_DIR)/executor/executor_test_main.o \
			  $(OBJ_DIR)/executor/executor_test_utils.o \
			  $(OBJ_DIR)/executor/executor_test_report.o \
			  $(OBJ_DIR)/executor/executor_test_case_external.o \
			  $(OBJ_DIR)/executor/executor_test_case_echo.o \
			  $(OBJ_DIR)/executor/executor_test_case_redir.o \
			  $(OBJ_DIR)/executor/executor_test_case_builtin.o \
			  $(OBJ_DIR)/executor/executor_test_case_cd.o \
			  $(OBJ_DIR)/executor/executor_test_case_path.o \
			  $(OBJ_DIR)/src/env/env_init.o \
			  $(OBJ_DIR)/src/env/env_ops.o \
			  $(OBJ_DIR)/src/env/env_utils.o \
			  $(OBJ_DIR)/src/env/env_export.o \
			  $(OBJ_DIR)/src/builtins/builtin_cd.o \
			  $(OBJ_DIR)/src/builtins/builtin_echo.o \
			  $(OBJ_DIR)/src/builtins/builtin_env.o \
			  $(OBJ_DIR)/src/builtins/builtin_pwd.o \
			  $(OBJ_DIR)/src/builtins/builtin_utils.o \
			  $(OBJ_DIR)/src/executor/exec_builtin.o \
			  $(OBJ_DIR)/src/executor/exec_error.o \
			  $(OBJ_DIR)/src/executor/exec_simple.o \
			  $(OBJ_DIR)/src/executor/redir.o \
			  $(OBJ_DIR)/src/executor/path_resolve.o

all: env_test executor_test

env_test: $(LIBFT) $(NAME_ENV)

executor_test: $(LIBFT) $(NAME_EXEC)

run_env_test: env_test
	./$(NAME_ENV)

run_executor_test: executor_test
	./$(NAME_EXEC)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME_ENV): $(ENV_OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(ENV_OBJS) -L $(LIBFT_DIR) -lft -o $(NAME_ENV)

$(NAME_EXEC): $(EXEC_OBJS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(EXEC_OBJS) -L $(LIBFT_DIR) -lft -o $(NAME_EXEC)

$(OBJ_DIR)/common/%.o: common/%.c $(TEST_HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/env/%.o: env/%.c $(TEST_HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/executor/%.o: executor/%.c $(TEST_HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/src/env/%.o: ../src/env/%.c $(TEST_HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/src/builtins/%.o: ../src/builtins/%.c $(TEST_HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/src/executor/%.o: ../src/executor/%.c $(TEST_HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	rm -f env_test executor_test *.o

fclean: clean
	rm -rf bin

re: fclean all

.PHONY: all clean env_test executor_test fclean re run_env_test run_executor_test
