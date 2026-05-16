/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline_wait.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/17 01:35:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/17 01:35:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

static int	wait_pipeline_cmd(pid_t pid, pid_t last_pid, int *last_status)
{
	int		status;

	if (waitpid(pid, &status, 0) < 0)
		return (1);
	if (pid == last_pid)
	{
		if (WIFEXITED(status))
			*last_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			*last_status = 128 + WTERMSIG(status);
		else
			*last_status = 1;
	}
	return (0);
}

int	wait_pipeline(pid_t *pids, int count, pid_t last_pid)
{
	int	index;
	int	last_status;

	index = 0;
	last_status = 1;
	while (index < count)
	{
		if (wait_pipeline_cmd(pids[index], last_pid, &last_status))
			return (1);
		index++;
	}
	return (last_status);
}

void	cleanup_pipeline_error(int prev_fd, int pipe_fd[2], pid_t *pids,
		int count)
{
	if (prev_fd != -1)
		close(prev_fd);
	if (pipe_fd[0] != -1)
		close(pipe_fd[0]);
	if (pipe_fd[1] != -1)
		close(pipe_fd[1]);
	wait_pipeline(pids, count, -1);
}
