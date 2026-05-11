/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_redir.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 13:33:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/06 14:25:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static char	*dup_redir_target(t_token **tokens, t_parse_error *error)
{
	char	*file;

	*tokens = (*tokens)->next;
	if (!(*tokens) || (*tokens)->type != TK_WORD)
	{
		*error = PARSE_ERR_REDIR_TARGET;
		return (NULL);
	}
	file = ft_strdup((*tokens)->value);
	if (!file)
	{
		*error = PARSE_ERR_INTERNAL;
		return (NULL);
	}
	return (file);
}

t_redir	*parse_redir(t_token **tokens, t_parse_error *error)
{
	t_redir	*redir;

	redir = new_redir((*tokens)->type);
	if (!redir)
	{
		*error = PARSE_ERR_INTERNAL;
		return (NULL);
	}
	redir->file = dup_redir_target(tokens, error);
	if (!redir->file)
	{
		free(redir);
		return (NULL);
	}
	redir->raw_file = ft_strdup(redir->file);
	if (!redir->raw_file)
	{
		free(redir->file);
		free(redir);
		*error = PARSE_ERR_INTERNAL;
		return (NULL);
	}
	*error = PARSE_OK;
	return (redir);
}

t_redir	*new_redir(t_token_type type)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->raw_file = NULL;
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
