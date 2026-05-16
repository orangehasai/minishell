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

typedef int	(*t_builtin_fn)(char **argv, t_shell *shell);

char	*resolve_path(char *cmd, t_shell *shell);
int		command_status_from_errno(int error_code);
void	print_resolve_error(char *cmd);
int		apply_redirections(t_redir *redirs);
int		exec_simple_cmd(t_cmd *cmd, t_shell *shell);
int		wait_pipeline(pid_t *pids, int count, pid_t last_pid);
void	cleanup_pipeline_error(int prev_fd, int pipe_fd[2], pid_t *pids,
			int count);
int		exec_pipeline(t_cmd *cmds, t_shell *shell);
void	exec_pipeline_child(t_cmd *cmd, t_shell *shell, int prev_fd,
			int pipe_fd[2]);
void	exec_external_cmd(t_cmd *cmd, t_shell *shell);
int		is_builtin_cmd(char *cmd);
int		exec_builtin_cmd(t_cmd *cmd, t_shell *shell);
int		exec_builtin_with_redir(t_cmd *cmd, t_shell *shell);

#endif
