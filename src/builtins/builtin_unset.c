/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 21:38:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/16 21:38:00 by skeita           ###   ########.fr       */
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
	while (arg[i])
	{
		if (!ft_isalnum(arg[i]) && arg[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

static void	print_unset_error(char *arg)
{
	ft_putstr_fd("minishell: unset: `", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
}

int	builtin_unset(char **argv, t_shell *shell)
{
	int	i;
	int	status;

	if (!shell || !argv)
		return (1);
	i = 1;
	status = 0;
	while (argv[i])
	{
		if (!is_valid_identifier(argv[i]))
		{
			print_unset_error(argv[i]);
			status = 1;
		}
		else
			env_unset(&shell->env, argv[i]);
		i++;
	}
	return (status);
}
