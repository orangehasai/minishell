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

static int	has_unclosed_quote(char *line)
{
	size_t			i;
	t_lexer_state	state;

	if (!line)
		return (1);
	i = 0;
	state = NORMAL;
	while (line[i])
	{
		if (state == NORMAL && line[i] == '\'')
			state = SINGLE_QUOTE;
		else if (state == NORMAL && line[i] == '"')
			state = DOUBLE_QUOTE;
		else if (state == SINGLE_QUOTE && line[i] == '\'')
			state = NORMAL;
		else if (state == DOUBLE_QUOTE && line[i] == '"')
			state = NORMAL;
		i++;
	}
	return (state != NORMAL);
}

static t_lexer_result	lexer_result(t_token *tokens, t_lexer_error error)
{
	t_lexer_result	result;

	result.tokens = tokens;
	result.error = error;
	return (result);
}

t_lexer_result	lexer(char *line)
{
	size_t	i;
	t_token	*head;
	t_token	*tail;

	if (has_unclosed_quote(line))
		return (lexer_result(NULL, LEXER_ERR_UNCLOSED_QUOTE));
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
			return (lexer_result(NULL, LEXER_ERR_INTERNAL));
		}
	}
	if (!append_eof_token(&head, &tail))
		return (free_tokens(head), lexer_result(NULL, LEXER_ERR_INTERNAL));
	return (lexer_result(head, LEXER_OK));
}
