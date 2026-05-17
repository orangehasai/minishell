/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_test_case_builtin_redir.c                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 23:30:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 23:30:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_test.h"

static int	read_test_file(char *path, char *buffer, size_t size)
{
	ssize_t	bytes;
	int		fd;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (1);
	bytes = read(fd, buffer, size - 1);
	close(fd);
	if (bytes < 0)
		return (1);
	buffer[bytes] = '\0';
	return (0);
}

static int	run_file_case(t_exec_case *case_data, t_redir *redir, char *path,
		char *buffer)
{
	int	status;

	unlink(path);
	case_data->cmd.redirs = redir;
	init_test_shell(&case_data->shell, case_data->envp);
	status = exec_simple_cmd(&case_data->cmd, &case_data->shell);
	if (read_test_file(path, buffer, 512))
		ft_strlcpy(buffer, "read failed", 512);
	unlink(path);
	return (status);
}

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

int	test_exec_builtin_redir_echo(void)
{
	char		buffer[512];
	t_exec_case	case_data;
	t_redir		redir;
	int			status;

	init_exec_case(&case_data, NULL, 0, STDOUT_FILENO);
	case_data.envp[0] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "echo";
	case_data.argv[1] = "hello";
	redir.type = TK_REDIR_OUT;
	redir.file = "executor_builtin_echo.txt";
	redir.next = NULL;
	status = run_file_case(&case_data, &redir, redir.file, buffer);
	cleanup_exec_case(&case_data);
	return (report_file_case("exec_builtin_redir_echo",
			"echo hello > executor_builtin_echo.txt", 0, status, redir.file,
			"hello\n", buffer));
}

int	test_exec_builtin_redir_pwd(void)
{
	char		buffer[512];
	char		*expected;
	t_exec_case	case_data;
	t_redir		redir;
	int			status;
	int			success;

	init_exec_case(&case_data, NULL, 0, STDOUT_FILENO);
	case_data.envp[0] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "pwd";
	redir.type = TK_REDIR_OUT;
	redir.file = "executor_builtin_pwd.txt";
	redir.next = NULL;
	expected = build_pwd_line();
	status = run_file_case(&case_data, &redir, redir.file, buffer);
	success = report_file_case("exec_builtin_redir_pwd",
			"pwd > executor_builtin_pwd.txt", 0, status, redir.file,
			expected, buffer);
	free(expected);
	cleanup_exec_case(&case_data);
	return (success);
}

int	test_exec_builtin_redir_env(void)
{
	char		buffer[512];
	t_exec_case	case_data;
	t_redir		redir;
	int			status;

	init_exec_case(&case_data, NULL, 0, STDOUT_FILENO);
	case_data.envp[0] = "USER=alice";
	case_data.envp[1] = "EMPTY=";
	case_data.envp[2] = "NAME_ONLY";
	case_data.envp[3] = "PATH=/bin:/usr/bin";
	case_data.envp[4] = "TERM=xterm-256color";
	case_data.argv[0] = "env";
	redir.type = TK_REDIR_OUT;
	redir.file = "executor_builtin_env.txt";
	redir.next = NULL;
	status = run_file_case(&case_data, &redir, redir.file, buffer);
	cleanup_exec_case(&case_data);
	return (report_file_case("exec_builtin_redir_env",
			"env > executor_builtin_env.txt", 0, status, redir.file,
			"USER=alice\nEMPTY=\nPATH=/bin:/usr/bin\nTERM=xterm-256color\n",
			buffer));
}

int	test_exec_builtin_redir_cd(void)
{
	char		buffer[512];
	char		*values[4];
	t_exec_case	case_data;
	t_cd_expect	expect;
	t_redir		redir;
	int			status;
	int			success;

	ft_bzero(values, sizeof(values));
	init_exec_case(&case_data, NULL, 0, STDOUT_FILENO);
	values[0] = getcwd(NULL, 0);
	if (values[0])
		values[1] = ft_strjoin("PWD=", values[0]);
	if (values[0])
		values[2] = ft_strjoin(values[0], "/executor_builtin_cd_dir");
	if (values[0])
		values[3] = ft_strjoin(values[0], "/executor_builtin_cd.txt");
	if (!values[0] || !values[1] || !values[2] || !values[3]
		|| (rmdir("executor_builtin_cd_dir") == -1 && errno != ENOENT)
		|| mkdir("executor_builtin_cd_dir", 0755) == -1)
		return (free(values[0]), free(values[1]), free(values[2]),
			free(values[3]), 0);
	case_data.envp[0] = "PATH=/bin:/usr/bin";
	case_data.envp[1] = values[1];
	case_data.argv[0] = "cd";
	case_data.argv[1] = "executor_builtin_cd_dir";
	redir.type = TK_REDIR_OUT;
	redir.file = values[3];
	redir.next = NULL;
	status = run_file_case(&case_data, &redir, redir.file, buffer);
	expect.cmdline = "cd executor_builtin_cd_dir > executor_builtin_cd.txt";
	expect.expected_status = 0;
	expect.oldpwd = values[0];
	expect.expected_pwd = values[2];
	success = report_cd_file_case("exec_builtin_redir_cd", expect.cmdline,
			status, &expect, redir.file, "", buffer, &case_data.shell);
	if (values[0])
		chdir(values[0]);
	rmdir("executor_builtin_cd_dir");
	free(values[0]);
	free(values[1]);
	free(values[2]);
	free(values[3]);
	cleanup_exec_case(&case_data);
	return (success);
}
