/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_test_case_redir.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 21:15:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 21:15:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_test.h"

static t_redir	*link_redirs(t_redir *first, t_redir *second)
{
	first->next = second;
	return (first);
}

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

static void	setup_external_case(t_exec_case *case_data, char *cmd,
		char *arg1, char *arg2)
{
	init_exec_case(case_data, NULL, 0, STDOUT_FILENO);
	case_data->envp[0] = "PATH=/bin:/usr/bin";
	case_data->argv[0] = cmd;
	case_data->argv[1] = arg1;
	case_data->argv[2] = arg2;
}

int	test_exec_redir_stdout_truncate(void)
{
	char		buffer[64];
	t_exec_case	case_data;
	t_redir		redir;
	int			status;

	setup_external_case(&case_data, "printf", "alpha", NULL);
	redir.type = TK_REDIR_OUT;
	redir.file = "executor_out.txt";
	redir.next = NULL;
	case_data.cmd.redirs = &redir;
	unlink(redir.file);
	init_test_shell(&case_data.shell, case_data.envp);
	status = exec_simple_cmd(&case_data.cmd, &case_data.shell);
	read_test_file(redir.file, buffer, sizeof(buffer));
	unlink(redir.file);
	cleanup_exec_case(&case_data);
	return (report_file_case("exec_redir_stdout_truncate",
			"printf alpha > executor_out.txt", 0, status, redir.file, "alpha",
			buffer));
}

int	test_exec_redir_stdout_append(void)
{
	char		buffer[64];
	t_exec_case	case_data;
	t_redir		redir;
	int			status;

	create_test_file("executor_append.txt", 0644, "base");
	setup_external_case(&case_data, "printf", "plus", NULL);
	redir.type = TK_APPEND;
	redir.file = "executor_append.txt";
	redir.next = NULL;
	case_data.cmd.redirs = &redir;
	init_test_shell(&case_data.shell, case_data.envp);
	status = exec_simple_cmd(&case_data.cmd, &case_data.shell);
	read_test_file(redir.file, buffer, sizeof(buffer));
	unlink(redir.file);
	cleanup_exec_case(&case_data);
	return (report_file_case("exec_redir_stdout_append",
			"printf plus >> executor_append.txt", 0, status, redir.file,
			"baseplus", buffer));
}

int	test_exec_redir_stdin(void)
{
	char		buffer[64];
	t_exec_case	case_data;
	t_capture_expect	expect;
	t_redir		redir;

	create_test_file("executor_in.txt", 0644, "fromfile\n");
	init_exec_case(&case_data, buffer, sizeof(buffer), STDOUT_FILENO);
	case_data.envp[0] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "cat";
	redir.type = TK_REDIR_IN;
	redir.file = "executor_in.txt";
	redir.next = NULL;
	case_data.cmd.redirs = &redir;
	expect.cmdline = "cat < executor_in.txt";
	expect.stream = "stdout";
	expect.expected_status = 0;
	expect.expected_output = "fromfile\n";
	init_test_shell(&case_data.shell, case_data.envp);
	capture_exec_output(&case_data.cmd, &case_data.shell, &case_data.capture);
	unlink(redir.file);
	cleanup_exec_case(&case_data);
	return (report_capture_case("exec_redir_stdin", &expect,
			&case_data.capture));
}

int	test_exec_redir_left_to_right(void)
{
	char		buf1[16];
	char		buf2[16];
	char		*paths[2];
	char		*expected[2];
	char		*actual[2];
	t_exec_case	case_data;
	t_redir		first;
	t_redir		second;
	int			status;

	setup_external_case(&case_data, "printf", "xy", NULL);
	first.type = TK_REDIR_OUT;
	first.file = "executor_left1.txt";
	second.type = TK_REDIR_OUT;
	second.file = "executor_left2.txt";
	second.next = NULL;
	case_data.cmd.redirs = link_redirs(&first, &second);
	unlink(first.file);
	unlink(second.file);
	init_test_shell(&case_data.shell, case_data.envp);
	status = exec_simple_cmd(&case_data.cmd, &case_data.shell);
	read_test_file(first.file, buf1, sizeof(buf1));
	read_test_file(second.file, buf2, sizeof(buf2));
	paths[0] = first.file;
	paths[1] = second.file;
	expected[0] = "";
	expected[1] = "xy";
	actual[0] = buf1;
	actual[1] = buf2;
	unlink(first.file);
	unlink(second.file);
	cleanup_exec_case(&case_data);
	return (report_dual_file_case("exec_redir_left_to_right",
			"printf xy > executor_left1.txt > executor_left2.txt", 0, status,
			paths, expected, actual));
}

int	test_exec_redir_heredoc_unsupported(void)
{
	char				buffer[128];
	t_exec_case			case_data;
	t_capture_expect	expect;
	t_redir				redir;

	init_exec_case(&case_data, buffer, sizeof(buffer), STDERR_FILENO);
	case_data.envp[0] = "PATH=/bin:/usr/bin";
	case_data.argv[0] = "cat";
	redir.type = TK_HEREDOC;
	redir.file = "EOF";
	redir.next = NULL;
	case_data.cmd.redirs = &redir;
	expect.cmdline = "cat << EOF";
	expect.stream = "stderr";
	expect.expected_status = 1;
	expect.expected_output = "minishell: heredoc not supported yet\n";
	init_test_shell(&case_data.shell, case_data.envp);
	capture_exec_output(&case_data.cmd, &case_data.shell, &case_data.capture);
	cleanup_exec_case(&case_data);
	return (report_capture_case("exec_redir_heredoc_unsupported", &expect,
			&case_data.capture));
}
