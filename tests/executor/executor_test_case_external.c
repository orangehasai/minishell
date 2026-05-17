/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_test_case_external.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 18:35:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 18:35:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_test.h"

int	test_exec_external_echo(void)
{
	char		buffer[32];
	t_exec_case	case_data;
	t_capture_expect	expect;
	int			success;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDOUT_FILENO);
	case_data.envp[0] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "/bin/echo";
	case_data.argv[1] = "hello";
	expect.cmdline = "/bin/echo hello";
	expect.stream = "stdout";
	expect.expected_status = 0;
	expect.expected_output = "hello\n";
	if (init_test_shell(&case_data.shell, case_data.envp)
		|| capture_exec_output(&case_data.cmd, &case_data.shell,
			&case_data.capture))
	{
		case_data.capture.status = -1;
		ft_strlcpy(buffer, "capture failed", sizeof(buffer));
	}
	success = report_capture_case("exec_external_echo", &expect,
			&case_data.capture);
	cleanup_exec_case(&case_data);
	return (success);
}

int	test_exec_missing_command(void)
{
	char		buffer[128];
	t_exec_case	case_data;
	t_capture_expect	expect;
	int			success;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDERR_FILENO);
	case_data.envp[0] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "missing_command_xyz";
	expect.cmdline = "missing_command_xyz";
	expect.stream = "stderr";
	expect.expected_status = 127;
	expect.expected_output = "minishell: missing_command_xyz: command not found\n";
	if (init_test_shell(&case_data.shell, case_data.envp)
		|| capture_exec_output(&case_data.cmd, &case_data.shell,
			&case_data.capture))
	{
		case_data.capture.status = -1;
		ft_strlcpy(buffer, "capture failed", sizeof(buffer));
	}
	success = report_capture_case("exec_missing_cmd", &expect,
			&case_data.capture);
	cleanup_exec_case(&case_data);
	return (success);
}

int	test_exec_path_search_printf(void)
{
	char		buffer[32];
	t_exec_case	case_data;
	t_capture_expect	expect;
	int			success;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDOUT_FILENO);
	case_data.envp[0] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "printf";
	case_data.argv[1] = "search";
	expect.cmdline = "printf search";
	expect.stream = "stdout";
	expect.expected_status = 0;
	expect.expected_output = "search";
	if (init_test_shell(&case_data.shell, case_data.envp)
		|| capture_exec_output(&case_data.cmd, &case_data.shell,
			&case_data.capture))
	{
		case_data.capture.status = -1;
		ft_strlcpy(buffer, "capture failed", sizeof(buffer));
	}
	success = report_capture_case("exec_path_search_printf", &expect,
			&case_data.capture);
	cleanup_exec_case(&case_data);
	return (success);
}

int	test_exec_builtin_export(void)
{
	char		buffer[64];
	t_exec_case	case_data;
	int			success;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDOUT_FILENO);
	case_data.envp[0] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "export";
	case_data.argv[1] = "ABC=xyz";
	if (init_test_shell(&case_data.shell, case_data.envp)
		|| capture_exec_output(&case_data.cmd, &case_data.shell,
			&case_data.capture))
	{
		case_data.capture.status = -1;
		ft_strlcpy(buffer, "capture failed", sizeof(buffer));
	}
	success = (case_data.capture.status == 0 && strings_equal(buffer, "")
			&& strings_equal(env_get(case_data.shell.env, "ABC"), "xyz"));
	cleanup_exec_case(&case_data);
	return (report_result("exec_builtin_export", success));
}
