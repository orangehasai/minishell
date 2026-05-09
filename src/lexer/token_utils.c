/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 16:11:27 by skeita            #+#    #+#             */
/*   Updated: 2025/04/05 16:11:28 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

t_token	*new_token(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
	{
		free(value);
		return (NULL);
	}
	token->type = type;
	token->value = value;
	token->next = NULL;
	return (token);
}

void	append_token(t_token **head, t_token **tail, t_token *new_tok)
{
	if (!*head)
		*head = new_tok;
	else
		(*tail)->next = new_tok;
	*tail = new_tok;
}

void	free_tokens(t_token *tokens)
{
	t_token	*next_token;

	while (tokens)
	{
		next_token = tokens->next;
		free(tokens->value);
		free(tokens);
		tokens = next_token;
	}
}
