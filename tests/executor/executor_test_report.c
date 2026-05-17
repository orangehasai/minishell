/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_test_report.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 19:10:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 19:10:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_test.h"

static void	print_escaped_str(char *value)
{
	size_t	index;

	if (!value)
		return ((void)printf("(null)"));
	index = 0;
	while (value[index])
	{
		if (value[index] == '\n')
			printf("\\n");
		else if (value[index] == '\t')
			printf("\\t");
		else if (value[index] == '\"')
			printf("\\\"");
		else
			printf("%c", value[index]);
		index++;
	}
}

static void	print_text_field(char *label, char *value)
{
	printf("  %s: \"", label);
	print_escaped_str(value);
	printf("\"\n");
}

static void	print_int_field(char *label, int value)
{
	printf("  %s: %d\n", label, value);
}

int	report_capture_case(char *name, t_capture_expect *expect,
		t_capture_exec *capture)
{
	int	success;

	success = (capture->status == expect->expected_status
			&& strings_equal(capture->buffer, expect->expected_output));
	printf("CASE: %s\n", name);
	printf("  cmd: %s\n", expect->cmdline);
	printf("  capture: %s\n", expect->stream);
	print_int_field("expected_status", expect->expected_status);
	print_int_field("actual_status", capture->status);
	if (ft_strncmp(expect->stream, "stdout", 7) == 0)
	{
		print_text_field("expected_stdout", expect->expected_output);
		print_text_field("actual_stdout", capture->buffer);
	}
	else
	{
		print_text_field("expected_stderr", expect->expected_output);
		print_text_field("actual_stderr", capture->buffer);
	}
	return (report_result(name, success));
}

int	report_cd_case(char *name, int actual_status, t_cd_expect *expect,
		t_shell *shell)
{
	char	*actual_cwd;
	int		success;

	actual_cwd = getcwd(NULL, 0);
	success = (actual_status == expect->expected_status && actual_cwd
			&& strings_equal(actual_cwd, expect->expected_pwd)
			&& strings_equal(env_get(shell->env, "PWD"), expect->expected_pwd)
			&& strings_equal(env_get(shell->env, "OLDPWD"), expect->oldpwd));
	printf("CASE: %s\n", name);
	printf("  cmd: %s\n", expect->cmdline);
	print_int_field("expected_status", expect->expected_status);
	print_int_field("actual_status", actual_status);
	print_text_field("expected_cwd", expect->expected_pwd);
	print_text_field("actual_cwd", actual_cwd);
	print_text_field("expected_PWD", expect->expected_pwd);
	print_text_field("actual_PWD", env_get(shell->env, "PWD"));
	print_text_field("expected_OLDPWD", expect->oldpwd);
	print_text_field("actual_OLDPWD", env_get(shell->env, "OLDPWD"));
	free(actual_cwd);
	return (report_result(name, success));
}

int	report_cd_file_case(char *name, char *cmdline, int actual_status,
		t_cd_expect *expect, char *path, char *expected, char *actual,
		t_shell *shell)
{
	char	*actual_cwd;
	int		success;

	actual_cwd = getcwd(NULL, 0);
	success = (actual_status == expect->expected_status && actual_cwd
			&& strings_equal(actual_cwd, expect->expected_pwd)
			&& strings_equal(env_get(shell->env, "PWD"), expect->expected_pwd)
			&& strings_equal(env_get(shell->env, "OLDPWD"), expect->oldpwd)
			&& strings_equal(expected, actual));
	printf("CASE: %s\n", name);
	printf("  cmd: %s\n", cmdline);
	print_int_field("expected_status", expect->expected_status);
	print_int_field("actual_status", actual_status);
	print_text_field("expected_cwd", expect->expected_pwd);
	print_text_field("actual_cwd", actual_cwd);
	print_text_field("expected_PWD", expect->expected_pwd);
	print_text_field("actual_PWD", env_get(shell->env, "PWD"));
	print_text_field("expected_OLDPWD", expect->oldpwd);
	print_text_field("actual_OLDPWD", env_get(shell->env, "OLDPWD"));
	print_text_field("path", path);
	print_text_field("expected_file", expected);
	print_text_field("actual_file", actual);
	free(actual_cwd);
	return (report_result(name, success));
}

int	report_file_case(char *name, char *cmdline, int expected_status,
		int actual_status, char *path, char *expected, char *actual)
{
	int	success;

	success = (expected_status == actual_status && strings_equal(expected,
				actual));
	printf("CASE: %s\n", name);
	printf("  cmd: %s\n", cmdline);
	print_int_field("expected_status", expected_status);
	print_int_field("actual_status", actual_status);
	print_text_field("path", path);
	print_text_field("expected_file", expected);
	print_text_field("actual_file", actual);
	return (report_result(name, success));
}

int	report_dual_file_case(char *name, char *cmdline, int expected_status,
		int actual_status, char **paths, char **expected, char **actual)
{
	int	success;

	success = (expected_status == actual_status);
	success = success && strings_equal(expected[0], actual[0]);
	success = success && strings_equal(expected[1], actual[1]);
	printf("CASE: %s\n", name);
	printf("  cmd: %s\n", cmdline);
	print_int_field("expected_status", expected_status);
	print_int_field("actual_status", actual_status);
	print_text_field("path1", paths[0]);
	print_text_field("expected_file1", expected[0]);
	print_text_field("actual_file1", actual[0]);
	print_text_field("path2", paths[1]);
	print_text_field("expected_file2", expected[1]);
	print_text_field("actual_file2", actual[1]);
	return (report_result(name, success));
}
