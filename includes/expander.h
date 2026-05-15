/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 16:57:10 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/15 01:19:15 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPANDER_H
# define EXPANDER_H
# define ORIGIN_INPUT '1'
# define ORIGIN_EXPANDED '0'

# include "env.h"
# include "minishell.h"

typedef enum e_quote_state
{
	QUOTE_NONE,
	QUOTE_SINGLE,
	QUOTE_DOUBLE,
}					t_quote_state;

typedef struct s_expand_result
{
	char			*expanded;
	char			*origin;
}					t_expand_result;

typedef struct s_expand_ctx
{
	const char		*input;
	t_quote_state	quote_state;
	t_shell			*shell;
	t_expand_result	result;
}					t_expand_ctx;

typedef struct s_remove_ctx
{
	const char		*input;
	const char		*origin;
	char			*removed_quote_input;
	t_quote_state	current_quote_state;
}					t_remove_ctx;

t_quote_state		update_quote_state(t_quote_state quote, char c);
char				*expand_dollar(const char *input, size_t *i,
						t_shell *shell);
int					append_literal(char **out, char c);
int					append_repeated_char(char **out, char c, size_t count);
int					append_expanded(char **out, char *expanded);
int					expand_str(const char *input, t_shell *shell,
						t_expand_result *result);
char				*remove_quotes(const char *input, const char *origin);
int					expander(t_cmd *cmds, t_shell *shell);

#endif
