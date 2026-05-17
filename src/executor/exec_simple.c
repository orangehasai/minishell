/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_simple.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 15:18:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 15:18:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include "signals.h"

static void	exec_child_process(t_cmd *cmd, t_shell *shell)
{
	if (apply_redirections(cmd->redirs))
		exit(1);
	exec_external_cmd(cmd, shell);
}

static int	wait_child_status(pid_t pid)
{
	int	status;

	if (waitpid(pid, &status, 0) < 0)
		return (1);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGQUIT)
			ft_putendl_fd("Quit (core dumped)", STDERR_FILENO);
		return (128 + WTERMSIG(status));
	}
	return (1);
}

static int	wait_external_cmd(pid_t pid)
{
	int	status;

	setup_signals_parent_wait();
	status = wait_child_status(pid);
	if (isatty(STDIN_FILENO))
		setup_signals_interactive();
	return (status);
}

int	exec_simple_cmd(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;

	if (!cmd || !shell)
		return (1);
	if (cmd->argv && cmd->argv[0] && is_builtin_cmd(cmd->argv[0])
		&& !cmd->redirs)
		return (exec_builtin_cmd(cmd, shell));
	if (cmd->argv && cmd->argv[0] && is_builtin_cmd(cmd->argv[0])
		&& cmd->redirs)
		return (exec_builtin_with_redir(cmd, shell));
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
	{
		setup_signals_child();
		exec_child_process(cmd, shell);
	}
	return (wait_external_cmd(pid));
}
