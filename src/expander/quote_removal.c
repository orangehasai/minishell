/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_removal.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 18:26:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/10 18:26:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"

static char	*remove_quote_fail(char *removed_quote_input)
{
	free(removed_quote_input);
	return (NULL);
}

static int	is_input_syntax_quote(char c, t_quote_state current_quote_state,
		t_quote_state next_quote_state, char origin)
{
	if (origin != '1')
		return (0);
	if ((c == '\'' || c == '\"') && current_quote_state != next_quote_state)
		return (1);
	return (0);
}

static int	process_remove_char(size_t *i, t_remove_ctx *ctx)
{
	t_quote_state	next_quote_state;

	if (ctx->origin[*i] == '1')
		next_quote_state = update_quote_state(*ctx->current_quote_state,
				ctx->input[*i]);
	else
		next_quote_state = *ctx->current_quote_state;
	if (!is_input_syntax_quote(ctx->input[*i], *ctx->current_quote_state,
			next_quote_state, ctx->origin[*i])
		&& !append_literal(ctx->removed_quote_input, ctx->input[*i]))
		return (0);
	*ctx->current_quote_state = next_quote_state;
	(*i)++;
	return (1);
}

char	*remove_quotes(const char *input, const char *origin)
{
	size_t			i;
	char			*removed_quote_input;
	t_quote_state	current_quote_state;
	t_remove_ctx	rem_ctx;

	if (!input || !origin)
		return (NULL);
	removed_quote_input = ft_strdup("");
	if (!removed_quote_input)
		return (NULL);
	current_quote_state = QUOTE_NONE;
	rem_ctx.input = input;
	rem_ctx.origin = origin;
	rem_ctx.removed_quote_input = &removed_quote_input;
	rem_ctx.current_quote_state = &current_quote_state;
	i = 0;
	while (input[i] && origin[i])
	{
		if (!process_remove_char(&i, &rem_ctx))
			return (remove_quote_fail(removed_quote_input));
	}
	if (input[i] || origin[i])
		return (remove_quote_fail(removed_quote_input));
	return (removed_quote_input);
}
