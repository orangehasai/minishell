/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_test_report.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 20:10:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 20:10:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env_test.h"

static void	print_escaped_value(char *value)
{
	size_t	index;

	if (!value)
	{
		printf("(null)");
		return ;
	}
	index = 0;
	while (value[index])
	{
		if (value[index] == '\n')
			printf("\\n");
		else if (value[index] == '\t')
			printf("\\t");
		else if (value[index] == '\"')
			printf("\\\"");
		else
			printf("%c", value[index]);
		index++;
	}
}

void	print_env_case(char *name, char *scenario)
{
	printf("CASE: %s\n", name);
	if (scenario)
		printf("  scenario: %s\n", scenario);
}

void	print_env_text(char *label, char *value)
{
	printf("  %s: \"", label);
	print_escaped_value(value);
	printf("\"\n");
}

void	print_env_int(char *label, int value)
{
	printf("  %s: %d\n", label, value);
}

void	print_env_flag(char *label, int value)
{
	printf("  %s: %s\n", label, value ? "true" : "false");
}
