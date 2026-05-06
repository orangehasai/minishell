/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 16:56:58 by takenakatak       #+#    #+#             */
/*   Updated: 2026/03/21 23:14:07 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "lexer.h"
#include "parser.h"

volatile sig_atomic_t	g_signal;

static void	handle_parse_error(t_parse_result result, t_token *tokens,
		t_shell *shell)
{
	if (result.error == PARSE_ERR_INTERNAL)
		shell->last_status = 1;
	else
	{
		syntax_error(result.error, result.token);
		shell->last_status = 2;
	}
	free_tokens(tokens);
}

static void	process_line(char *line, t_shell *shell)
{
	t_token			*tokens;
	t_parse_result	result;

	tokens = lexer(line);
	if (!tokens)
		return ;
	result = parser(tokens);
	if (result.error != PARSE_OK)
	{
		handle_parse_error(result, tokens, shell);
		return ;
	}
	free_cmds(result.cmds);
	free_tokens(tokens);
}

int	main(void)
{
	char	*line;
	t_shell	shell;

	shell.last_status = 0;
	while (1)
	{
		line = readline("minishell$ ");
		if (!line)
			break ;
		if (!*line)
		{
			free(line);
			continue ;
		}
		process_line(line, &shell);
		free(line);
	}
	return (0);
}
