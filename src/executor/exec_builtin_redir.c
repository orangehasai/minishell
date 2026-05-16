/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin_redir.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 23:30:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 23:30:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

static int	save_stdio(int saved[2])
{
	saved[0] = dup(STDIN_FILENO);
	if (saved[0] < 0)
	{
		perror("minishell: dup");
		return (1);
	}
	saved[1] = dup(STDOUT_FILENO);
	if (saved[1] < 0)
	{
		perror("minishell: dup");
		close(saved[0]);
		return (1);
	}
	return (0);
}

static int	restore_stdio(int saved[2])
{
	int	status;

	status = 0;
	if (dup2(saved[0], STDIN_FILENO) < 0)
		status = (perror("minishell: dup2"), 1);
	if (dup2(saved[1], STDOUT_FILENO) < 0)
		status = (perror("minishell: dup2"), 1);
	if (close(saved[0]) < 0)
		status = (perror("minishell: close"), 1);
	if (close(saved[1]) < 0)
		status = (perror("minishell: close"), 1);
	return (status);
}

int	exec_builtin_with_redir(t_cmd *cmd, t_shell *shell)
{
	int	saved[2];
	int	status;

	if (!cmd || !shell)
		return (1);
	if (save_stdio(saved))
		return (1);
	status = 1;
	if (!apply_redirections(cmd->redirs))
		status = exec_builtin_cmd(cmd, shell);
	if (restore_stdio(saved))
		return (1);
	return (status);
}
