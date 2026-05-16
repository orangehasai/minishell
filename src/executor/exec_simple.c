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

static void	free_env_array(char **array)
{
	size_t	index;

	if (!array)
		return ;
	index = 0;
	while (array[index])
	{
		free(array[index]);
		index++;
	}
	free(array);
}

static int	command_status_from_errno(int error_code)
{
	if (error_code == ENOENT)
		return (127);
	if (error_code == EACCES || error_code == ENOTDIR
		|| error_code == EISDIR || error_code == ENOEXEC)
		return (126);
	return (1);
}

static void	exec_child_process(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	char	**envp;
	int		status;

	path = resolve_path(cmd->argv[0], shell);
	if (!path)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror(cmd->argv[0]);
		exit(command_status_from_errno(errno));
	}
	envp = env_to_array(shell->env);
	if (!envp)
		exit(1);
	execve(path, cmd->argv, envp);
	status = command_status_from_errno(errno);
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	perror(cmd->argv[0]);
	free(path);
	free_env_array(envp);
	exit(status);
}

static int	wait_child_status(pid_t pid)
{
	int	status;

	if (waitpid(pid, &status, 0) < 0)
		return (1);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

int	exec_simple_cmd(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;

	if (!cmd || !cmd->argv || !cmd->argv[0] || !shell)
		return (1);
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
		exec_child_process(cmd, shell);
	return (wait_child_status(pid));
}
