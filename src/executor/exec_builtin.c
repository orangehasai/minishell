/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 15:35:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 15:42:01 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "executor.h"

static t_builtin_fn	find_builtin_cmd(char *cmd)
{
	if (!cmd)
		return (NULL);
	if (ft_strncmp(cmd, "echo", 5) == 0)
		return (builtin_echo);
	if (ft_strncmp(cmd, "cd", 3) == 0)
		return (builtin_cd);
	if (ft_strncmp(cmd, "env", 4) == 0)
		return (builtin_env);
	if (ft_strncmp(cmd, "pwd", 4) == 0)
		return (builtin_pwd);
	if (ft_strncmp(cmd, "export", 7) == 0)
		return (builtin_export);
	if (ft_strncmp(cmd, "unset", 6) == 0)
		return (builtin_unset);
	return (NULL);
}

int	is_builtin_cmd(char *cmd)
{
	return (find_builtin_cmd(cmd) != NULL);
}

int	exec_builtin_cmd(t_cmd *cmd, t_shell *shell)
{
	t_builtin_fn	builtin_cmd;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (1);
	builtin_cmd = find_builtin_cmd(cmd->argv[0]);
	if (!builtin_cmd)
		return (1);
	return (builtin_cmd(cmd->argv, shell));
}
