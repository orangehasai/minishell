/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/17 11:40:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/17 11:40:00 by takenakatak      ###   ########.fr       */
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

void	exec_external_cmd(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	char	**envp;

	if (!cmd->argv || !cmd->argv[0])
		exit(0);
	path = resolve_path(cmd->argv[0], shell);
	if (!path)
	{
		print_resolve_error(cmd->argv[0]);
		exit(command_status_from_errno(errno));
	}
	envp = env_to_array(shell->env);
	if (!envp)
		return (free(path), exit(1));
	execve(path, cmd->argv, envp);
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	perror(path);
	free(path);
	free_env_array(envp);
	exit(command_status_from_errno(errno));
}
