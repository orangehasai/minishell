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
	printf("%d/9 tests passed\n", passed);
	if (passed != 9)
		return (1);
	return (0);
}
