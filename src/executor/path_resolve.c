/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_resolve.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/16 14:50:00 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/16 14:50:00 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

static char	*path_error(int error_code)
{
	errno = error_code;
	return (NULL);
}

static char	*build_candidate(char *path_env, size_t start, size_t len,
		char *cmd)
{
	char	*candidate;
	size_t	cmd_len;

	if (len == 0)
		return (ft_strdup(cmd));
	cmd_len = ft_strlen(cmd);
	candidate = malloc(sizeof(char) * (len + cmd_len + 2));
	if (!candidate)
		return (NULL);
	ft_memcpy(candidate, path_env + start, len);
	candidate[len] = '/';
	ft_memcpy(candidate + len + 1, cmd, cmd_len);
	candidate[len + cmd_len + 1] = '\0';
	return (candidate);
}

static int	is_executable(char *candidate, int *denied)
{
	if (access(candidate, X_OK) == 0)
		return (1);
	if (errno == EACCES)
		*denied = 1;
	return (0);
}

static char	*search_in_path(char *path_env, char *cmd)
{
	size_t	start;
	size_t	end;
	char	*candidate;
	int		denied;

	start = 0;
	end = 0;
	denied = 0;
	while (1)
	{
		while (path_env[end] && path_env[end] != ':')
			end++;
		candidate = build_candidate(path_env, start, end - start, cmd);
		if (!candidate)
			return (NULL);
		if (is_executable(candidate, &denied))
			return (candidate);
		free(candidate);
		if (!path_env[end])
			break ;
		start = ++end;
	}
	if (denied)
		return (path_error(EACCES));
	return (path_error(ENOENT));
}

char	*resolve_path(char *cmd, t_shell *shell)
{
	char	*path_env;

	if (!cmd || !*cmd)
		return (path_error(ENOENT));
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	if (!shell)
		return (path_error(EINVAL));
	path_env = env_get(shell->env, "PATH");
	if (!path_env)
		return (path_error(ENOENT));
	return (search_in_path(path_env, cmd));
}
