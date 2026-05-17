/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_test.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 18:35:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 18:35:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_TEST_H
# define EXECUTOR_TEST_H

# include "env.h"
# include "executor.h"
# include "test.h"

typedef struct s_capture_exec
{
	char	*buffer;
	size_t	size;
	int		status;
	int		fd;
}	t_capture_exec;

typedef struct s_exec_case
{
	char			*envp[8];
	char			*argv[8];
	t_cmd			cmd;
	t_shell			shell;
	t_capture_exec	capture;
}	t_exec_case;

typedef struct s_capture_expect
{
	char	*cmdline;
	char	*stream;
	int		expected_status;
	char	*expected_output;
}	t_capture_expect;

typedef struct s_cd_expect
{
	char	*cmdline;
	int		expected_status;
	char	*oldpwd;
	char	*expected_pwd;
}	t_cd_expect;

int		init_test_shell(t_shell *shell, char **envp);
void	init_exec_case(t_exec_case *case_data, char *buffer, size_t size,
			int fd);
void	cleanup_exec_case(t_exec_case *case_data);
int		capture_exec_output(t_cmd *cmd, t_shell *shell,
			t_capture_exec *capture);
int		capture_pipeline_output(t_cmd *cmds, t_shell *shell,
			t_capture_exec *capture);
int		create_test_file(char *path, mode_t mode, char *content);
int		report_capture_case(char *name, t_capture_expect *expect,
			t_capture_exec *capture);
int		report_cd_case(char *name, int actual_status, t_cd_expect *expect,
			t_shell *shell);
int		report_cd_file_case(char *name, char *cmdline, int actual_status,
			t_cd_expect *expect, char *path, char *expected, char *actual,
			t_shell *shell);
int		report_file_case(char *name, char *cmdline, int expected_status,
			int actual_status, char *path, char *expected, char *actual);
int		report_dual_file_case(char *name, char *cmdline, int expected_status,
			int actual_status, char **paths, char **expected, char **actual);
int		test_exec_external_echo(void);
int		test_exec_path_search_printf(void);
int		test_exec_missing_command(void);
int		test_exec_permission_denied(void);
int		test_exec_path_empty_entry(void);
int		test_exec_builtin_export(void);
int		test_exec_redir_stdout_truncate(void);
int		test_exec_redir_stdout_append(void);
int		test_exec_redir_stdin(void);
int		test_exec_redir_left_to_right(void);
int		test_exec_redir_heredoc_unsupported(void);
int		test_exec_builtin_redir_echo(void);
int		test_exec_builtin_redir_pwd(void);
int		test_exec_builtin_redir_env(void);
int		test_exec_builtin_redir_cd(void);
int		test_exec_builtin_echo(void);
int		test_exec_builtin_echo_no_newline(void);
int		test_exec_builtin_echo_multi_n(void);
int		test_exec_builtin_echo_invalid_n_flag(void);
int		test_exec_builtin_pwd(void);
int		test_exec_builtin_env(void);
int		test_exec_builtin_env_rejects_args(void);
int		test_exec_builtin_unset(void);
int		test_exec_builtin_unset_invalid_identifier(void);
int		test_exec_builtin_unset_partial_error(void);
int		test_exec_unset_path_then_ls(void);
int		test_exec_builtin_cd(void);
int		test_exec_builtin_cd_absolute(void);
int		test_exec_builtin_cd_rejects_args(void);
int		test_exec_pipeline_external(void);
int		test_exec_pipeline_builtin_echo(void);
int		test_exec_pipeline_redir(void);
int		test_exec_pipeline_missing_first(void);
int		test_exec_pipeline_missing_last(void);
int		test_exec_pipeline_wait_scope(void);

#endif
