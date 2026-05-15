/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_test_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 20:44:09 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/09 01:09:32 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env_test.h"

int	env_count(t_env *env)
{
	int	count;

	count = 0;
	while (env)
	{
		count++;
		env = env->next;
	}
	return (count);
}

t_env	*env_find(t_env *env, char *key)
{
	while (env)
	{
		if (strings_equal(env->key, key))
			return (env);
		env = env->next;
	}
	return (NULL);
}

int	env_array_len(char **array)
{
	int	length;

	if (!array)
		return (0);
	length = 0;
	while (array[length])
		length++;
	return (length);
}

void	free_env_array(char **array)
{
	int	index;

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

int	capture_export_output(t_env *env, char *buffer, size_t size)
{
	int		save_fd;
	int		pipe_fd[2];
	ssize_t	bytes;

	if (!buffer || size == 0 || pipe(pipe_fd) == -1)
		return (1);
	save_fd = dup(STDOUT_FILENO);
	if (save_fd == -1)
		return (close(pipe_fd[0]), close(pipe_fd[1]), 1);
	dup2(pipe_fd[1], STDOUT_FILENO);
	close(pipe_fd[1]);
	env_print_export(env);
	dup2(save_fd, STDOUT_FILENO);
	close(save_fd);
	bytes = read(pipe_fd[0], buffer, size - 1);
	close(pipe_fd[0]);
	if (bytes < 0)
		return (1);
	buffer[bytes] = '\0';
	return (0);
}
