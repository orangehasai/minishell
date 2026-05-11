#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static void	parent_handler(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "parent: SIGINT received\n", 24);
}

int	main(void)
{
	struct sigaction	sa;
	pid_t				pid;
	int					status;

	sa.sa_handler = parent_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) < 0)
		return (perror("sigaction"), 1);
	pid = fork();
	if (pid < 0)
		return (perror("fork"), 1);
	if (pid == 0)
	{
		sa.sa_handler = SIG_DFL;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		if (sigaction(SIGINT, &sa, NULL) < 0)
			return (perror("sigaction"), 1);
		printf("child: waiting for SIGINT\n");
		fflush(stdout);
		sleep(10);
		printf("child: still alive\n");
		exit(0);
	}
	sleep(1);
	printf("parent: sending SIGINT to child pid=%d\n", pid);
	kill(pid, SIGINT);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
	{
		printf("parent: child killed by signal=%d\n", WTERMSIG(status));
		printf("parent: minishell style status=%d\n", 128 + WTERMSIG(status));
	}
	return (0);
}
