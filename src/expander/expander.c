/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 13:46:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/09 13:46:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"

static int	process_plain_char(t_expand_ctx *ctx, size_t i)
{
	if (append_literal(&ctx->result.expanded, ctx->input[i]))
		return (1);
	if (append_literal(&ctx->result.origin, ORIGIN_INPUT))
		return (1);
	return (0);
}

static int	process_expand_char(t_expand_ctx *ctx, size_t *i)
{
	char	*expanded_input;

	expanded_input = expand_dollar(ctx->input, i, ctx->shell);
	if (!expanded_input)
		return (1);
	if (append_expanded(&ctx->result.expanded, expanded_input))
		return (free(expanded_input), 1);
	if (append_repeated_char(&ctx->result.origin, ORIGIN_EXPANDED,
			ft_strlen(expanded_input)))
		return (free(expanded_input), 1);
	return (free(expanded_input), 0);
}

static int	init_expand_ctx(t_expand_ctx *ctx, const char *input,
		t_shell *shell)
{
	ctx->input = input;
	ctx->quote_state = QUOTE_NONE;
	ctx->shell = shell;
	ctx->result.origin = ft_strdup("");
	if (!ctx->result.origin)
		return (1);
	ctx->result.expanded = ft_strdup("");
	if (!ctx->result.expanded)
	{
		free(ctx->result.origin);
		ctx->result.origin = NULL;
		return (1);
	}
	return (0);
}

static int	expand_fail(t_expand_ctx *ctx)
{
	free(ctx->result.expanded);
	free(ctx->result.origin);
	return (1);
}

int	expand_str(const char *input, t_shell *shell, t_expand_result *result)
{
	size_t			i;
	t_expand_ctx	ctx;

	if (!result)
		return (1);
	if (init_expand_ctx(&ctx, input, shell))
		return (1);
	i = 0;
	while (ctx.input[i])
	{
		ctx.quote_state = update_quote_state(ctx.quote_state, ctx.input[i]);
		if (ctx.input[i] == '$' && ctx.quote_state != QUOTE_SINGLE)
		{
			if (process_expand_char(&ctx, &i))
				return (expand_fail(&ctx));
		}
		else if (process_plain_char(&ctx, i))
			return (expand_fail(&ctx));
		i++;
	}
	*result = ctx.result;
	return (0);
}
