/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_test_case_init.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 20:44:09 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/08 20:16:14 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env_test.h"

int	test_env_init(void)
{
	char	user_entry[16];
	char	empty_entry[8];
	char	name_entry[8];
	char	*envp[4];
	t_env	*env;

	ft_strlcpy(user_entry, "USER=alice", sizeof(user_entry));
	ft_strlcpy(empty_entry, "EMPTY=", sizeof(empty_entry));
	ft_strlcpy(name_entry, "NOVALUE", sizeof(name_entry));
	envp[0] = user_entry;
	envp[1] = empty_entry;
	envp[2] = name_entry;
	envp[3] = NULL;
	env = NULL;
	if (env_init(&env, envp))
		return (report_result("env_init", 0));
	user_entry[5] = 'X';
	if (!env || !strings_equal(env_get(env, "USER"), "alice"))
		return (report_result("env_init", 0));
	if (!strings_equal(env_get(env, "EMPTY"), ""))
		return (env_free(env), report_result("env_init", 0));
	if (env_get(env, "NOVALUE") || env_count(env) != 3)
		return (env_free(env), report_result("env_init", 0));
	env_free(env);
	return (report_result("env_init", 1));
}

int	test_env_set_insert(void)
{
	t_env	*env;
	int		status;

	env = NULL;
	status = env_set(&env, "PATH", "/bin");
	if (status || env_count(env) != 1)
		return (report_result("env_set_insert", 0));
	if (!strings_equal(env_get(env, "PATH"), "/bin"))
		return (env_free(env), report_result("env_set_insert", 0));
	if (!strings_equal(env->key, "PATH"))
		return (env_free(env), report_result("env_set_insert", 0));
	env_free(env);
	return (report_result("env_set_insert", 1));
}

int	test_env_set_update(void)
{
	t_env	*env;
	char	value[8];
	int		status;

	env = NULL;
	ft_strlcpy(value, "second", sizeof(value));
	status = env_set(&env, "PATH", "first");
	status += env_set(&env, "PATH", value);
	value[0] = 'X';
	if (status || env_count(env) != 1)
		return (report_result("env_set_update", 0));
	if (!strings_equal(env_get(env, "PATH"), "second"))
		return (env_free(env), report_result("env_set_update", 0));
	env_free(env);
	return (report_result("env_set_update", 1));
}
