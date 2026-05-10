/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_ops.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 19:32:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/08 19:13:25 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"

static t_env	*env_new_node(char *key, char *value)
{
	t_env	*node;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->key = ft_strdup(key);
	if (!node->key)
	{
		free(node);
		return (NULL);
	}
	node->value = NULL;
	if (value)
		node->value = ft_strdup(value);
	if (value && !node->value)
	{
		free(node->key);
		free(node);
		return (NULL);
	}
	node->next = NULL;
	return (node);
}

char	*env_get(t_env *env, char *key)
{
	size_t	key_len;

	if (!key)
		return (NULL);
	key_len = ft_strlen(key) + 1;
	while (env)
	{
		if (ft_strncmp(env->key, key, key_len) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

int	env_set(t_env **env, char *key, char *value)
{
	t_env	**current;
	t_env	*new_node;
	char	*new_value;

	if (!env || !key)
		return (1);
	current = env;
	while (*current && ft_strncmp((*current)->key, key, ft_strlen(key) + 1))
		current = &(*current)->next;
	if (*current)
	{
		new_value = NULL;
		if (value)
			new_value = ft_strdup(value);
		if (value && !new_value)
			return (1);
		free((*current)->value);
		(*current)->value = new_value;
		return (0);
	}
	new_node = env_new_node(key, value);
	if (!new_node)
		return (1);
	*current = new_node;
	return (0);
}

int	env_unset(t_env **env, char *key)
{
	t_env	*current;
	t_env	*previous;

	if (!env || !key)
		return (1);
	previous = NULL;
	current = *env;
	while (current)
	{
		if (ft_strncmp(current->key, key, ft_strlen(key) + 1) == 0)
		{
			if (previous)
				previous->next = current->next;
			else
				*env = current->next;
			free(current->key);
			free(current->value);
			free(current);
			break ;
		}
		previous = current;
		current = current->next;
	}
	return (0);
}

void	env_free(t_env *env)
{
	t_env	*next;

	while (env)
	{
		next = env->next;
		free(env->key);
		free(env->value);
		free(env);
		env = next;
	}
}
