/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 16:57:08 by takenakatak       #+#    #+#             */
/*   Updated: 2026/03/21 16:57:08 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "env.h"

char	*resolve_path(char *cmd, t_shell *shell);
int		exec_simple_cmd(t_cmd *cmd, t_shell *shell);

#endif
