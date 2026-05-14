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

static int	append_origin_n(char **origin, char flag, size_t n)
{
	while (n--)
	{
		if (append_literal(origin, flag))
			return (1);
	}
	return (0);
}

static int	process_expand_char(const char *input, size_t *i,
		t_expand_out *out)
{
	char	*expanded_input;
	size_t	expanded_i;

	expanded_i = *i;
	expanded_input = expand_dollar(input, &expanded_i, out->shell);
	if (!expanded_input)
		return (1);
	*i = expanded_i;
	if (append_expanded(out->formatted_input, expanded_input))
	{
		free(expanded_input);
		return (1);
	}
	if (append_origin_n(out->origin, '0', ft_strlen(expanded_input)))
	{
		free(expanded_input);
		return (1);
	}
	free(expanded_input);
	return (0);
}

static int	init_expand_str(char **formatted_input, char **origin,
		t_expand_out *out, t_shell *shell)
{
	*origin = ft_strdup("");
	if (!*origin)
		return (1);
	*formatted_input = ft_strdup("");
	if (!*formatted_input)
	{
		free(*origin);
		*origin = NULL;
		return (1);
	}
	out->origin = origin;
	out->formatted_input = formatted_input;
	out->shell = shell;
	return (0);
}

static char	*expand_fail(char *formatted_input, char *origin)
{
	free(formatted_input);
	free(origin);
	return (NULL);
}

static int	process_expand_step(const char *input, size_t *i, t_expand_out *out,
		t_quote_state current_quote_state)
{
	if (input[*i] == '$' && current_quote_state != QUOTE_SINGLE)
		return (process_expand_char(input, i, out));
	if (append_literal(out->formatted_input, input[*i])
		|| append_literal(out->origin, '1'))
		return (1);
	return (0);
}

char	*expand_str(const char *input, t_shell *shell, char **origin)
{
	size_t			i;
	char			*formatted_input;
	t_quote_state	current_quote_state;
	t_expand_out	out;

	if (init_expand_str(&formatted_input, origin, &out, shell))
		return (NULL);
	current_quote_state = QUOTE_NONE;
	i = 0;
	while (input[i])
	{
		current_quote_state = update_quote_state(current_quote_state, input[i]);
		if (process_expand_step(input, &i, &out, current_quote_state))
			return (expand_fail(formatted_input, *origin));
		i++;
	}
	return (formatted_input);
}
