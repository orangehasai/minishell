/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_command.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 21:08:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/05 21:08:00 by skeita           ###   ########.fr       */
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

t_cmd	*parse_command(t_token **tokens)
{
	t_cmd	*cmd;
	char	**new_argv;

	cmd = new_cmd();
	if (!cmd)
		return (NULL);
	while (*tokens != NULL)
	{
		if ((*tokens)->type == TK_EOF)
			break ;
		if ((*tokens)->type == TK_PIPE)
		{
			*tokens = (*tokens)->next;
			break ;
		}
		new_argv = add_argv(cmd->argv, (*tokens)->value);
		if (!new_argv)
		{
			free_cmd(cmd);
			return (NULL);
		}
		cmd->argv = new_argv;
		*tokens = (*tokens)->next;
	}
	return (cmd);
}
