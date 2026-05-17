/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_test_case_echo.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 20:45:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 20:45:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_test.h"

static int	run_echo_case(char *name, char **argv, char *cmdline,
		char *expected_output)
{
	char				buffer[128];
	t_exec_case			case_data;
	t_capture_expect	expect;
	int					success;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDOUT_FILENO);
	case_data.envp[0] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = argv[0];
	case_data.argv[1] = argv[1];
	case_data.argv[2] = argv[2];
	case_data.argv[3] = argv[3];
	expect.cmdline = cmdline;
	expect.stream = "stdout";
	expect.expected_status = 0;
	expect.expected_output = expected_output;
	if (init_test_shell(&case_data.shell, case_data.envp)
		|| capture_exec_output(&case_data.cmd, &case_data.shell,
			&case_data.capture))
	{
		case_data.capture.status = -1;
		ft_strlcpy(buffer, "capture failed", sizeof(buffer));
	}
	success = report_capture_case(name, &expect, &case_data.capture);
	cleanup_exec_case(&case_data);
	return (success);
}

int	test_exec_builtin_echo(void)
{
	char	*argv[4];

	argv[0] = "echo";
	argv[1] = "hello";
	argv[2] = "world";
	argv[3] = NULL;
	return (run_echo_case("exec_builtin_echo", argv, "echo hello world",
			"hello world\n"));
}

int	test_exec_builtin_echo_no_newline(void)
{
	char	*argv[4];

	argv[0] = "echo";
	argv[1] = "-n";
	argv[2] = "hello";
	argv[3] = NULL;
	return (run_echo_case("exec_builtin_echo_no_newline", argv,
			"echo -n hello", "hello"));
}

int	test_exec_builtin_echo_multi_n(void)
{
	char	*argv[4];

	argv[0] = "echo";
	argv[1] = "-nnn";
	argv[2] = "hello";
	argv[3] = NULL;
	return (run_echo_case("exec_builtin_echo_multi_n", argv,
			"echo -nnn hello", "hello"));
}

int	test_exec_builtin_echo_invalid_n_flag(void)
{
	char	*argv[4];

	argv[0] = "echo";
	argv[1] = "-nab";
	argv[2] = "hello";
	argv[3] = NULL;
	return (run_echo_case("exec_builtin_echo_invalid_n_flag", argv,
			"echo -nab hello", "-nab hello\n"));
}
