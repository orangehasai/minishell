/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/17 00:50:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/17 00:50:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "signals.h"

static pid_t	start_pipeline_cmd(t_cmd *cmd, t_shell *shell, int prev_fd,
		int pipe_fd[2])
{
	pid_t	pid;

	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
	if (cmd->next && pipe(pipe_fd) < 0)
		return (perror("pipe"), -1);
	pid = fork();
	if (pid < 0)
		return (perror("fork"), -1);
	if (pid == 0)
	{
		setup_signals_child();
		exec_pipeline_child(cmd, shell, prev_fd, pipe_fd);
	}
	return (pid);
}

static void	advance_pipeline_parent(t_cmd *cmd, int *prev_fd, int pipe_fd[2])
{
	if (*prev_fd != -1)
		close(*prev_fd);
	if (cmd->next)
	{
		close(pipe_fd[1]);
		*prev_fd = pipe_fd[0];
	}
}

static int	count_pipeline_cmds(t_cmd *cmds)
{
	int		count;

	count = 0;
	while (cmds)
	{
		count++;
		cmds = cmds->next;
	}
	return (count);
}

static int	launch_pipeline(t_cmd *cmds, t_shell *shell, pid_t *pids)
{
	int		pipe_fd[2];
	int		prev_fd;
	int		index;
	pid_t	pid;

	prev_fd = -1;
	index = 0;
	while (cmds)
	{
		pid = start_pipeline_cmd(cmds, shell, prev_fd, pipe_fd);
		if (pid < 0)
		{
			cleanup_pipeline_error(prev_fd, pipe_fd, pids, index);
			return (1);
		}
		advance_pipeline_parent(cmds, &prev_fd, pipe_fd);
		pids[index++] = pid;
		cmds = cmds->next;
	}
	return (0);
}

int	exec_pipeline(t_cmd *cmds, t_shell *shell)
{
	int		count;
	int		status;
	pid_t	*pids;

	if (!cmds || !shell)
		return (1);
	count = count_pipeline_cmds(cmds);
	pids = malloc(sizeof(pid_t) * count);
	if (!pids)
		return (1);
	setup_signals_parent_wait();
	if (launch_pipeline(cmds, shell, pids))
	{
		if (isatty(STDIN_FILENO))
			setup_signals_interactive();
		free(pids);
		return (1);
	}
	status = wait_all(pids, count, pids[count - 1]);
	if (isatty(STDIN_FILENO))
		setup_signals_interactive();
	free(pids);
	return (status);
}
