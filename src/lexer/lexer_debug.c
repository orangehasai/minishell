/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_debug.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 16:11:27 by skeita            #+#    #+#             */
/*   Updated: 2025/04/08 16:11:28 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

char	*token_type_to_str(t_token_type type)
{
	if (type == TK_WORD)
		return ("WORD");
	else if (type == TK_PIPE)
		return ("PIPE");
	else if (type == TK_REDIR_IN)
		return ("REDIR_IN");
	else if (type == TK_REDIR_OUT)
		return ("REDIR_OUT");
	else if (type == TK_HEREDOC)
		return ("HEREDOC");
	else if (type == TK_APPEND)
		return ("APPEND");
	else if (type == TK_EOF)
		return ("EOF");
	return ("UNKNOWN");
}

void	debug_print_tokens(t_token *tokens)
{
	char	*type;
	char	*value;

	while (tokens)
	{
		type = token_type_to_str(tokens->type);
		if (tokens->value)
			value = tokens->value;
		else
			value = "NULL";
		printf("[%s : %s]\n", type, value);
		tokens = tokens->next;
	}
	return ;
}
