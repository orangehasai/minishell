/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipeline_child.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/17 01:05:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/17 01:05:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

void	exec_pipeline_child(t_cmd *cmd, t_shell *shell, int prev_fd,
		int pipe_fd[2])
{
	if (prev_fd != -1 && dup2(prev_fd, STDIN_FILENO) < 0)
	{
		perror("minishell: dup2");
		exit(1);
	}
	if (cmd->next && dup2(pipe_fd[1], STDOUT_FILENO) < 0)
	{
		perror("minishell: dup2");
		exit(1);
	}
	if (prev_fd != -1)
		close(prev_fd);
	if (cmd->next)
	{
		close(pipe_fd[0]);
		close(pipe_fd[1]);
	}
	if (apply_redirections(cmd->redirs))
		exit(1);
	if (cmd->argv && cmd->argv[0] && is_builtin_cmd(cmd->argv[0]))
		exit(exec_builtin_cmd(cmd, shell));
	exec_external_cmd(cmd, shell);
}
