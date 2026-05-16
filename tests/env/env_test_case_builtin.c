/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_test_case_builtin.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 00:00:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 00:00:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env_test.h"

static void	setup_builtin_env_output_case(char **envp, char **argv,
		t_shell *shell, t_capture_env *capture)
{
	envp[0] = "USER=alice";
	envp[1] = "EMPTY=";
	envp[2] = "NAME_ONLY";
	envp[3] = "PATH=/bin";
	envp[4] = NULL;
	argv[0] = "env";
	argv[1] = NULL;
	shell->env = NULL;
	shell->last_status = 0;
	capture->buffer = NULL;
	capture->size = 128;
	capture->status = 0;
	capture->fd = STDOUT_FILENO;
}

static int	capture_builtin_env_output(char **argv, t_shell *shell,
		t_capture_env *capture)
{
	ssize_t	bytes;
	int		save_fd;
	int		pipe_fd[2];

	if (!capture || !capture->buffer || capture->size == 0
		|| pipe(pipe_fd) == -1)
		return (1);
	save_fd = dup(capture->fd);
	if (save_fd == -1)
		return (close(pipe_fd[0]), close(pipe_fd[1]), 1);
	if (dup2(pipe_fd[1], capture->fd) == -1)
		return (close(save_fd), close(pipe_fd[0]), close(pipe_fd[1]), 1);
	close(pipe_fd[1]);
	capture->status = builtin_env(argv, shell);
	if (dup2(save_fd, capture->fd) == -1)
		return (close(save_fd), close(pipe_fd[0]), 1);
	close(save_fd);
	bytes = read(pipe_fd[0], capture->buffer, capture->size - 1);
	close(pipe_fd[0]);
	if (bytes < 0)
		return (1);
	capture->buffer[bytes] = '\0';
	return (0);
}

int	test_builtin_env_output(void)
{
	char			buffer[128];
	char			*envp[5];
	char			*argv[2];
	t_shell			shell;
	t_capture_env	capture;

	setup_builtin_env_output_case(envp, argv, &shell, &capture);
	capture.buffer = buffer;
	if (env_init(&shell.env, envp))
		return (report_result("builtin_env_output", 0));
	if (capture_builtin_env_output(argv, &shell, &capture))
		return (env_free(shell.env), report_result("builtin_env_output", 0));
	if (capture.status
		|| !strings_equal(buffer, "USER=alice\nEMPTY=\nPATH=/bin\n"))
		return (env_free(shell.env), report_result("builtin_env_output", 0));
	env_free(shell.env);
	return (report_result("builtin_env_output", 1));
}

int	test_builtin_env_rejects_args(void)
{
	char			buffer[128];
	char			*argv[3];
	t_shell			shell;
	t_capture_env	capture;

	argv[0] = "env";
	argv[1] = "extra";
	argv[2] = NULL;
	shell.env = NULL;
	shell.last_status = 0;
	capture.buffer = buffer;
	capture.size = sizeof(buffer);
	capture.status = 0;
	capture.fd = STDERR_FILENO;
	if (capture_builtin_env_output(argv, &shell, &capture))
		return (report_result("builtin_env_rejects_args", 0));
	if (capture.status != 1 || !strings_equal(buffer,
			"minishell: env: options or arguments are not supported\n"))
		return (report_result("builtin_env_rejects_args", 0));
	return (report_result("builtin_env_rejects_args", 1));
}

int	test_builtin_env_empty_shell(void)
{
	char			buffer[8];
	char			*argv[2];
	t_shell			shell;
	t_capture_env	capture;

	argv[0] = "env";
	argv[1] = NULL;
	shell.env = NULL;
	shell.last_status = 0;
	capture.buffer = buffer;
	capture.size = sizeof(buffer);
	capture.status = 0;
	capture.fd = STDOUT_FILENO;
	if (capture_builtin_env_output(argv, &shell, &capture))
		return (report_result("builtin_env_empty_shell", 0));
	if (capture.status || !strings_equal(buffer, ""))
		return (report_result("builtin_env_empty_shell", 0));
	return (report_result("builtin_env_empty_shell", 1));
}
