/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 16:57:12 by takenakatak       #+#    #+#             */
/*   Updated: 2026/03/21 16:57:13 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include "minishell.h"

typedef enum e_lex_state
{
	NORMAL,
	SINGLE_QUOTE,
	DOUBLE_QUOTE,
}	t_lex_state;

t_token	*lexer(char *line);
t_token	*read_word(char *line, size_t *i);
t_token	*read_operator(char *line, size_t *i);
t_token	*new_token(t_token_type type, char *value);
void	append_token(t_token **head, t_token **tail, t_token *new_tok);
void	free_tokens(t_token *tokens);
int		is_whitespace(char c);
int		is_operator(char c);
void	skip_whitespace(char *line, size_t *i);
void	debug_print_tokens(t_token *tokens);
char	*token_type_to_str(t_token_type type);

#endif
