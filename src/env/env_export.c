/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_export.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 00:22:44 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/09 00:22:44 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"

static int	compare_keys(char *left, char *right)
{
	size_t	index;

	index = 0;
	while (left[index] && right[index] && left[index] == right[index])
		index++;
	return ((unsigned char)left[index] - (unsigned char)right[index]);
}

static t_env	*find_next_export(t_env *env, char *last_key)
{
	t_env	*next;

	next = NULL;
	while (env)
	{
		if ((!last_key
				|| compare_keys(env->key, last_key) > 0)
			&& (!next
				|| compare_keys(env->key, next->key) < 0))
			next = env;
		env = env->next;
	}
	return (next);
}

int	split_key_value(char *str, char **key, char **value)
{
	char	*equal;

	if (!str || !key || !value)
		return (1);
	equal = ft_strchr(str, '=');
	if (!equal)
	{
		*key = ft_strdup(str);
		*value = NULL;
		return (*key == NULL);
	}
	*key = ft_substr(str, 0, equal - str);
	if (!*key)
		return (1);
	*value = ft_strdup(equal + 1);
	if (!*value)
		return (free(*key), 1);
	return (0);
}

int	env_print_export(t_env *env)
{
	t_env	*current;
	char	*last_key;

	last_key = NULL;
	current = find_next_export(env, last_key);
	while (current)
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(current->key, STDOUT_FILENO);
		if (current->value)
		{
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(current->value, STDOUT_FILENO);
			ft_putchar_fd('"', STDOUT_FILENO);
		}
		ft_putchar_fd('\n', STDOUT_FILENO);
		last_key = current->key;
		current = find_next_export(env, last_key);
	}
	return (0);
}
