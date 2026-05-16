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
	passed += test_exec_path_search_echo();
	passed += test_exec_missing_command();
	passed += test_exec_permission_denied();
	passed += test_exec_path_empty_entry();
	passed += test_exec_builtin_export();
	passed += test_exec_builtin_pwd();
	passed += test_exec_builtin_env();
	passed += test_exec_builtin_env_rejects_args();
	passed += test_exec_builtin_cd();
	passed += test_exec_builtin_cd_absolute();
	passed += test_exec_builtin_cd_rejects_args();
	printf("%d/12 tests passed\n", passed);
	if (passed != 12)
		return (1);
	return (0);
}
