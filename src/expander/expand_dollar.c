/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_dollar.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 21:50:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/09 21:50:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"

t_quote_state	update_quote_state(t_quote_state quote, char c)
{
	if (c == '\'' && quote != QUOTE_DOUBLE)
	{
		if (quote == QUOTE_SINGLE)
			return (QUOTE_NONE);
		return (QUOTE_SINGLE);
	}
	else if (c == '\"' && quote != QUOTE_SINGLE)
	{
		if (quote == QUOTE_DOUBLE)
			return (QUOTE_NONE);
		return (QUOTE_DOUBLE);
	}
	return (quote);
}

static char	*expand_variable(const char *input, size_t *i, t_shell *shell)
{
	char	*expand_target_input;
	char	*expanded_input;
	size_t	input_i;

	input_i = *i + 1;
	while (ft_isalnum(input[input_i]) || input[input_i] == '_')
		input_i++;
	expand_target_input = ft_substr(input, *i + 1, input_i - *i - 1);
	if (!expand_target_input)
		return (NULL);
	expanded_input = env_get(shell->env, expand_target_input);
	free(expand_target_input);
	*i = input_i - 1;
	if (!expanded_input)
		return (ft_strdup(""));
	return (ft_strdup(expanded_input));
}

char	*expand_dollar(const char *input, size_t *i, t_shell *shell)
{
	size_t	input_i;

	if (input[*i] != '$')
		return (NULL);
	input_i = *i + 1;
	if (input[input_i] == '?')
	{
		*i = input_i;
		return (ft_itoa(shell->last_status));
	}
	if (ft_isdigit(input[input_i]))
	{
		*i = input_i;
		return (ft_strdup(""));
	}
	if (!ft_isalpha(input[input_i]) && input[input_i] != '_')
		return (ft_strdup("$"));
	return (expand_variable(input, i, shell));
}
