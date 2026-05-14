/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 21:50:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/09 21:50:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"

int	append_literal(char **out, char c)
{
	char	*new_out;
	size_t	len;

	if (*out)
		len = ft_strlen(*out);
	else
		len = 0;
	new_out = malloc(sizeof(char) * (len + 2));
	if (!new_out)
		return (1);
	if (*out)
		ft_memcpy(new_out, *out, len);
	new_out[len] = c;
	new_out[len + 1] = '\0';
	free(*out);
	*out = new_out;
	return (0);
}

int	append_repeated_char(char **out, char c, size_t count)
{
	char	*new_out;
	size_t	len;

	if (*out)
		len = ft_strlen(*out);
	else
		len = 0;
	new_out = malloc(sizeof(char) * (len + count + 1));
	if (!new_out)
		return (1);
	if (*out)
		ft_memcpy(new_out, *out, len);
	ft_memset(new_out + len, c, count);
	new_out[len + count] = '\0';
	free(*out);
	*out = new_out;
	return (0);
}

int	append_expanded(char **out, char *expanded)
{
	char	*new_out;
	size_t	len;
	size_t	add_len;

	if (*out)
		len = ft_strlen(*out);
	else
		len = 0;
	if (expanded)
		add_len = ft_strlen(expanded);
	else
		add_len = 0;
	new_out = malloc(sizeof(char) * (len + add_len + 1));
	if (!new_out)
		return (1);
	if (*out)
		ft_memcpy(new_out, *out, len);
	if (expanded)
		ft_memcpy(new_out + len, expanded, add_len);
	new_out[len + add_len] = '\0';
	free(*out);
	*out = new_out;
	return (0);
}
