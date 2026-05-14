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

static char	*remove_quote_fail(t_remove_ctx *ctx)
{
	free(ctx->removed_quote_input);
	return (NULL);
}

static int	is_input_syntax_quote(t_remove_ctx *ctx, size_t i,
		t_quote_state next_quote_state)
{
	if (ctx->origin[i] != ORIGIN_INPUT)
		return (0);
	if ((ctx->input[i] == '\'' || ctx->input[i] == '\"')
		&& ctx->current_quote_state != next_quote_state)
		return (1);
	return (0);
}

static int	process_remove_char(t_remove_ctx *ctx, size_t *i)
{
	t_quote_state	next_quote_state;

	next_quote_state = ctx->current_quote_state;
	if (ctx->origin[*i] == ORIGIN_INPUT)
		next_quote_state = update_quote_state(ctx->current_quote_state,
				ctx->input[*i]);
	if (!is_input_syntax_quote(ctx, *i, next_quote_state)
		&& append_literal(&ctx->removed_quote_input, ctx->input[*i]))
		return (1);
	ctx->current_quote_state = next_quote_state;
	(*i)++;
	return (0);
}

char	*remove_quotes(const char *input, const char *origin)
{
	size_t			i;
	t_remove_ctx	ctx;

	if (!input || !origin)
		return (NULL);
	ctx.input = input;
	ctx.origin = origin;
	ctx.current_quote_state = QUOTE_NONE;
	ctx.removed_quote_input = ft_strdup("");
	if (!ctx.removed_quote_input)
		return (NULL);
	i = 0;
	while (ctx.input[i] && ctx.origin[i])
	{
		if (process_remove_char(&ctx, &i))
			return (remove_quote_fail(&ctx));
	}
	if (ctx.input[i] || ctx.origin[i])
		return (remove_quote_fail(&ctx));
	return (ctx.removed_quote_input);
}
