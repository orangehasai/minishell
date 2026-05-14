/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 21:51:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/09 21:51:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"

static int	replace_argv_value(char **argv_value, t_shell *shell)
{
	t_expand_result	result;
	char			*removed_quote_argv;

	if (expand_str(*argv_value, shell, &result))
		return (1);
	removed_quote_argv = remove_quotes(result.expanded, result.origin);
	free(result.origin);
	free(result.expanded);
	if (!removed_quote_argv)
		return (1);
	free(*argv_value);
	*argv_value = removed_quote_argv;
	return (0);
}

int	expander(t_cmd *cmds, t_shell *shell)
{
	t_cmd	*current_cmd;
	size_t	argv_i;

	current_cmd = cmds;
	while (current_cmd)
	{
		argv_i = 0;
		while (current_cmd->argv && current_cmd->argv[argv_i])
		{
			if (replace_argv_value(&current_cmd->argv[argv_i], shell))
				return (1);
			argv_i++;
		}
		current_cmd = current_cmd->next;
	}
	return (0);
}
