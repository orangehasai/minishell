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

static void	update_word_state(char c, t_lexer_state *state)
{
	if (*state == NORMAL && c == '\'')
		*state = SINGLE_QUOTE;
	else if (*state == NORMAL && c == '"')
		*state = DOUBLE_QUOTE;
	else if (*state == SINGLE_QUOTE && c == '\'')
		*state = NORMAL;
	else if (*state == DOUBLE_QUOTE && c == '"')
		*state = NORMAL;
}

static t_token	*build_word_token(char *line, size_t start_i, size_t end_i)
{
	char	*value;

	value = ft_substr(line, start_i, end_i - start_i);
	if (value == NULL)
		return (NULL);
	return (new_token(TK_WORD, value));
}

t_token	*read_word(char *line, size_t *i)
{
	size_t			start_i;
	t_lexer_state	current_state;

	start_i = *i;
	current_state = NORMAL;
	while (line[*i])
	{
		if (current_state == NORMAL
			&& (is_whitespace(line[*i]) || is_operator(line[*i])))
			break ;
		update_word_state(line[*i], &current_state);
		(*i)++;
	}
	if (current_state != NORMAL)
		return (NULL);
	return (build_word_token(line, start_i, *i));
}
