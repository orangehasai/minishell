/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_debug.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 07:34:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/07 07:34:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static void	print_argv(char **argv)
{
	int	i;

	if (!argv || !argv[0])
	{
		ft_putstr_fd("  argv: (empty)\n", 1);
		return ;
	}
	ft_putstr_fd("  argv:", 1);
	i = 0;
	while (argv[i])
	{
		ft_putstr_fd(" ", 1);
		ft_putstr_fd("[", 1);
		ft_putstr_fd(argv[i], 1);
		ft_putstr_fd("]", 1);
		i++;
	}
	ft_putstr_fd("\n", 1);
	return ;
}

static void	redir_type_to_str(t_token_type type)
{
	if (type == TK_REDIR_IN)
		ft_putstr_fd("<", 1);
	else if (type == TK_REDIR_OUT)
		ft_putstr_fd(">", 1);
	else if (type == TK_HEREDOC)
		ft_putstr_fd("<<", 1);
	else if (type == TK_APPEND)
		ft_putstr_fd(">>", 1);
	else
		ft_putstr_fd("none", 1);
	return ;
}

static void	print_redirs(t_redir *redirs)
{
	if (!redirs)
	{
		ft_putstr_fd("  redirs: (none)\n", 1);
		return ;
	}
	ft_putstr_fd("  redirs:", 1);
	while (redirs)
	{
		ft_putstr_fd(" ", 1);
		ft_putstr_fd("(", 1);
		redir_type_to_str(redirs->type);
		if (redirs->file[0])
		{
			ft_putstr_fd(" ", 1);
			ft_putstr_fd(redirs->file, 1);
		}
		ft_putstr_fd(")", 1);
		redirs = redirs->next;
	}
	ft_putstr_fd("\n", 1);
	return ;
}

void	debug_print_cmds(t_cmd *cmds)
{
	int	i;

	ft_putstr_fd("== parser output ==\n", 1);
	i = 0;
	while (cmds)
	{
		ft_putstr_fd("cmd[", 1);
		ft_putnbr_fd(i, 1);
		ft_putstr_fd("]\n", 1);
		print_argv(cmds->argv);
		print_redirs(cmds->redirs);
		cmds = cmds->next;
		i++;
	}
	ft_putstr_fd("== end parser output ==\n", 1);
	return ;
}
