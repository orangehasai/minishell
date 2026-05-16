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
	int		success;

	env = NULL;
	success = (env_set(&env, "NAME_ONLY", NULL) == 0);
	node = env_find(env, "NAME_ONLY");
	success = success && node && (node->value == NULL);
	success = success && (env_count(env) == 1);
	print_env_case("env_set_null_value", "env_set(NAME_ONLY, NULL)");
	print_env_int("expected_count", 1);
	print_env_int("actual_count", env_count(env));
	print_env_text("expected_NAME_ONLY", NULL);
	print_env_text("actual_NAME_ONLY", node ? node->value : NULL);
	env_free(env);
	return (report_result("env_set_null_value", success));
}

int	test_env_unset(void)
{
	char	*envp[4];
	t_env	*env;
	int		success;

	envp[0] = "A=1";
	envp[1] = "B=2";
	envp[2] = "C=3";
	envp[3] = NULL;
	env = NULL;
	success = (env_init(&env, envp) == 0);
	env_unset(&env, "A");
	env_unset(&env, "B");
	env_unset(&env, "MISSING");
	success = success && env && (env_count(env) == 1);
	success = success && strings_equal(env->key, "C");
	success = success && strings_equal(env->value, "3");
	print_env_case("env_unset", "unset A, B, MISSING from [A=1, B=2, C=3]");
	print_env_int("expected_count", 1);
	print_env_int("actual_count", env_count(env));
	print_env_text("expected_last_key", "C");
	print_env_text("actual_last_key", env ? env->key : NULL);
	print_env_text("expected_last_value", "3");
	print_env_text("actual_last_value", env ? env->value : NULL);
	env_free(env);
	return (report_result("env_unset", success));
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
	print_env_case("env_invalid_args", "NULL and invalid argument handling");
	print_env_flag("env_get(NULL, A) == NULL", env_get(NULL, "A") == NULL);
	print_env_flag("env_init(NULL, NULL) == 1", env_init(NULL, NULL) == 1);
	print_env_flag("env_init(&env, NULL) == 0", env_init(&env, NULL) == 0);
	print_env_flag("env_set(NULL, A, 1) == 1", env_set(NULL, "A", "1") == 1);
	print_env_flag("env_set(&env, NULL, 1) == 1", env_set(&env, NULL, "1") == 1);
	print_env_flag("env_unset(NULL, A) == 1", env_unset(NULL, "A") == 1);
	print_env_flag("env_unset(&env, NULL) == 1", env_unset(&env, NULL) == 1);
	env_free(env);
	return (report_result("env_invalid_args", success));
}
