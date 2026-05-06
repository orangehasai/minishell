/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 21:08:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/06 13:33:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "minishell.h"

t_cmd	*parser(t_token *tokens);
t_cmd	*parse_command(t_token **tokens);
t_redir	*parse_redir(t_token **tokens);
t_redir	*new_redir(t_token_type type);
void	append_cmd(t_cmd **head, t_cmd **tail, t_cmd *new_cmd);
void	append_redir(t_cmd *cmd, t_redir *redir);
void	free_cmd(t_cmd *cmd);
void	free_cmds(t_cmd *cmd);

#endif
