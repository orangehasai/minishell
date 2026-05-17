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

int	test_exec_builtin_unset(void)
{
	char		buffer[64];
	t_exec_case	case_data;
	int			success;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDOUT_FILENO);
	case_data.envp[0] = "A=10";
	case_data.envp[1] = "B=20";
	case_data.envp[2] = "C=30";
	case_data.envp[3] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "unset";
	case_data.argv[1] = "B";
	case_data.argv[2] = "C";
	case_data.argv[3] = "NON_EXISTENT";
	if (init_test_shell(&case_data.shell, case_data.envp)
		|| capture_exec_output(&case_data.cmd, &case_data.shell,
			&case_data.capture))
	{
		case_data.capture.status = -1;
		ft_strlcpy(buffer, "capture failed", sizeof(buffer));
	}
	success = (case_data.capture.status == 0 && strings_equal(buffer, "")
			&& strings_equal(env_get(case_data.shell.env, "A"), "10")
			&& env_get(case_data.shell.env, "B") == NULL
			&& env_get(case_data.shell.env, "C") == NULL);
	cleanup_exec_case(&case_data);
	return (report_result("exec_builtin_unset", success));
}

int	test_exec_builtin_unset_invalid_identifier(void)
{
	char		buffer[128];
	t_exec_case	case_data;
	int			success;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDERR_FILENO);
	case_data.envp[0] = "A=10";
	case_data.envp[1] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "unset";
	case_data.argv[1] = "1VAR";
	if (init_test_shell(&case_data.shell, case_data.envp)
		|| capture_exec_output(&case_data.cmd, &case_data.shell,
			&case_data.capture))
	{
		case_data.capture.status = -1;
		ft_strlcpy(buffer, "capture failed", sizeof(buffer));
	}
	success = (case_data.capture.status == 1
			&& strings_equal(buffer,
				"minishell: unset: `1VAR': not a valid identifier\n")
			&& strings_equal(env_get(case_data.shell.env, "A"), "10"));
	cleanup_exec_case(&case_data);
	return (report_result("exec_builtin_unset_invalid", success));
}

int	test_exec_builtin_unset_partial_error(void)
{
	char		buffer[128];
	t_exec_case	case_data;
	int			success;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDERR_FILENO);
	case_data.envp[0] = "D=100";
	case_data.envp[1] = "E=200";
	case_data.envp[2] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "unset";
	case_data.argv[1] = "D";
	case_data.argv[2] = "2ERROR";
	case_data.argv[3] = "E";
	if (init_test_shell(&case_data.shell, case_data.envp)
		|| capture_exec_output(&case_data.cmd, &case_data.shell,
			&case_data.capture))
	{
		case_data.capture.status = -1;
		ft_strlcpy(buffer, "capture failed", sizeof(buffer));
	}
	success = (case_data.capture.status == 1
			&& strings_equal(buffer,
				"minishell: unset: `2ERROR': not a valid identifier\n")
			&& env_get(case_data.shell.env, "D") == NULL
			&& env_get(case_data.shell.env, "E") == NULL);
	cleanup_exec_case(&case_data);
	return (report_result("exec_builtin_unset_partial_error", success));
}

int	test_exec_unset_path_then_ls(void)
{
	char		buffer[128];
	t_exec_case	case_data;
	int			unset_status;
	int			success;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDOUT_FILENO);
	case_data.envp[0] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "unset";
	case_data.argv[1] = "PATH";
	case_data.argv[2] = NULL;
	if (init_test_shell(&case_data.shell, case_data.envp)
		|| capture_exec_output(&case_data.cmd, &case_data.shell,
			&case_data.capture))
	{
		case_data.capture.status = -1;
		ft_strlcpy(buffer, "capture failed", sizeof(buffer));
	}
	unset_status = case_data.capture.status;
	case_data.capture.fd = STDERR_FILENO;
	case_data.argv[0] = "ls";
	case_data.argv[1] = NULL;
	capture_exec_output(&case_data.cmd, &case_data.shell, &case_data.capture);
	success = (unset_status == 0 && env_get(case_data.shell.env, "PATH") == NULL
			&& case_data.capture.status == 127
			&& (strings_equal(buffer, "minishell: ls: command not found\n")
				|| strings_equal(buffer, "minishell: ls: No such file or directory\n")));
	cleanup_exec_case(&case_data);
	return (report_result("exec_unset_path_then_ls", success));
}
