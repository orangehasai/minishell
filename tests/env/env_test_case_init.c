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
	int		success;

	ft_strlcpy(user_entry, "USER=alice", sizeof(user_entry));
	ft_strlcpy(empty_entry, "EMPTY=", sizeof(empty_entry));
	ft_strlcpy(name_entry, "NOVALUE", sizeof(name_entry));
	envp[0] = user_entry;
	envp[1] = empty_entry;
	envp[2] = name_entry;
	envp[3] = NULL;
	env = NULL;
	success = (env_init(&env, envp) == 0);
	user_entry[5] = 'X';
	success = success && env && strings_equal(env_get(env, "USER"), "alice");
	success = success && strings_equal(env_get(env, "EMPTY"), "");
	success = success && (env_get(env, "NOVALUE") == NULL);
	success = success && (env_count(env) == 3);
	print_env_case("env_init", "env_init([USER=alice, EMPTY=, NOVALUE])");
	print_env_text("expected_USER", "alice");
	print_env_text("actual_USER", env_get(env, "USER"));
	print_env_text("expected_EMPTY", "");
	print_env_text("actual_EMPTY", env_get(env, "EMPTY"));
	print_env_text("expected_NOVALUE", NULL);
	print_env_text("actual_NOVALUE", env_get(env, "NOVALUE"));
	print_env_int("expected_count", 3);
	print_env_int("actual_count", env_count(env));
	env_free(env);
	return (report_result("env_init", success));
}

int	test_env_set_insert(void)
{
	t_env	*env;
	int		status;
	int		success;

	env = NULL;
	status = env_set(&env, "PATH", "/bin");
	success = (status == 0);
	success = success && (env_count(env) == 1);
	success = success && strings_equal(env_get(env, "PATH"), "/bin");
	success = success && env && strings_equal(env->key, "PATH");
	print_env_case("env_set_insert", "env_set(PATH=/bin) on empty env");
	print_env_int("expected_status", 0);
	print_env_int("actual_status", status);
	print_env_int("expected_count", 1);
	print_env_int("actual_count", env_count(env));
	print_env_text("expected_PATH", "/bin");
	print_env_text("actual_PATH", env_get(env, "PATH"));
	env_free(env);
	return (report_result("env_set_insert", success));
}

int	test_env_set_update(void)
{
	t_env	*env;
	char	value[8];
	int		status;
	int		success;

	env = NULL;
	ft_strlcpy(value, "second", sizeof(value));
	status = env_set(&env, "PATH", "first");
	status += env_set(&env, "PATH", value);
	value[0] = 'X';
	success = (status == 0);
	success = success && (env_count(env) == 1);
	success = success && strings_equal(env_get(env, "PATH"), "second");
	print_env_case("env_set_update",
		"env_set(PATH=first) then env_set(PATH=second)");
	print_env_int("expected_status", 0);
	print_env_int("actual_status", status);
	print_env_int("expected_count", 1);
	print_env_int("actual_count", env_count(env));
	print_env_text("expected_PATH", "second");
	print_env_text("actual_PATH", env_get(env, "PATH"));
	env_free(env);
	return (report_result("env_set_update", success));
}
