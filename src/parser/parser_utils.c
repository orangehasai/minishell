/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 21:08:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/06 13:33:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static void	free_redirs(t_cmd *cmd);

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
	free_redirs(cmd);
	free(cmd);
}

static void	free_redirs(t_cmd *cmd)
{
	t_redir	*redir;
	t_redir	*next_redir;

	redir = cmd->redirs;
	if (!redir)
		return ;
	while (redir)
	{
		next_redir = redir->next;
		free(redir->file);
		free(redir);
		redir = next_redir;
	}
	cmd->redirs = NULL;
}
