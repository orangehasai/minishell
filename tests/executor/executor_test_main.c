/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_test_main.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 18:35:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 18:35:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_test.h"

int	main(void)
{
	int	passed;

	passed = 0;
	passed += test_exec_external_echo();
	passed += test_exec_path_search_printf();
	passed += test_exec_missing_command();
	passed += test_exec_permission_denied();
	passed += test_exec_path_empty_entry();
	passed += test_exec_redir_stdout_truncate();
	passed += test_exec_redir_stdout_append();
	passed += test_exec_redir_stdin();
	passed += test_exec_redir_left_to_right();
	passed += test_exec_redir_heredoc_unsupported();
	passed += test_exec_builtin_echo();
	passed += test_exec_builtin_echo_no_newline();
	passed += test_exec_builtin_echo_multi_n();
	passed += test_exec_builtin_echo_invalid_n_flag();
	passed += test_exec_builtin_pwd();
	passed += test_exec_builtin_env();
	passed += test_exec_builtin_env_rejects_args();
	passed += test_exec_builtin_cd();
	passed += test_exec_builtin_cd_absolute();
	passed += test_exec_builtin_cd_rejects_args();
	printf("%d/20 tests passed\n", passed);
	if (passed != 20)
		return (1);
	return (0);
}
