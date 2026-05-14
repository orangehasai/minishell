#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int	main(void)
{
	pid_t	pid;
	int		status;

	dprintf(STDOUT_FILENO, "before fork: pid=%d\n", getpid());
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
	{
		printf("child: pid=%d parent=%d\n", getpid(), getppid());
		printf("child: exiting with code 42\n");
		exit(42);
	}
	printf("parent: child pid=%d\n", pid);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		printf("parent: child exit code=%d\n", WEXITSTATUS(status));
	return (0);
}
