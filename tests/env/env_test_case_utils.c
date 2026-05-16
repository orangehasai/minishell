/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_test_case_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 00:12:12 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/09 01:09:30 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env_test.h"

int	test_env_to_array(void)
{
	char	*envp[4];
	t_env	*env;
	t_env	*node;
	char	**array;
	int		success;

	envp[0] = "NAME_ONLY";
	envp[1] = "PATH=/bin";
	envp[2] = "EMPTY=";
	envp[3] = NULL;
	env = NULL;
	success = (env_init(&env, envp) == 0);
	array = env_to_array(env);
	node = env_find(env, "PATH");
	node->value[0] = 'X';
	success = success && array && (env_array_len(array) == 2);
	success = success && strings_equal(array[0], "PATH=/bin");
	success = success && strings_equal(array[1], "EMPTY=");
	print_env_case("env_to_array", "env_to_array([NAME_ONLY, PATH=/bin, EMPTY=])");
	print_env_int("expected_length", 2);
	print_env_int("actual_length", env_array_len(array));
	print_env_text("expected_array[0]", "PATH=/bin");
	print_env_text("actual_array[0]", array ? array[0] : NULL);
	print_env_text("expected_array[1]", "EMPTY=");
	print_env_text("actual_array[1]", array ? array[1] : NULL);
	env_free(env);
	free_env_array(array);
	return (report_result("env_to_array", success));
}

int	test_split_key_value(void)
{
	char	*key;
	char	*value;
	int		status;
	int		success;

	success = 1;
	status = split_key_value("PATH=/bin:/usr/bin", &key, &value);
	success = success && (status == 0);
	success = success && strings_equal(key, "PATH");
	success = success && strings_equal(value, "/bin:/usr/bin");
	print_env_case("split_key_value", "PATH=/bin:/usr/bin, NAME_ONLY, EMPTY=");
	print_env_text("case1_expected_key", "PATH");
	print_env_text("case1_actual_key", key);
	print_env_text("case1_expected_value", "/bin:/usr/bin");
	print_env_text("case1_actual_value", value);
	free(key);
	free(value);
	status = split_key_value("NAME_ONLY", &key, &value);
	success = success && (status == 0);
	success = success && strings_equal(key, "NAME_ONLY");
	success = success && (value == NULL);
	print_env_text("case2_expected_key", "NAME_ONLY");
	print_env_text("case2_actual_key", key);
	print_env_text("case2_expected_value", NULL);
	print_env_text("case2_actual_value", value);
	free(key);
	status = split_key_value("EMPTY=", &key, &value);
	success = success && (status == 0);
	success = success && strings_equal(key, "EMPTY");
	success = success && strings_equal(value, "");
	print_env_text("case3_expected_key", "EMPTY");
	print_env_text("case3_actual_key", key);
	print_env_text("case3_expected_value", "");
	print_env_text("case3_actual_value", value);
	free(key);
	free(value);
	return (report_result("split_key_value", success));
}

int	test_env_print_export(void)
{
	char	buffer[128];
	char	*envp[4];
	t_env	*env;
	int		status;
	int		success;

	envp[0] = "Z=1";
	envp[1] = "A";
	envp[2] = "EMPTY=";
	envp[3] = NULL;
	env = NULL;
	success = (env_init(&env, envp) == 0);
	status = capture_export_output(env, buffer, sizeof(buffer));
	success = success && (status == 0);
	success = success && strings_equal(buffer,
			"declare -x A\n"
			"declare -x EMPTY=\"\"\n"
			"declare -x Z=\"1\"\n");
	print_env_case("env_print_export", "env_print_export([Z=1, A, EMPTY=])");
	print_env_int("expected_status", 0);
	print_env_int("actual_status", status);
	print_env_text("expected_output",
		"declare -x A\n"
		"declare -x EMPTY=\"\"\n"
		"declare -x Z=\"1\"\n");
	print_env_text("actual_output", buffer);
	env_free(env);
	return (report_result("env_print_export", success));
}
