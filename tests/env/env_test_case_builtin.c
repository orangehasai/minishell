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
	int				success;

	setup_builtin_env_output_case(envp, argv, &shell, &capture);
	capture.buffer = buffer;
	success = (env_init(&shell.env, envp) == 0);
	success = success && (capture_builtin_env_output(argv, &shell, &capture) == 0);
	success = success && (capture.status == 0);
	success = success && strings_equal(buffer, "USER=alice\nEMPTY=\nPATH=/bin\n");
	print_env_case("builtin_env_output", "builtin_env(env) with USER, EMPTY, NAME_ONLY, PATH");
	print_env_int("expected_status", 0);
	print_env_int("actual_status", capture.status);
	print_env_text("expected_output", "USER=alice\nEMPTY=\nPATH=/bin\n");
	print_env_text("actual_output", buffer);
	env_free(shell.env);
	return (report_result("builtin_env_output", success));
}

int	test_builtin_env_rejects_args(void)
{
	char			buffer[128];
	char			*argv[3];
	t_shell			shell;
	t_capture_env	capture;
	int				success;

	argv[0] = "env";
	argv[1] = "extra";
	argv[2] = NULL;
	shell.env = NULL;
	shell.last_status = 0;
	capture.buffer = buffer;
	capture.size = sizeof(buffer);
	capture.status = 0;
	capture.fd = STDERR_FILENO;
	success = (capture_builtin_env_output(argv, &shell, &capture) == 0);
	success = success && (capture.status == 1);
	success = success && strings_equal(buffer,
			"minishell: env: options or arguments are not supported\n");
	print_env_case("builtin_env_rejects_args", "builtin_env(env extra)");
	print_env_int("expected_status", 1);
	print_env_int("actual_status", capture.status);
	print_env_text("expected_error",
		"minishell: env: options or arguments are not supported\n");
	print_env_text("actual_error", buffer);
	return (report_result("builtin_env_rejects_args", success));
}

int	test_builtin_env_empty_shell(void)
{
	char			buffer[8];
	char			*argv[2];
	t_shell			shell;
	t_capture_env	capture;
	int				success;

	argv[0] = "env";
	argv[1] = NULL;
	shell.env = NULL;
	shell.last_status = 0;
	capture.buffer = buffer;
	capture.size = sizeof(buffer);
	capture.status = 0;
	capture.fd = STDOUT_FILENO;
	success = (capture_builtin_env_output(argv, &shell, &capture) == 0);
	success = success && (capture.status == 0);
	success = success && strings_equal(buffer, "");
	print_env_case("builtin_env_empty_shell", "builtin_env(empty shell)");
	print_env_int("expected_status", 0);
	print_env_int("actual_status", capture.status);
	print_env_text("expected_output", "");
	print_env_text("actual_output", buffer);
	return (report_result("builtin_env_empty_shell", success));
}
