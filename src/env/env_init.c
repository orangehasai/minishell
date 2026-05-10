/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 19:32:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/08 19:13:22 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"

static size_t	get_key_len(char *entry)
{
	size_t	index;

	index = 0;
	while (entry[index] && entry[index] != '=')
		index++;
	return (index);
}

static int	add_env_entry(t_env **env, char *entry)
{
	char	*key;
	char	*value;
	char	*equal;
	int		status;

	equal = ft_strchr(entry, '=');
	key = ft_substr(entry, 0, get_key_len(entry));
	if (!key)
		return (1);
	value = NULL;
	if (equal)
		value = ft_strdup(equal + 1);
	if (equal && !value)
	{
		free(key);
		return (1);
	}
	status = env_set(env, key, value);
	free(key);
	free(value);
	return (status);
}

t_env	*env_init(char **envp)
{
	t_env	*env;
	int		index;

	env = NULL;
	index = 0;
	errno = 0;
	while (envp && envp[index])
	{
		if (add_env_entry(&env, envp[index]))
		{
			errno = ENOMEM;
			env_free(env);
			return (NULL);
		}
		index++;
	}
	return (env);
}
