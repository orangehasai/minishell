/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_output.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 20:44:09 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/08 20:44:09 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test.h"

int	report_result(char *name, int success)
{
	if (success)
		printf("PASS: %s\n", name);
	else
		printf("FAIL: %s\n", name);
	return (success);
}

int	strings_equal(char *left, char *right)
{
	if (!left && !right)
		return (1);
	if (!left || !right)
		return (0);
	return (ft_strncmp(left, right, ft_strlen(right) + 1) == 0);
}
