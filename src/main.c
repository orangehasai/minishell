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

#include "env.h"
#include "executor.h"
#include "expander.h"
#include "lexer.h"
#include "parser.h"
#include "signals.h"

volatile sig_atomic_t	g_signal = 0;

static void	handle_error(t_shell *shell, t_token *tokens,
		t_lexer_error lexer_error, t_parse_result *parse_result)
{
	if (lexer_error == LEXER_ERR_UNCLOSED_QUOTE)
	{
		ft_putstr_fd("minishell: syntax error: unclosed quote\n", 2);
		shell->last_status = 2;
	}
	else if (lexer_error == LEXER_ERR_INTERNAL)
	{
		ft_putstr_fd("minishell: internal error\n", 2);
		shell->last_status = 1;
	}
	else if (parse_result->error == PARSE_ERR_INTERNAL)
		shell->last_status = 1;
	else
	{
		syntax_error(parse_result->error, parse_result->token);
		shell->last_status = 2;
	}
	free_tokens(tokens);
}

static void	handle_expand_stage(t_parse_result parser_result, t_token *tokens,
		t_shell *shell)
{
	if (expander(parser_result.cmds, shell))
	{
		shell->last_status = 1;
		free_cmds(parser_result.cmds);
		free_tokens(tokens);
		return ;
	}
	if (parser_result.cmds)
		shell->last_status = execute(parser_result.cmds, shell);
	free_cmds(parser_result.cmds);
	free_tokens(tokens);
}

static void	process_line(char *line, t_shell *shell)
{
	t_lexer_result	lexer_result;
	t_parse_result	parser_result;

	lexer_result = lexer(line);
	if (!lexer_result.tokens)
	{
		handle_error(shell, NULL, lexer_result.error, NULL);
		return ;
	}
	parser_result = parser(lexer_result.tokens);
	if (parser_result.error != PARSE_OK)
	{
		handle_error(shell, lexer_result.tokens, LEXER_OK, &parser_result);
		return ;
	}
	handle_expand_stage(parser_result, lexer_result.tokens, shell);
}

static void	run_shell(t_shell *shell)
{
	char	*line;

	if (isatty(STDIN_FILENO))
		setup_signals_interactive();
	while (1)
	{
		line = readline("minishell$ ");
		if (!line)
		{
			if (isatty(STDIN_FILENO))
				ft_putendl_fd("exit", STDOUT_FILENO);
			break ;
		}
		if (!*line)
		{
			free(line);
			continue ;
		}
		add_history(line);
		process_line(line, shell);
		free(line);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	shell.env = NULL;
	if (env_init(&shell.env, envp))
		return (1);
	shell.last_status = 0;
	run_shell(&shell);
	env_free(shell.env);
	clear_history();
	return (shell.last_status);
}
