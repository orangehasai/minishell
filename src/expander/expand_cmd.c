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

static int	replace_value(char **value, t_shell *shell)
{
	char	*expanded_value;
	char	*origin;
	char	*removed_quote_value;

	expanded_value = expand_str(*value, shell, &origin);
	if (!expanded_value)
		return (0);
	removed_quote_value = remove_quotes(expanded_value, origin);
	free(origin);
	free(expanded_value);
	if (!removed_quote_value)
		return (0);
	free(*value);
	*value = removed_quote_value;
	return (1);
}

static int	expand_argv(char **argv, t_shell *shell)
{
	size_t	argv_i;

	argv_i = 0;
	while (argv && argv[argv_i])
	{
		if (!replace_value(&argv[argv_i], shell))
			return (0);
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
		if (!replace_value(&current_redir->file, shell))
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
