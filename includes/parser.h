/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 21:08:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/06 21:18:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "minishell.h"

typedef enum e_parse_error
{
	PARSE_OK,
	PARSE_ERR_INTERNAL,
	PARSE_ERR_PIPE_START,
	PARSE_ERR_PIPE_CONSECUTIVE,
	PARSE_ERR_PIPE_END,
	PARSE_ERR_REDIR_TARGET,
}	t_parse_error;

typedef struct s_parse_result
{
	t_token			*token;
	t_cmd			*cmds;
	t_parse_error	error;
}	t_parse_result;

t_parse_result	parser(t_token *tokens);
t_cmd			*parse_command(t_token **tokens, t_parse_error *error);
t_redir			*parse_redir(t_token **tokens, t_parse_error *error);
t_redir			*new_redir(t_token_type type);
void			append_cmd(t_cmd **head, t_cmd **tail, t_cmd *new_cmd);
void			append_redir(t_cmd *cmd, t_redir *redir);
char			**parser_add_argv(char **argv, char *value);
t_parse_error	syntax_error(t_parse_error error, t_token *token);
void			free_cmd(t_cmd *cmd);
void			free_cmds(t_cmd *cmd);

#endif
