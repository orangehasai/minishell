/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_error.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 16:22:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 16:22:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

int	command_status_from_errno(int error_code)
{
	if (error_code == ENOENT)
		return (127);
	if (error_code == EACCES || error_code == ENOTDIR
		|| error_code == EISDIR || error_code == ENOEXEC)
		return (126);
	return (1);
}

void	print_resolve_error(char *cmd)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(cmd, STDERR_FILENO);
	if (errno == ENOENT)
		ft_putendl_fd(": command not found", STDERR_FILENO);
	else
	{
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putendl_fd((char *)strerror(errno), STDERR_FILENO);
	}
}
