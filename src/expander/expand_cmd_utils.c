/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_cmd_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 00:00:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/11 00:00:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"

int	expand_replace_value(char **value, t_shell *shell)
{
	char	*expanded_value;
	char	*origin;
	char	*removed_quote_value;

	expanded_value = expand_str(*value, shell, &origin);
	if (!expanded_value)
		return (0);
	removed_quote_value = remove_quotes(expanded_value, origin);
	free(origin);
	free(expanded_value);
	if (!removed_quote_value)
		return (0);
	free(*value);
	*value = removed_quote_value;
	return (1);
}
