/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/17 11:25:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/17 11:25:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

static int	parse_digits(char *arg, int i, int sign, unsigned long long *num)
{
	unsigned long long	limit;

	limit = 9223372036854775807ULL + (sign < 0);
	while (arg[i] && ft_isdigit(arg[i]))
	{
		if (*num > limit / 10 || (*num == limit / 10
				&& (unsigned long long)(arg[i] - '0') > limit % 10))
			return (1);
		*num = *num * 10 + (unsigned long long)(arg[i] - '0');
		i++;
	}
	if (arg[i] != '\0')
		return (1);
	return (0);
}

static int	parse_exit_code(char *arg, int *status)
{
	unsigned long long	num;
	int					sign;
	int					i;

	num = 0;
	sign = 1;
	i = 0;
	if (!arg || !arg[0])
		return (1);
	if (arg[i] == '+' || arg[i] == '-')
	{
		if (arg[i] == '-')
			sign = -1;
		i++;
	}
	if (!ft_isdigit(arg[i]))
		return (1);
	if (parse_digits(arg, i, sign, &num))
		return (1);
	if (sign > 0)
		*status = (unsigned char)(num % 256);
	else
		*status = (unsigned char)((256 - (num % 256)) % 256);
	return (0);
}

static void	print_numeric_error(char *arg)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd(": numeric argument required", STDERR_FILENO);
}

int	builtin_exit(char **argv, t_shell *shell)
{
	int	status;

	if (!shell || !argv || !argv[0])
		return (1);
	if (isatty(STDIN_FILENO))
		ft_putendl_fd("exit", STDERR_FILENO);
	if (!argv[1])
	{
		shell->should_exit = 1;
		return (shell->last_status);
	}
	if (parse_exit_code(argv[1], &status))
	{
		print_numeric_error(argv[1]);
		shell->should_exit = 1;
		return (2);
	}
	if (argv[2])
	{
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		return (1);
	}
	shell->should_exit = 1;
	return (status);
}
