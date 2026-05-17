/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 21:15:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 21:15:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

static int	open_redir_file(t_redir *redir)
{
	if (redir->type == TK_REDIR_IN)
		return (open(redir->file, O_RDONLY));
	if (redir->type == TK_REDIR_OUT)
		return (open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0666));
	if (redir->type == TK_APPEND)
		return (open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0666));
	ft_putendl_fd("minishell: heredoc not supported yet", STDERR_FILENO);
	return (-1);
}

static int	dup_redir_fd(t_redir *redir, int fd)
{
	int	target_fd;

	target_fd = STDOUT_FILENO;
	if (redir->type == TK_REDIR_IN)
		target_fd = STDIN_FILENO;
	if (dup2(fd, target_fd) < 0)
	{
		perror("minishell: dup2");
		close(fd);
		return (1);
	}
	if (close(fd) < 0)
	{
		perror("minishell: close");
		return (1);
	}
	return (0);
}

int	apply_redirections(t_redir *redirs)
{
	int	fd;

	while (redirs)
	{
		fd = open_redir_file(redirs);
		if (fd < 0)
		{
			if (redirs->type != TK_HEREDOC)
			{
				ft_putstr_fd("minishell: ", STDERR_FILENO);
				perror(redirs->file);
			}
			return (1);
		}
		if (dup_redir_fd(redirs, fd))
			return (1);
		redirs = redirs->next;
	}
	return (0);
}
