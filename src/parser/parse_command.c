/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_command.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 21:08:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/06 13:33:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static size_t	count_argv(char **argv)
{
	size_t	i;

	i = 0;
	while (argv && argv[i])
		i++;
	return (i);
}

static char	**add_argv(char **argv, char *value)
{
	char	**new_argv;
	char	*dup;
	size_t	argv_len;
	size_t	i;

	dup = ft_strdup(value);
	if (!dup)
		return (NULL);
	argv_len = count_argv(argv);
	new_argv = malloc(sizeof(char *) * (argv_len + 2));
	if (!new_argv)
	{
		free(dup);
		return (NULL);
	}
	i = 0;
	while (i < argv_len)
	{
		new_argv[i] = argv[i];
		i++;
	}
	new_argv[i] = dup;
	new_argv[i + 1] = NULL;
	free(argv);
	return (new_argv);
}

static t_cmd	*new_cmd(void)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->redirs = NULL;
	cmd->next = NULL;
	return (cmd);
}

static int	parse_token_in_cmd(t_cmd *cmd, t_token **tokens)
{
	t_redir	*redir;
	char	**new_argv;

	if ((*tokens)->type == TK_REDIR_IN || (*tokens)->type == TK_REDIR_OUT
		|| (*tokens)->type == TK_HEREDOC || (*tokens)->type == TK_APPEND)
	{
		redir = parse_redir(tokens);
		if (!redir)
			return (1);
		append_redir(cmd, redir);
	}
	else if ((*tokens)->type == TK_WORD)
	{
		new_argv = add_argv(cmd->argv, (*tokens)->value);
		if (!new_argv)
			return (1);
		cmd->argv = new_argv;
	}
	return (0);
}

t_cmd	*parse_command(t_token **tokens)
{
	t_cmd	*cmd;

	cmd = new_cmd();
	if (!cmd)
		return (NULL);
	while (*tokens != NULL)
	{
		if ((*tokens)->type == TK_EOF)
			break ;
		else if ((*tokens)->type == TK_PIPE)
		{
			*tokens = (*tokens)->next;
			break ;
		}
		if (parse_token_in_cmd(cmd, tokens))
		{
			free_cmd(cmd);
			return (NULL);
		}
		*tokens = (*tokens)->next;
	}
	return (cmd);
}
