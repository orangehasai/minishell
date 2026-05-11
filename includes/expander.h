/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 16:57:10 by takenakatak       #+#    #+#             */
/*   Updated: 2026/03/21 16:57:11 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPANDER_H
# define EXPANDER_H

# include "minishell.h"
# include "env.h"

typedef enum e_quote_state
{
	QUOTE_NONE,
	QUOTE_SINGLE,
	QUOTE_DOUBLE,
}	t_quote_state;

t_quote_state	update_quote_state(t_quote_state quote, char c);
char			*expand_dollar(const char *input, size_t *i, t_shell *shell);
int				append_literal(char **out, char c);
int				append_expanded(char **out, char *expanded);
char			*expand_str(const char *input, t_shell *shell);
int				expander(t_cmd *cmds, t_shell *shell);

#endif
