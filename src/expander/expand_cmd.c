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
	char			*removed_quote_value;
	t_expand_result	result;

	if (expand_str(*value, shell, &result))
		return (1);
	removed_quote_value = remove_quotes(result.expanded, result.origin);
	free(result.origin);
	free(result.expanded);
	if (!removed_quote_value)
		return (1);
	free(*value);
	*value = removed_quote_value;
	return (0);
}

static void	remove_argv_entry(char **argv, size_t argv_i)
{
	free(argv[argv_i]);
	while (argv[argv_i + 1])
	{
		argv[argv_i] = argv[argv_i + 1];
		argv_i++;
	}
	argv[argv_i] = NULL;
}

static int	expand_argv(char **argv, t_shell *shell)
{
	size_t	argv_i;
	int		has_quote;

	argv_i = 0;
	while (argv && argv[argv_i])
	{
		has_quote = (ft_strchr(argv[argv_i], '\'') != NULL
				|| ft_strchr(argv[argv_i], '\"') != NULL);
		if (replace_value(&argv[argv_i], shell))
			return (1);
		if (!argv[argv_i][0] && !has_quote)
			remove_argv_entry(argv, argv_i);
		else
			argv_i++;
	}
	return (0);
}

static int	expand_redirs(t_redir *redirs, t_shell *shell)
{
	t_redir	*current_redir;

	current_redir = redirs;
	while (current_redir)
	{
		if (replace_value(&current_redir->file, shell))
			return (1);
		current_redir = current_redir->next;
	}
	return (0);
}

int	expander(t_cmd *cmds, t_shell *shell)
{
	t_cmd	*current_cmd;

	current_cmd = cmds;
	while (current_cmd)
	{
		if (expand_argv(current_cmd->argv, shell))
			return (1);
		if (expand_redirs(current_cmd->redirs, shell))
			return (1);
		current_cmd = current_cmd->next;
	}
	return (0);
}
