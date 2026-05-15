/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/13 07:31:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/16 02:59:44 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

int	builtin_update_pwd_values(t_env **env, char *oldpwd, char *newpwd)
{
	int	status;

	if (!env || !oldpwd || !newpwd)
		return (1);
	status = 0;
	if (env_set(env, "PWD", newpwd))
		status = 1;
	if (env_set(env, "OLDPWD", oldpwd))
		status = 1;
	return (status);
}

static int	update_pwd_vars(t_shell *shell, char *oldpwd)
{
	char	*newpwd;
	int		status;

	newpwd = getcwd(NULL, 0);
	if (!newpwd)
	{
		free(oldpwd);
		perror("minishell: cd");
		return (1);
	}
	status = builtin_update_pwd_values(&shell->env, oldpwd, newpwd);
	free(oldpwd);
	free(newpwd);
	if (status)
	{
		perror("minishell: cd");
		return (1);
	}
	return (0);
}

static char	*get_cd_target(char **argv, t_shell *shell)
{
	char	*home;

	if (argv && argv[1] && argv[2])
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", 2);
		return (NULL);
	}
	if (argv && argv[1])
		return (argv[1]);
	home = env_get(shell->env, "HOME");
	if (!home)
	{
		ft_putstr_fd("minishell: cd: HOME not set\n", 2);
		return (NULL);
	}
	return (home);
}

static char	*get_oldpwd_value(t_shell *shell)
{
	char	*pwd;

	pwd = env_get(shell->env, "PWD");
	if (pwd)
		return (ft_strdup(pwd));
	pwd = getcwd(NULL, 0);
	if (pwd)
		return (pwd);
	return (ft_strdup(""));
}

int	builtin_cd(char **argv, t_shell *shell)
{
	char	*target;
	char	*oldpwd;

	if (!shell)
		return (1);
	target = get_cd_target(argv, shell);
	if (!target)
		return (1);
	oldpwd = get_oldpwd_value(shell);
	if (!oldpwd)
	{
		perror("minishell: cd");
		return (1);
	}
	if (chdir(target) != 0)
	{
		free(oldpwd);
		ft_putstr_fd("minishell: cd: ", 2);
		perror(target);
		return (1);
	}
	return (update_pwd_vars(shell, oldpwd));
}
