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

	envp[0] = "NAME_ONLY";
	envp[1] = "PATH=/bin";
	envp[2] = "EMPTY=";
	envp[3] = NULL;
	env = NULL;
	if (env_init(&env, envp))
		return (report_result("env_to_array", 0));
	array = env_to_array(env);
	node = env_find(env, "PATH");
	node->value[0] = 'X';
	if (!array || env_array_len(array) != 2 || !strings_equal(array[0],
			"PATH=/bin"))
		return (env_free(env), free_env_array(array),
			report_result("env_to_array", 0));
	if (!strings_equal(array[1], "EMPTY="))
		return (env_free(env), free_env_array(array),
			report_result("env_to_array", 0));
	env_free(env);
	free_env_array(array);
	return (report_result("env_to_array", 1));
}

int	test_split_key_value(void)
{
	char	*key;
	char	*value;
	int		status;

	status = split_key_value("PATH=/bin:/usr/bin", &key, &value);
	if (status || !strings_equal(key, "PATH") || !strings_equal(value,
			"/bin:/usr/bin"))
		return (free(key), free(value), report_result("split_key_value", 0));
	free(key);
	free(value);
	status = split_key_value("NAME_ONLY", &key, &value);
	if (status || !strings_equal(key, "NAME_ONLY") || value != NULL)
		return (free(key), report_result("split_key_value", 0));
	free(key);
	status = split_key_value("EMPTY=", &key, &value);
	if (status || !strings_equal(key, "EMPTY") || !strings_equal(value, ""))
		return (free(key), free(value), report_result("split_key_value", 0));
	free(key);
	free(value);
	return (report_result("split_key_value", 1));
}

int	test_env_print_export(void)
{
	char	buffer[128];
	char	*envp[4];
	t_env	*env;
	int		status;

	envp[0] = "Z=1";
	envp[1] = "A";
	envp[2] = "EMPTY=";
	envp[3] = NULL;
	env = NULL;
	if (env_init(&env, envp))
		return (report_result("env_print_export", 0));
	status = capture_export_output(env, buffer, sizeof(buffer));
	if (status)
		return (env_free(env), report_result("env_print_export", 0));
	if (!strings_equal(buffer,
			"declare -x A\n"
			"declare -x EMPTY=\"\"\n"
			"declare -x Z=\"1\"\n"))
		return (env_free(env), report_result("env_print_export", 0));
	env_free(env);
	return (report_result("env_print_export", 1));
}
