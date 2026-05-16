/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 16:58:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/16 16:58:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

static int	is_valid_identifier(char *arg)
{
	int	i;

	if (!arg || !arg[0])
		return (0);
	if (!ft_isalpha(arg[0]) && arg[0] != '_')
		return (0);
	i = 1;
	while (arg[i] && arg[i] != '=')
	{
		if (!ft_isalnum(arg[i]) && arg[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

static void	print_identifier_error(char *arg)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
}

static int	should_skip_export(t_shell *shell, char *arg, char *key)
{
	if (ft_strchr(arg, '='))
		return (0);
	if (!env_get(shell->env, key))
		return (0);
	return (1);
}

static int	apply_export_arg(t_shell *shell, char *arg)
{
	char	*key;
	char	*value;

	if (split_key_value(arg, &key, &value))
		return (1);
	if (should_skip_export(shell, arg, key))
	{
		free(key);
		free(value);
		return (0);
	}
	if (env_set(&shell->env, key, value))
	{
		free(key);
		free(value);
		return (1);
	}
	free(key);
	free(value);
	return (0);
}

int	builtin_export(char **argv, t_shell *shell)
{
	int	i;
	int	status;

	if (!shell)
		return (1);
	if (!argv || !argv[1])
		return (env_print_export(shell->env));
	i = 1;
	status = 0;
	while (argv[i])
	{
		if (!is_valid_identifier(argv[i]))
		{
			print_identifier_error(argv[i]);
			status = 1;
			i++;
			continue ;
		}
		if (apply_export_arg(shell, argv[i]))
			status = 1;
		i++;
	}
	return (status);
}
