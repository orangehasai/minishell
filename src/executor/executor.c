/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/17 15:20:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/17 15:20:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

int	execute(t_cmd *cmds, t_shell *shell)
{
	if (!cmds || !shell)
		return (1);
	if (!cmds->next)
		return (exec_simple_cmd(cmds, shell));
	return (exec_pipeline(cmds, shell));
}
