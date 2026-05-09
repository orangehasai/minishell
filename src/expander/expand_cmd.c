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

int	expander(t_cmd *cmds, t_shell *shell)
{
	t_cmd	*current_cmd;
	size_t	argv_i;
	char	*formatted_argv;

	current_cmd = cmds;
	while (current_cmd)
	{
		argv_i = 0;
		while (current_cmd->argv && current_cmd->argv[argv_i])
		{
			formatted_argv = expand_str(current_cmd->argv[argv_i], shell);
			if (!formatted_argv)
				return (1);
			free(current_cmd->argv[argv_i]);
			current_cmd->argv[argv_i] = formatted_argv;
			argv_i++;
		}
		current_cmd = current_cmd->next;
	}
	return (0);
}
