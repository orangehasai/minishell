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

volatile sig_atomic_t	g_signal;

int	main(void)
{
	char	*line;
	t_token	*tokens;

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
	return (0);
}
