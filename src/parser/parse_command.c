/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_command.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 21:08:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/06 14:21:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static char	**add_argv(char **argv, char *value)
{
	char	**new_argv;
	char	*dup;
	size_t	argv_len;

	dup = ft_strdup(value);
	if (!dup)
		return (NULL);
	argv_len = 0;
	while (argv && argv[argv_len])
		argv_len++;
	new_argv = malloc(sizeof(char *) * (argv_len + 2));
	if (!new_argv)
	{
		free(dup);
		return (NULL);
	}
	if (argv_len > 0)
		ft_memcpy(new_argv, argv, sizeof(char *) * argv_len);
	new_argv[argv_len] = dup;
	new_argv[argv_len + 1] = NULL;
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

static t_parse_error	parse_token_in_cmd(t_cmd *cmd, t_token **tokens,
		t_parse_error *error)
{
	t_redir	*redir;
	char	**new_argv;

	if ((*tokens)->type == TK_REDIR_IN || (*tokens)->type == TK_REDIR_OUT
		|| (*tokens)->type == TK_HEREDOC || (*tokens)->type == TK_APPEND)
	{
		redir = parse_redir(tokens, error);
		if (!redir)
			return (*error);
		append_redir(cmd, redir);
	}
	else if ((*tokens)->type == TK_WORD)
	{
		new_argv = add_argv(cmd->argv, (*tokens)->value);
		if (!new_argv)
			return (PARSE_ERR_INTERNAL);
		cmd->argv = new_argv;
	}
	return (PARSE_OK);
}

static t_cmd	*parse_command_loop(t_cmd *cmd, t_token **tokens,
		t_parse_error *error)
{
	t_parse_error	token_error;

	while (*tokens != NULL)
	{
		if ((*tokens)->type == TK_EOF)
			break ;
		else if ((*tokens)->type == TK_PIPE)
		{
			*tokens = (*tokens)->next;
			break ;
		}
		token_error = parse_token_in_cmd(cmd, tokens, error);
		if (token_error != PARSE_OK)
		{
			free_cmd(cmd);
			*error = token_error;
			return (NULL);
		}
		*tokens = (*tokens)->next;
	}
	return (cmd);
}

t_cmd	*parse_command(t_token **tokens, t_parse_error *error)
{
	t_cmd	*cmd;

	cmd = new_cmd();
	if (!cmd)
	{
		*error = PARSE_ERR_INTERNAL;
		return (NULL);
	}
	cmd = parse_command_loop(cmd, tokens, error);
	if (!cmd)
		return (NULL);
	*error = PARSE_OK;
	return (cmd);
}
