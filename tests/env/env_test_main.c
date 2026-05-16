/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_test_main.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 20:44:09 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/09 01:09:33 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env_test.h"

int	main(void)
{
	int	passed;

	passed = 0;
	passed += test_env_init();
	passed += test_env_set_insert();
	passed += test_env_set_update();
	passed += test_env_set_null_value();
	passed += test_env_unset();
	passed += test_env_invalid_args();
	passed += test_env_to_array();
	passed += test_split_key_value();
	passed += test_env_print_export();
	passed += test_builtin_env_output();
	passed += test_builtin_env_rejects_args();
	passed += test_builtin_env_empty_shell();
	printf("%d/12 tests passed\n", passed);
	if (passed != 12)
		return (1);
	return (0);
}
