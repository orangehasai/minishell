/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 16:56:58 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/09 02:05:31 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"
#include "minishell.h"
#include "lexer.h"

volatile sig_atomic_t	g_signal = 0;

/*
** Future pipeline entry:
** lexer(line) -> parser(tokens, shell) -> expand(cmd, shell)
** -> execute(cmd, shell)
*/
static void	handle_line(char *line, t_shell *shell)
{
	if (*line)
		add_history(line);
	(void)shell;
}

static int	init_shell(t_shell *shell, char **envp)
{
	shell->env = env_init(envp);
	if (!shell->env && errno == ENOMEM)
		return (1);
	shell->last_status = 0;
}

static void	run_shell(t_shell *shell)
{
	char	*line;
	t_token	*tokens;

	while (1)
	{
		line = readline("minishell$ ");
		if (!line)
		{
			if (isatty(STDIN_FILENO))
				ft_putendl_fd("exit", STDOUT_FILENO);
			break ;
		}
		handle_line(line, shell);
		tokens = lexer(line);
		if (!tokens)
		{
			free(line);
			continue ;
		}
		debug_print_tokens(tokens);
		free_tokens(tokens);
		free(line);
	}
}

static void	cleanup_shell(t_shell *shell)
{
	env_free(shell->env);
	clear_history();
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	if (init_shell(&shell, envp))
		return (1);
	run_shell(&shell);
	cleanup_shell(&shell);
	return (shell.last_status);
}
