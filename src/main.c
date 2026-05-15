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
	int				expand_error;

	tokens = lexer(line);
	if (!tokens)
		return ;
	result = parser(tokens);
	if (result.error != PARSE_OK)
	{
		handle_parse_error(result, tokens, shell);
		return ;
	}
	expand_error = expander(result.cmds, shell);
	if (expand_error)
	{
		shell->last_status = 1;
		free_cmds(result.cmds);
		free_tokens(tokens);
		return ;
	}
	debug_print_cmds(result.cmds);
	free_cmds(result.cmds);
	free_tokens(tokens);
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
