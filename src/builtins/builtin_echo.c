/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 20:45:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 20:45:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

static int	is_n_flag(char *arg)
{
	int	index;

	if (!arg || arg[0] != '-' || arg[1] != 'n')
		return (0);
	index = 1;
	while (arg[index] == 'n')
		index++;
	return (arg[index] == '\0');
}

static int	write_echo_args(char **argv, int index)
{
	while (argv[index])
	{
		if (put_str_fd_safe(argv[index], STDOUT_FILENO))
			return (1);
		if (argv[index + 1] && put_str_fd_safe(" ", STDOUT_FILENO))
			return (1);
		index++;
	}
	return (0);
}

int	builtin_echo(char **argv, t_shell *shell)
{
	int	index;
	int	newline;

	(void)shell;
	index = 1;
	newline = 1;
	while (argv[index] && is_n_flag(argv[index]))
	{
		newline = 0;
		index++;
	}
	if (write_echo_args(argv, index))
		return (perror("minishell: echo"), 1);
	if (newline && put_str_fd_safe("\n", STDOUT_FILENO))
		return (perror("minishell: echo"), 1);
	return (0);
}
