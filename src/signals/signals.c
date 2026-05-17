/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skeita <skeita@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/17 13:20:00 by skeita            #+#    #+#             */
/*   Updated: 2026/05/17 13:20:00 by skeita           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "signals.h"

static void	handle_sigint_interactive(int signum)
{
	(void)signum;
	g_signal = SIGINT;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

static void	set_signal_handler(int signum, void (*handler)(int))
{
	struct sigaction	sa;

	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sigaction(signum, &sa, NULL);
}

void	setup_signals_interactive(void)
{
	set_signal_handler(SIGINT, handle_sigint_interactive);
	set_signal_handler(SIGQUIT, SIG_IGN);
}
