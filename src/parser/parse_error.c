/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_error.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 20:53:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/06 20:53:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static char	*unexpected_token(t_token *token)
{
	if (!token || token->type == TK_EOF || !token->value)
		return ("newline");
	return (token->value);
}

t_parse_error	syntax_error(t_parse_error error, t_token *token)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(unexpected_token(token), 2);
	ft_putendl_fd("'", 2);
	return (error);
}
