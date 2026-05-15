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
#include "expander.h"
#include "lexer.h"
#include "parser.h"

volatile sig_atomic_t	g_signal = 0;

static void	handle_lexer_error(t_shell *shell, t_lexer_error error)
{
	if (error == LEXER_ERR_UNCLOSED_QUOTE)
	{
		ft_putstr_fd("minishell: syntax error: unclosed quote\n", 2);
		shell->last_status = 2;
	}
	else
	{
		ft_putstr_fd("minishell: internal error\n", 2);
		shell->last_status = 1;
	}
}

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

static int	handle_expand_stage(t_parse_result result, t_token *tokens,
		t_shell *shell)
{
	int	expand_error;

	expand_error = expander(result.cmds, shell);
	if (expand_error)
	{
		shell->last_status = 1;
		free_cmds(result.cmds);
		free_tokens(tokens);
		return (0);
	}
	debug_print_cmds(result.cmds);
	free_cmds(result.cmds);
	free_tokens(tokens);
	return (1);
}

static void	process_line(char *line, t_shell *shell)
{
	t_lexer_result	lexer_result;
	t_parse_result	parser_result;

	lexer_result = lexer(line);
	if (!lexer_result.tokens)
	{
		handle_lexer_error(shell, lexer_result.error);
		return ;
	}
	parser_result = parser(lexer_result.tokens);
	if (parser_result.error != PARSE_OK)
	{
		handle_parse_error(parser_result, lexer_result.tokens, shell);
		return ;
	}
	if (!handle_expand_stage(parser_result, lexer_result.tokens, shell))
		return ;
	shell->last_status = 0;
}

static int	init_shell(t_shell *shell, char **envp)
{
	shell->env = NULL;
	if (env_init(&shell->env, envp))
		return (1);
	shell->last_status = 0;
	return (0);
}

static void	run_shell(t_shell *shell)
{
	char	*line;

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
	if (init_shell(&shell, envp))
		return (1);
	run_shell(&shell);
	env_free(shell.env);
	clear_history();
	return (shell.last_status);
}
