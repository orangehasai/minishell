/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 16:57:12 by takenakatak       #+#    #+#             */
/*   Updated: 2026/03/21 16:57:13 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

static int	append_current_token(char *line, size_t *i, t_token **head,
		t_token **tail)
{
	t_token	*token;

	if (is_operator(line[*i]))
		token = read_operator(line, i);
	else
		token = read_word(line, i);
	if (!token)
		return (0);
	append_token(head, tail, token);
	return (1);
}

static int	append_eof_token(t_token **head, t_token **tail)
{
	t_token	*token;

	token = new_token(TK_EOF, NULL);
	if (!token)
		return (0);
	append_token(head, tail, token);
	return (1);
}

t_token	*lexer(char *line)
{
	size_t	i;
	t_token	*head;
	t_token	*tail;

	i = 0;
	head = NULL;
	tail = NULL;
	while (line[i])
	{
		skip_whitespace(line, &i);
		if (!line[i])
			break ;
		if (!append_current_token(line, &i, &head, &tail))
		{
			free_tokens(head);
			return (NULL);
		}
	}
	if (!append_eof_token(&head, &tail))
	{
		free_tokens(head);
		return (NULL);
	}
	return (head);
}
