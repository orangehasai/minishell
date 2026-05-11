/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 21:51:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/09 21:51:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander.h"

static int	has_quote(char *value)
{
	size_t	i;

	if (!value)
		return (0);
	i = 0;
	while (value[i])
	{
		if (value[i] == '\'' || value[i] == '"')
			return (1);
		i++;
	}
	return (0);
}

static void	remove_argv_node(char **argv, size_t argv_i)
{
	free(argv[argv_i]);
	while (argv[argv_i])
	{
		argv[argv_i] = argv[argv_i + 1];
		argv_i++;
	}
}

static int	expand_argv(char **argv, t_shell *shell)
{
	size_t	argv_i;
	int		had_quote;

	argv_i = 0;
	while (argv && argv[argv_i])
	{
		had_quote = has_quote(argv[argv_i]);
		if (!expand_replace_value(&argv[argv_i], shell))
			return (0);
		if (!had_quote && argv[argv_i][0] == '\0')
		{
			remove_argv_node(argv, argv_i);
			continue ;
		}
		argv_i++;
	}
	return (1);
}

static int	expand_redirs(t_redir *redirs, t_shell *shell)
{
	t_redir	*current_redir;

	current_redir = redirs;
	while (current_redir)
	{
		if (!expand_replace_value(&current_redir->file, shell))
			return (0);
		current_redir = current_redir->next;
	}
	return (1);
}

int	expander(t_cmd *cmds, t_shell *shell)
{
	t_cmd	*current_cmd;

	current_cmd = cmds;
	while (current_cmd)
	{
		if (!expand_argv(current_cmd->argv, shell))
			return (1);
		if (!expand_redirs(current_cmd->redirs, shell))
			return (1);
		current_cmd = current_cmd->next;
	}
	return (0);
}
