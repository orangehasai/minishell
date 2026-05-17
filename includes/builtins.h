/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 16:57:05 by skeita            #+#    #+#             */
/*   Updated: 2026/05/16 02:56:11 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

# include "env.h"

int	builtin_pwd(char **argv, t_shell *shell);
int	builtin_env(char **argv, t_shell *shell);
int	builtin_cd(char **argv, t_shell *shell);
int	builtin_export(char **argv, t_shell *shell);
int	builtin_echo(char **argv, t_shell *shell);
int	put_str_fd_safe(const char *str, int fd);

#endif
