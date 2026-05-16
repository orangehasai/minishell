/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 21:08:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/12 21:08:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

static char	*build_env_line(t_env *env)
{
	char	*line;
	char	*temp;

	line = ft_strjoin(env->key, "=");
	if (!line)
		return (NULL);
	temp = line;
	line = ft_strjoin(temp, env->value);
	free(temp);
	if (!line)
		return (NULL);
	temp = line;
	line = ft_strjoin(temp, "\n");
	free(temp);
	return (line);
}

static int	print_env_entry(t_env *env)
{
	char	*line;

	line = build_env_line(env);
	if (!line)
		return (1);
	if (put_str_fd_safe(line, 1))
	{
		free(line);
		return (1);
	}
	free(line);
	return (0);
}

int	builtin_env(char **argv, t_shell *shell)
{
	t_env	*current_env;

	if (argv && argv[1])
	{
		ft_putstr_fd("minishell: env: options or arguments are not supported\n",
			2);
		return (1);
	}
	if (!shell)
		return (1);
	current_env = shell->env;
	while (current_env)
	{
		if (current_env->key && current_env->value)
		{
			if (print_env_entry(current_env))
			{
				perror("minishell: env");
				return (1);
			}
		}
		current_env = current_env->next;
	}
	return (0);
}
