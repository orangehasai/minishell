/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_test_case_cd.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 18:35:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 18:35:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_test.h"

static char	*join_dir(char *left, char *right)
{
	char	*with_slash;
	char	*joined;

	with_slash = ft_strjoin(left, "/");
	if (!with_slash)
		return (NULL);
	joined = ft_strjoin(with_slash, right);
	free(with_slash);
	return (joined);
}

static int	finish_cd_test(t_exec_case *case_data, char **values, int success)
{
	if (values[0])
		chdir(values[0]);
	rmdir("executor_cd_dir");
	free(values[0]);
	free(values[1]);
	free(values[2]);
	cleanup_exec_case(case_data);
	return (success);
}

static int	prepare_cd_case(t_exec_case *case_data, char *pwd_entry)
{
	case_data->envp[0] = "PATH=/bin:/usr/bin";
	case_data->envp[1] = pwd_entry;
	case_data->argv[0] = "cd";
	case_data->argv[1] = "executor_cd_dir";
	if ((rmdir(case_data->argv[1]) == -1 && errno != ENOENT)
		|| mkdir(case_data->argv[1], 0755) == -1)
		return (1);
	return (init_test_shell(&case_data->shell, case_data->envp));
}

static int	prepare_cd_case_with_target(t_exec_case *case_data, char *pwd_entry,
		char *target)
{
	case_data->envp[0] = "PATH=/bin:/usr/bin";
	case_data->envp[1] = pwd_entry;
	case_data->argv[0] = "cd";
	case_data->argv[1] = target;
	return (init_test_shell(&case_data->shell, case_data->envp));
}

int	test_exec_builtin_cd(void)
{
	char		*values[3];
	t_exec_case	case_data;
	t_cd_expect	expect;
	int			status;
	int			success;

	ft_bzero(values, sizeof(values));
	init_exec_case(&case_data, NULL, 0, STDOUT_FILENO);
	values[0] = getcwd(NULL, 0);
	if (values[0])
		values[1] = ft_strjoin("PWD=", values[0]);
	if (values[0])
		values[2] = join_dir(values[0], "executor_cd_dir");
	if (!values[0] || !values[1] || !values[2]
		|| prepare_cd_case(&case_data, values[1]))
		return (finish_cd_test(&case_data, values, 0));
	status = execute(&case_data.cmd, &case_data.shell);
	expect.cmdline = "cd executor_cd_dir";
	expect.expected_status = 0;
	expect.oldpwd = values[0];
	expect.expected_pwd = values[2];
	success = report_cd_case("exec_builtin_cd", status, &expect,
			&case_data.shell);
	return (finish_cd_test(&case_data, values, success));
}

int	test_exec_builtin_cd_absolute(void)
{
	char		*values[3];
	t_exec_case	case_data;
	t_cd_expect	expect;
	int			status;
	int			success;

	ft_bzero(values, sizeof(values));
	init_exec_case(&case_data, NULL, 0, STDOUT_FILENO);
	values[0] = getcwd(NULL, 0);
	if (values[0])
		values[1] = ft_strjoin("PWD=", values[0]);
	if (values[0])
		values[2] = join_dir(values[0], "executor_cd_abs_dir");
	if (!values[0] || !values[1] || !values[2]
		|| (rmdir("executor_cd_abs_dir") == -1 && errno != ENOENT)
		|| mkdir("executor_cd_abs_dir", 0755) == -1
		|| prepare_cd_case_with_target(&case_data, values[1], values[2]))
		return (finish_cd_test(&case_data, values, 0));
	status = execute(&case_data.cmd, &case_data.shell);
	expect.cmdline = "cd /abs/path/to/executor_cd_abs_dir";
	expect.expected_status = 0;
	expect.oldpwd = values[0];
	expect.expected_pwd = values[2];
	success = report_cd_case("exec_builtin_cd_absolute", status, &expect,
			&case_data.shell);
	if (values[0])
		chdir(values[0]);
	rmdir("executor_cd_abs_dir");
	return (finish_cd_test(&case_data, values, success));
}

int	test_exec_builtin_cd_rejects_args(void)
{
	char		buffer[256];
	t_exec_case	case_data;
	t_capture_expect	expect;
	int			success;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDERR_FILENO);
	case_data.envp[0] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "cd";
	case_data.argv[1] = "a";
	case_data.argv[2] = "b";
	expect.cmdline = "cd a b";
	expect.stream = "stderr";
	expect.expected_status = 1;
	expect.expected_output = "minishell: cd: too many arguments\n";
	if (init_test_shell(&case_data.shell, case_data.envp)
		|| capture_exec_output(&case_data.cmd, &case_data.shell,
			&case_data.capture))
	{
		case_data.capture.status = -1;
		ft_strlcpy(buffer, "capture failed", sizeof(buffer));
	}
	success = report_capture_case("exec_builtin_cd_rejects_args", &expect,
			&case_data.capture);
	cleanup_exec_case(&case_data);
	return (success);
}
