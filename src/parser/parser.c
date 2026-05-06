/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 21:08:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/06 14:23:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static t_parse_result	parser_error_result(t_token *token, t_cmd *head,
		t_parse_error error)
{
	t_parse_result	result;

	free_cmds(head);
	if ((error == PARSE_ERR_PIPE_END || error == PARSE_ERR_PIPE_CONSECUTIVE)
		&& token && token->next)
		token = token->next;
	result.token = token;
	result.cmds = NULL;
	result.error = error;
	return (result);
}

t_parse_result	parser(t_token *tokens)
{
	t_parse_result	result;
	t_cmd			*head;
	t_cmd			*tail;
	t_cmd			*new_cmd;
	t_token			*current_token;

	head = NULL;
	tail = NULL;
	current_token = tokens;
	if (current_token && current_token->type == TK_PIPE)
		return (parser_error_result(current_token, head, PARSE_ERR_PIPE_START));
	while (current_token != NULL)
	{
		if (current_token->type == TK_EOF)
			break ;
		new_cmd = parse_command(&current_token, &result.error);
		if (!new_cmd)
			return (parser_error_result(current_token, head, result.error));
		new_cmd->next = NULL;
		append_cmd(&head, &tail, new_cmd);
	}
	result.cmds = head;
	result.token = NULL;
	result.error = PARSE_OK;
	return (result);
}

void	append_cmd(t_cmd **head, t_cmd **tail, t_cmd *new_cmd)
{
	if (!*head)
		*head = new_cmd;
	else
		(*tail)->next = new_cmd;
	*tail = new_cmd;
}
