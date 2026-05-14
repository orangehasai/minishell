CC = cc
CFLAGS = -Wall -Wextra -Werror

TARGETS = 01_fork_wait \
		  02_execve \
		  03_dup2_redir \
		  04_pipe \
		  05_signal \
		  06_path_search

all: $(TARGETS)

01_fork_wait: 01_fork_wait.c
	$(CC) $(CFLAGS) $< -o $@

02_execve: 02_execve.c
	$(CC) $(CFLAGS) $< -o $@

03_dup2_redir: 03_dup2_redir.c
	$(CC) $(CFLAGS) $< -o $@

04_pipe: 04_pipe.c
	$(CC) $(CFLAGS) $< -o $@

05_signal: 05_signal.c
	$(CC) $(CFLAGS) $< -o $@

06_path_search: 06_path_search.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS) redirect_output.txt

fclean: clean

re: fclean all

.PHONY: all clean fclean re
