/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_test_case_builtin.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 18:35:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 18:35:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_test.h"

static char	*build_pwd_line(void)
{
	char	*cwd;
	char	*line;

	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (NULL);
	line = ft_strjoin(cwd, "\n");
	free(cwd);
	return (line);
}

int	test_exec_builtin_pwd(void)
{
	char		buffer[512];
	char		*expected;
	t_exec_case	case_data;
	t_capture_expect	expect;
	int			success;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDOUT_FILENO);
	case_data.envp[0] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "pwd";
	expected = build_pwd_line();
	if (!expected || init_test_shell(&case_data.shell, case_data.envp)
		|| capture_exec_output(&case_data.cmd, &case_data.shell,
			&case_data.capture))
	{
		case_data.capture.status = -1;
		ft_strlcpy(buffer, "capture failed", sizeof(buffer));
	}
	expect.cmdline = "pwd";
	expect.stream = "stdout";
	expect.expected_status = 0;
	expect.expected_output = expected;
	success = report_capture_case("exec_builtin_pwd", &expect,
			&case_data.capture);
	free(expected);
	cleanup_exec_case(&case_data);
	return (success);
}

int	test_exec_builtin_env(void)
{
	char		buffer[256];
	t_exec_case	case_data;
	t_capture_expect	expect;
	int			success;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDOUT_FILENO);
	case_data.envp[0] = "USER=alice";
	case_data.envp[1] = "EMPTY=";
	case_data.envp[2] = "NAME_ONLY";
	case_data.envp[3] = "PATH=/bin:/usr/bin";
	case_data.envp[4] = "TERM=xterm-256color";
	case_data.argv[0] = "env";
	expect.cmdline = "env";
	expect.stream = "stdout";
	expect.expected_status = 0;
	expect.expected_output = "USER=alice\nEMPTY=\nPATH=/bin:/usr/bin\nTERM=xterm-256color\n";
	if (init_test_shell(&case_data.shell, case_data.envp)
		|| capture_exec_output(&case_data.cmd, &case_data.shell,
			&case_data.capture))
	{
		case_data.capture.status = -1;
		ft_strlcpy(buffer, "capture failed", sizeof(buffer));
	}
	success = report_capture_case("exec_builtin_env", &expect,
			&case_data.capture);
	cleanup_exec_case(&case_data);
	return (success);
}

int	test_exec_builtin_env_rejects_args(void)
{
	char		buffer[256];
	t_exec_case	case_data;
	t_capture_expect	expect;
	int			success;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDERR_FILENO);
	case_data.envp[0] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "env";
	case_data.argv[1] = "extra";
	expect.cmdline = "env extra";
	expect.stream = "stderr";
	expect.expected_status = 1;
	expect.expected_output = "minishell: env: options or arguments are not supported\n";
	if (init_test_shell(&case_data.shell, case_data.envp)
		|| capture_exec_output(&case_data.cmd, &case_data.shell,
			&case_data.capture))
	{
		case_data.capture.status = -1;
		ft_strlcpy(buffer, "capture failed", sizeof(buffer));
	}
	success = report_capture_case("exec_builtin_env_rejects_args", &expect,
			&case_data.capture);
	cleanup_exec_case(&case_data);
	return (success);
}
