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

static t_token	*make_operator_token(t_token_type type, char *value,
		size_t *i, size_t len)
{
	*i += len;
	return (new_token(type, ft_strdup(value)));
}

t_token	*read_operator(char *line, size_t *i)
{
	if (line[*i] == '|')
		return (make_operator_token(TK_PIPE, "|", i, 1));
	if (line[*i] == '<' && line[*i + 1] == '<')
		return (make_operator_token(TK_HEREDOC, "<<", i, 2));
	if (line[*i] == '<')
		return (make_operator_token(TK_REDIR_IN, "<", i, 1));
	if (line[*i] == '>' && line[*i + 1] == '>')
		return (make_operator_token(TK_APPEND, ">>", i, 2));
	if (line[*i] == '>')
		return (make_operator_token(TK_REDIR_OUT, ">", i, 1));
	return (NULL);
}
