#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int	main(void)
{
	pid_t	pid;
	int		status;
	char	*argv[] = {"/usr/bin/env", NULL};
	char	*envp[] = {"DEMO_NAME=minishell", "DEMO_STAGE=execve", NULL};

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
	{
		write(STDOUT_FILENO, "child: execve will replace this process\n", 40);
		execve("/usr/bin/env", argv, envp);
		perror("execve");
		exit(1);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		printf("parent: child exit code=%d\n", WEXITSTATUS(status));
	return (0);
}
