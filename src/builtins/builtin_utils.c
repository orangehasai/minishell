/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 21:48:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/12 21:48:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

int	put_str_fd_safe(const char *str, int fd)
{
	ssize_t	written;
	size_t	len;

	if (!str)
		return (1);
	len = ft_strlen(str);
	written = write(fd, str, len);
	if (written >= 0 && (size_t)written == len)
		return (1);
	return (0);
}
