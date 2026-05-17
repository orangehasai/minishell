/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_test_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 18:35:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 18:35:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_test.h"
#include <limits.h>

int	init_test_shell(t_shell *shell, char **envp)
{
	if (!shell)
		return (1);
	shell->env = NULL;
	shell->last_status = 0;
	return (env_init(&shell->env, envp));
}

void	init_exec_case(t_exec_case *case_data, char *buffer, size_t size,
		int fd)
{
	ft_bzero(case_data, sizeof(*case_data));
	case_data->cmd.argv = case_data->argv;
	case_data->capture.buffer = buffer;
	case_data->capture.size = size;
	case_data->capture.fd = fd;
}

void	cleanup_exec_case(t_exec_case *case_data)
{
	if (!case_data)
		return ;
	env_free(case_data->shell.env);
	case_data->shell.env = NULL;
}

int	capture_exec_output(t_cmd *cmd, t_shell *shell, t_capture_exec *capture)
{
	ssize_t	bytes;
	int		save_fd;
	int		pipe_fd[2];

	if (!capture || !capture->buffer || capture->size == 0
		|| pipe(pipe_fd) == -1)
		return (1);
	save_fd = dup(capture->fd);
	if (save_fd == -1)
		return (close(pipe_fd[0]), close(pipe_fd[1]), 1);
	if (dup2(pipe_fd[1], capture->fd) == -1)
		return (close(save_fd), close(pipe_fd[0]), close(pipe_fd[1]), 1);
	close(pipe_fd[1]);
	capture->status = execute(cmd, shell);
	if (dup2(save_fd, capture->fd) == -1)
		return (close(save_fd), close(pipe_fd[0]), 1);
	close(save_fd);
	bytes = read(pipe_fd[0], capture->buffer, capture->size - 1);
	close(pipe_fd[0]);
	if (bytes < 0)
		return (1);
	capture->buffer[bytes] = '\0';
	return (0);
}

int	capture_pipeline_output(t_cmd *cmds, t_shell *shell,
		t_capture_exec *capture)
{
	ssize_t	bytes;
	int		save_fd;
	int		pipe_fd[2];

	if (!capture || !capture->buffer || capture->size == 0
		|| pipe(pipe_fd) == -1)
		return (1);
	save_fd = dup(capture->fd);
	if (save_fd == -1)
		return (close(pipe_fd[0]), close(pipe_fd[1]), 1);
	if (dup2(pipe_fd[1], capture->fd) == -1)
		return (close(save_fd), close(pipe_fd[0]), close(pipe_fd[1]), 1);
	close(pipe_fd[1]);
	capture->status = execute(cmds, shell);
	if (dup2(save_fd, capture->fd) == -1)
		return (close(save_fd), close(pipe_fd[0]), 1);
	close(save_fd);
	bytes = read(pipe_fd[0], capture->buffer, capture->size - 1);
	close(pipe_fd[0]);
	if (bytes < 0)
		return (1);
	capture->buffer[bytes] = '\0';
	return (0);
}

int	create_test_file(char *path, mode_t mode, char *content)
{
	ssize_t	length;
	int		fd;

	unlink(path);
	fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, mode);
	if (fd == -1)
		return (1);
	length = ft_strlen(content);
	if (write(fd, content, length) != length)
		return (close(fd), unlink(path), 1);
	if (close(fd) == -1)
		return (unlink(path), 1);
	return (0);
}
