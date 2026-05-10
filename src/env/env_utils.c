/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 23:49:18 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/08 23:49:52 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"

static char	*create_env_line(char *key, char *value)
{
	char	*line;
	char	*temp;

	line = ft_strjoin(key, "=");
	if (!line)
		return (NULL);
	temp = line;
	line = ft_strjoin(temp, value);
	free(temp);
	return (line);
}

static void	free_env_lines(char **array, int size)
{
	while (size > 0)
		free(array[--size]);
	free(array);
}

static int	count_env_values(t_env *env)
{
	int		count;

	count = 0;
	while (env)
	{
		if (env->value)
			count++;
		env = env->next;
	}
	return (count);
}

char	**env_to_array(t_env *env)
{
	char	**array;
	int		count;
	int		index;

	count = count_env_values(env);
	array = ft_calloc(count + 1, sizeof(char *));
	if (!array)
		return (NULL);
	index = 0;
	while (env)
	{
		if (env->value)
		{
			array[index] = create_env_line(env->key, env->value);
			if (!array[index])
				return (free_env_lines(array, index), NULL);
			index++;
		}
		env = env->next;
	}
	return (array);
}
