/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 21:08:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/12 21:08:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

int	builtin_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("minishell: pwd");
		return (1);
	}
	if (put_str_fd_safe(cwd, 1) || put_str_fd_safe("\n", 1))
	{
		perror("minishell: pwd");
		free(cwd);
		return (1);
	}
	free(cwd);
	return (0);
}
