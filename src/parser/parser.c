/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 21:08:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/05 21:08:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

t_cmd	*parser(t_token *tokens)
{
	t_cmd	*head;
	t_cmd	*tail;
	t_cmd	*new_cmd;
	t_token	*current_token;

	head = NULL;
	tail = NULL;
	current_token = tokens;
	while (current_token != NULL)
	{
		if (current_token->type == TK_EOF)
			break ;
		new_cmd = parse_command(&current_token);
		if (!new_cmd)
		{
			free_cmds(head);
			return (NULL);
		}
		new_cmd->next = NULL;
		append_cmd(&head, &tail, new_cmd);
	}
	return (head);
}

void	append_cmd(t_cmd **head, t_cmd **tail, t_cmd *new_cmd)
{
	if (!*head)
		*head = new_cmd;
	else
		(*tail)->next = new_cmd;
	*tail = new_cmd;
}
