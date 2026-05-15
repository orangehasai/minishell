/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_test_case_ops.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 20:44:09 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/08 20:44:09 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env_test.h"

int	test_env_set_null_value(void)
{
	t_env	*env;
	t_env	*node;

	env = NULL;
	if (env_set(&env, "NAME_ONLY", NULL))
		return (report_result("env_set_null_value", 0));
	node = env_find(env, "NAME_ONLY");
	if (!node || node->value != NULL)
		return (env_free(env), report_result("env_set_null_value", 0));
	if (env_count(env) != 1)
		return (env_free(env), report_result("env_set_null_value", 0));
	env_free(env);
	return (report_result("env_set_null_value", 1));
}

int	test_env_unset(void)
{
	char	*envp[4];
	t_env	*env;

	envp[0] = "A=1";
	envp[1] = "B=2";
	envp[2] = "C=3";
	envp[3] = NULL;
	env = NULL;
	if (env_init(&env, envp))
		return (report_result("env_unset", 0));
	env_unset(&env, "A");
	env_unset(&env, "B");
	env_unset(&env, "MISSING");
	if (!env || env_count(env) != 1)
		return (report_result("env_unset", 0));
	if (!strings_equal(env->key, "C") || !strings_equal(env->value, "3"))
		return (env_free(env), report_result("env_unset", 0));
	env_free(env);
	return (report_result("env_unset", 1));
}

int	test_env_invalid_args(void)
{
	t_env	*env;
	int		success;

	env = NULL;
	success = (env_get(NULL, "A") == NULL);
	success = success && (env_init(NULL, NULL) == 1);
	success = success && (env_init(&env, NULL) == 0);
	success = success && (env == NULL);
	success = success && (env_set(NULL, "A", "1") == 1);
	success = success && (env_set(&env, NULL, "1") == 1);
	success = success && (env_unset(NULL, "A") == 1);
	success = success && (env_unset(&env, NULL) == 1);
	env_free(env);
	return (report_result("env_invalid_args", success));
}
