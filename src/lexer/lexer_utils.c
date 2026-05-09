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

int	is_whitespace(char c)
{
	return (c == ' ' || ('\t' <= c && c <= '\r'));
}

int	is_operator(char c)
{
	if (c == '|' || c == '<' || c == '>')
		return (1);
	return (0);
}

void	skip_whitespace(char *line, size_t *i)
{
	if (!line || !i)
		return ;
	while (is_whitespace(line[*i]))
		(*i)++;
}
