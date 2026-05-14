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

static int	process_expand_char(const char *input, size_t *i,
		char **formatted_input, t_shell *shell)
{
	char	*expanded_input;
	size_t	expanded_i;

	expanded_i = *i;
	expanded_input = expand_dollar(input, &expanded_i, shell);
	if (!expanded_input)
		return (1);
	*i = expanded_i;
	if (append_expanded(formatted_input, expanded_input))
	{
		free(expanded_input);
		return (1);
	}
	free(expanded_input);
	return (0);
}

static char	*expand_fail(char *formatted_input)
{
	free(formatted_input);
	return (NULL);
}

char	*expand_str(const char *input, t_shell *shell)
{
	size_t			i;
	char			*formatted_input;
	t_quote_state	current_quote_state;

	formatted_input = ft_strdup("");
	if (!formatted_input)
		return (NULL);
	current_quote_state = QUOTE_NONE;
	i = 0;
	while (input[i])
	{
		current_quote_state = update_quote_state(current_quote_state, input[i]);
		if (input[i] == '$' && current_quote_state != QUOTE_SINGLE
			&& process_expand_char(input, &i, &formatted_input, shell))
			return (expand_fail(formatted_input));
		else if ((input[i] != '$' || current_quote_state == QUOTE_SINGLE)
			&& append_literal(&formatted_input, input[i]))
			return (expand_fail(formatted_input));
		i++;
	}
	return (formatted_input);
}
