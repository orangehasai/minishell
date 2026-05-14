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

static int	process_plain_char(t_expand_out *out, char c)
{
	if (append_literal(out->formatted_input, c))
		return (1);
	if (append_literal(out->origin, '1'))
		return (1);
	return (0);
}

static int	process_expand_char(const char *input, size_t *i, t_expand_out *out)
{
	char	*expanded_input;
	size_t	expanded_i;
	size_t	len;

	expanded_i = *i;
	expanded_input = expand_dollar(input, &expanded_i, out->shell);
	if (!expanded_input)
		return (1);
	*i = expanded_i;
	if (append_expanded(out->formatted_input, expanded_input))
		return (free(expanded_input), 1);
	len = ft_strlen(expanded_input);
	while (len-- > 0)
		if (append_literal(out->origin, '0'))
			return (free(expanded_input), 1);
	return (free(expanded_input), 0);
}

static int	init_expand_out(t_expand_out *out, char **formatted_input,
		char **origin, t_shell *shell)
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

char	*expand_str(const char *input, t_shell *shell, char **origin)
{
	size_t			i;
	char			*formatted_input;
	t_quote_state	current_quote_state;
	t_expand_out	out;

	if (init_expand_out(&out, &formatted_input, origin, shell))
		return (NULL);
	current_quote_state = QUOTE_NONE;
	i = 0;
	while (input[i])
	{
		current_quote_state = update_quote_state(current_quote_state, input[i]);
		if (input[i] == '$' && current_quote_state != QUOTE_SINGLE
			&& process_expand_char(input, &i, &out))
			return (expand_fail(formatted_input, *origin));
		else if ((input[i] != '$' || current_quote_state == QUOTE_SINGLE)
			&& process_plain_char(&out, input[i]))
			return (expand_fail(formatted_input, *origin));
		i++;
	}
	return (formatted_input);
}
