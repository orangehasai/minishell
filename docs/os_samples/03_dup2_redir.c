#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int	main(void)
{
	int	fd;
	int	saved_stdout;

	saved_stdout = dup(STDOUT_FILENO);
	if (saved_stdout < 0)
	{
		perror("dup");
		return (1);
	}
	fd = open("redirect_output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		perror("open");
		close(saved_stdout);
		return (1);
	}
	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		perror("dup2");
		close(fd);
		close(saved_stdout);
		return (1);
	}
	close(fd);
	printf("this line goes into redirect_output.txt\n");
	fflush(stdout);
	if (dup2(saved_stdout, STDOUT_FILENO) < 0)
	{
		perror("dup2 restore");
		close(saved_stdout);
		return (1);
	}
	close(saved_stdout);
	printf("stdout was restored to the terminal\n");
	printf("check docs/os_samples/redirect_output.txt or ./redirect_output.txt\n");
	return (0);
}
