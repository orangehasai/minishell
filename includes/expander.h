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

typedef enum e_quote_state
{
	QUOTE_NONE,
	QUOTE_SINGLE,
	QUOTE_DOUBLE,
}	t_quote_state;

typedef struct s_remove_ctx
{
	const char		*input;
	const char		*origin;
	char			**removed_quote_input;
	t_quote_state	*current_quote_state;
}	t_remove_ctx;

typedef struct s_expand_out
{
	char	**formatted_input;
	char	**origin;
	t_shell	*shell;
}	t_expand_out;

char			*env_get(t_env *env, char *key);
t_quote_state	update_quote_state(t_quote_state quote, char c);
char			*expand_dollar(const char *input, size_t *i, t_shell *shell);
int				append_literal(char **out, char c);
int				append_expanded(char **out, char *expanded);
char			*expand_str(const char *input, t_shell *shell, char **origin);
char			*remove_quotes(const char *input, const char *origin);
int				expand_replace_value(char **value, t_shell *shell);
int				expander(t_cmd *cmds, t_shell *shell);

#endif
