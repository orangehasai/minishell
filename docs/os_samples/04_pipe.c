#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static void	reader_loop(void)
{
	char	buffer[64];
	ssize_t	read_size;

	read_size = read(STDIN_FILENO, buffer, sizeof(buffer));
	while (read_size > 0)
	{
		write(STDOUT_FILENO, buffer, read_size);
		read_size = read(STDIN_FILENO, buffer, sizeof(buffer));
	}
}

int	main(void)
{
	int		pipe_fd[2];
	pid_t	writer_pid;
	pid_t	reader_pid;

	if (pipe(pipe_fd) < 0)
		return (perror("pipe"), 1);
	writer_pid = fork();
	if (writer_pid < 0)
		return (perror("fork"), 1);
	if (writer_pid == 0)
	{
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		write(STDOUT_FILENO, "hello through pipe\n", 19);
		exit(0);
	}
	reader_pid = fork();
	if (reader_pid < 0)
		return (perror("fork"), 1);
	if (reader_pid == 0)
	{
		dup2(pipe_fd[0], STDIN_FILENO);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		reader_loop();
		exit(0);
	}
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	waitpid(writer_pid, NULL, 0);
	waitpid(reader_pid, NULL, 0);
	return (0);
}
