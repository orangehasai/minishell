/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_test_case_pipeline.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/17 00:50:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/17 00:50:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_test.h"

static void	init_cmd(t_cmd *cmd, char **argv, t_redir *redirs, t_cmd *next)
{
	ft_bzero(cmd, sizeof(*cmd));
	cmd->argv = argv;
	cmd->redirs = redirs;
	cmd->next = next;
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

int	test_exec_pipeline_external(void)
{
	char				buffer[256];
	char				*argv1[] = {"printf", "alpha\nbeta\n", NULL};
	char				*argv2[] = {"grep", "beta", NULL};
	t_cmd				cmd1;
	t_cmd				cmd2;
	t_shell				shell;
	t_capture_exec		capture;
	t_capture_expect	expect;

	init_cmd(&cmd2, argv2, NULL, NULL);
	init_cmd(&cmd1, argv1, NULL, &cmd2);
	ft_bzero(&capture, sizeof(capture));
	capture.buffer = buffer;
	capture.size = sizeof(buffer);
	capture.fd = STDOUT_FILENO;
	init_test_shell(&shell, (char *[]){"PATH=/bin:/usr/bin", NULL});
	capture_pipeline_output(&cmd1, &shell, &capture);
	expect.cmdline = "printf 'alpha\\nbeta\\n' | grep beta";
	expect.stream = "stdout";
	expect.expected_status = 0;
	expect.expected_output = "beta\n";
	env_free(shell.env);
	return (report_capture_case("exec_pipeline_external", &expect, &capture));
}

int	test_exec_pipeline_builtin_echo(void)
{
	char				buffer[256];
	char				*argv1[] = {"echo", "hello", NULL};
	char				*argv2[] = {"cat", NULL};
	t_cmd				cmd1;
	t_cmd				cmd2;
	t_shell				shell;
	t_capture_exec		capture;
	t_capture_expect	expect;

	init_cmd(&cmd2, argv2, NULL, NULL);
	init_cmd(&cmd1, argv1, NULL, &cmd2);
	ft_bzero(&capture, sizeof(capture));
	capture.buffer = buffer;
	capture.size = sizeof(buffer);
	capture.fd = STDOUT_FILENO;
	init_test_shell(&shell, (char *[]){"PATH=/bin:/usr/bin", NULL});
	capture_pipeline_output(&cmd1, &shell, &capture);
	expect.cmdline = "echo hello | cat";
	expect.stream = "stdout";
	expect.expected_status = 0;
	expect.expected_output = "hello\n";
	env_free(shell.env);
	return (report_capture_case("exec_pipeline_builtin_echo",
			&expect, &capture));
}

int	test_exec_pipeline_redir(void)
{
	char		buffer[256];
	char		*argv1[] = {"cat", NULL};
	char		*argv2[] = {"grep", "beta", NULL};
	t_cmd		cmd1;
	t_cmd		cmd2;
	t_redir		in_redir;
	t_redir		out_redir;
	t_shell		shell;
	int			status;

	create_test_file("executor_pipe_in.txt", 0644, "alpha\nbeta\n");
	in_redir.type = TK_REDIR_IN;
	in_redir.file = "executor_pipe_in.txt";
	in_redir.next = NULL;
	out_redir.type = TK_REDIR_OUT;
	out_redir.file = "executor_pipe_out.txt";
	out_redir.next = NULL;
	init_cmd(&cmd2, argv2, &out_redir, NULL);
	init_cmd(&cmd1, argv1, &in_redir, &cmd2);
	unlink(out_redir.file);
	init_test_shell(&shell, (char *[]){"PATH=/bin:/usr/bin", NULL});
	status = execute(&cmd1, &shell);
	if (read_test_file(out_redir.file, buffer, sizeof(buffer)))
		ft_strlcpy(buffer, "read failed", sizeof(buffer));
	unlink(in_redir.file);
	unlink(out_redir.file);
	env_free(shell.env);
	return (report_file_case("exec_pipeline_redir",
			"cat < executor_pipe_in.txt | grep beta > executor_pipe_out.txt",
			0, status, out_redir.file, "beta\n", buffer));
}

int	test_exec_pipeline_missing_first(void)
{
	char				buffer[256];
	char				*argv1[] = {"missing_command_xyz", NULL};
	char				*argv2[] = {"cat", NULL};
	t_cmd				cmd1;
	t_cmd				cmd2;
	t_shell				shell;
	t_capture_exec		capture;
	t_capture_expect	expect;

	init_cmd(&cmd2, argv2, NULL, NULL);
	init_cmd(&cmd1, argv1, NULL, &cmd2);
	ft_bzero(&capture, sizeof(capture));
	capture.buffer = buffer;
	capture.size = sizeof(buffer);
	capture.fd = STDERR_FILENO;
	init_test_shell(&shell, (char *[]){"PATH=/bin:/usr/bin", NULL});
	capture_pipeline_output(&cmd1, &shell, &capture);
	expect.cmdline = "missing_command_xyz | cat";
	expect.stream = "stderr";
	expect.expected_status = 0;
	expect.expected_output = "minishell: missing_command_xyz: command not found\n";
	env_free(shell.env);
	return (report_capture_case("exec_pipeline_missing_first",
			&expect, &capture));
}

int	test_exec_pipeline_missing_last(void)
{
	char				buffer[256];
	char				*argv1[] = {"printf", "hello", NULL};
	char				*argv2[] = {"missing_command_xyz", NULL};
	t_cmd				cmd1;
	t_cmd				cmd2;
	t_shell				shell;
	t_capture_exec		capture;
	t_capture_expect	expect;

	init_cmd(&cmd2, argv2, NULL, NULL);
	init_cmd(&cmd1, argv1, NULL, &cmd2);
	ft_bzero(&capture, sizeof(capture));
	capture.buffer = buffer;
	capture.size = sizeof(buffer);
	capture.fd = STDERR_FILENO;
	init_test_shell(&shell, (char *[]){"PATH=/bin:/usr/bin", NULL});
	capture_pipeline_output(&cmd1, &shell, &capture);
	expect.cmdline = "printf hello | missing_command_xyz";
	expect.stream = "stderr";
	expect.expected_status = 127;
	expect.expected_output = "minishell: missing_command_xyz: command not found\n";
	env_free(shell.env);
	return (report_capture_case("exec_pipeline_missing_last",
			&expect, &capture));
}

int	test_exec_pipeline_wait_scope(void)
{
	char	*argv1[] = {"printf", "hello", NULL};
	char	*argv2[] = {"sleep", "1", NULL};
	t_cmd	cmd1;
	t_cmd	cmd2;
	t_shell	shell;
	pid_t	helper_pid;
	int		helper_status;
	int		status;
	int		success;

	init_cmd(&cmd2, argv2, NULL, NULL);
	init_cmd(&cmd1, argv1, NULL, &cmd2);
	helper_pid = fork();
	if (helper_pid == 0)
		exit(42);
	init_test_shell(&shell, (char *[]){"PATH=/bin:/usr/bin", NULL});
	status = execute(&cmd1, &shell);
	success = (status == 0 && waitpid(helper_pid, &helper_status, 0) == helper_pid
			&& WIFEXITED(helper_status) && WEXITSTATUS(helper_status) == 42);
	printf("CASE: exec_pipeline_wait_scope\n");
	printf("  cmd: printf hello | sleep 1\n");
	printf("  expected_status: 0\n");
	printf("  actual_status: %d\n", status);
	printf("  helper_pid_waitable: %s\n", success ? "true" : "false");
	env_free(shell.env);
	return (report_result("exec_pipeline_wait_scope", success));
}
