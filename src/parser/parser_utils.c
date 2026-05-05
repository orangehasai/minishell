/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 21:08:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/05 21:08:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

void	free_cmds(t_cmd *cmd)
{
	t_cmd	*next;

	while (cmd != NULL)
	{
		next = cmd->next;
		free_cmd(cmd);
		cmd = next;
	}
}

void	free_cmd(t_cmd *cmd)
{
	size_t	i;

	if (!cmd)
		return ;
	i = 0;
	while (cmd->argv && cmd->argv[i])
	{
		free(cmd->argv[i]);
		i++;
	}
	free(cmd->argv);
	free(cmd);
}
