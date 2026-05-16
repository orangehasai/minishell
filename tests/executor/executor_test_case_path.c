/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_test_case_path.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 19:10:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 19:10:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_test.h"

static int	finish_path_test(t_exec_case *case_data, char *path, int success)
{
	unlink(path);
	cleanup_exec_case(case_data);
	return (success);
}

static int	report_setup_failure(char *name, char *cmdline)
{
	printf("CASE: %s\n", name);
	printf("  cmd: %s\n", cmdline);
	printf("  file setup failed\n");
	return (report_result(name, 0));
}

static void	mark_capture_failure(t_exec_case *case_data, char *buffer,
		size_t size)
{
	case_data->capture.status = -1;
	ft_strlcpy(buffer, "capture failed", size);
}

int	test_exec_permission_denied(void)
{
	char				buffer[128];
	t_exec_case			case_data;
	t_capture_expect	expect;
	int					success;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDERR_FILENO);
	case_data.envp[0] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "./executor_noexec.sh";
	expect.cmdline = "./executor_noexec.sh";
	expect.stream = "stderr";
	expect.expected_status = 126;
	expect.expected_output = "minishell: ./executor_noexec.sh: Permission denied\n";
	if (create_test_file(case_data.argv[0], 0644, "#!/bin/sh\necho denied\n"))
		return (report_setup_failure("exec_permission_denied",
				expect.cmdline));
	if (init_test_shell(&case_data.shell, case_data.envp)
		|| capture_exec_output(&case_data.cmd, &case_data.shell,
			&case_data.capture))
		mark_capture_failure(&case_data, buffer, sizeof(buffer));
	success = report_capture_case("exec_permission_denied", &expect,
			&case_data.capture);
	return (finish_path_test(&case_data, case_data.argv[0], success));
}

int	test_exec_path_empty_entry(void)
{
	char				buffer[32];
	t_exec_case			case_data;
	t_capture_expect	expect;
	int					success;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDOUT_FILENO);
	case_data.envp[0] = "PATH=:/bin:/usr/bin";
	case_data.argv[0] = "executor_current.sh";
	expect.cmdline = "executor_current.sh";
	expect.stream = "stdout";
	expect.expected_status = 0;
	expect.expected_output = "current\n";
	if (create_test_file(case_data.argv[0], 0755, "#!/bin/sh\necho current\n"))
		return (report_setup_failure("exec_path_empty_entry", expect.cmdline));
	if (init_test_shell(&case_data.shell, case_data.envp)
		|| capture_exec_output(&case_data.cmd, &case_data.shell,
			&case_data.capture))
		mark_capture_failure(&case_data, buffer, sizeof(buffer));
	success = report_capture_case("exec_path_empty_entry", &expect,
			&case_data.capture);
	return (finish_path_test(&case_data, case_data.argv[0], success));
}
