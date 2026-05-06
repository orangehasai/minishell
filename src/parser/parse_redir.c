/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_redir.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 13:33:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/06 13:33:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

t_redir	*parse_redir(t_token **tokens)
{
	t_redir	*redir;
	char	*file;

	redir = new_redir((*tokens)->type);
	if (!redir)
		return (NULL);
	*tokens = (*tokens)->next;
	if (!(*tokens) || (*tokens)->type != TK_WORD)
	{
		free(redir);
		return (NULL);
	}
	file = ft_strdup((*tokens)->value);
	if (!file)
	{
		free(redir);
		return (NULL);
	}
	redir->file = file;
	return (redir);
}

t_redir	*new_redir(t_token_type type)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->file = NULL;
	redir->next = NULL;
	return (redir);
}

void	append_redir(t_cmd *cmd, t_redir *redir)
{
	t_redir	*tail;

	if (!cmd->redirs)
	{
		cmd->redirs = redir;
		return ;
	}
	tail = cmd->redirs;
	while (tail->next)
		tail = tail->next;
	tail->next = redir;
}
