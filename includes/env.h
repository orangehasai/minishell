/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 19:32:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/08 19:11:48 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_H
# define ENV_H

# include "minishell.h"

t_env	*env_init(char **envp);
char	*env_get(t_env *env, char *key);
int		env_set(t_env **env, char *key, char *value);
int		env_unset(t_env **env, char *key);
void	env_free(t_env *env);

#endif
