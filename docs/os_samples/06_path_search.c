#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char	*join_path(const char *dir, const char *cmd)
{
	char	*full_path;
	size_t	len;

	len = strlen(dir) + strlen(cmd) + 2;
	full_path = malloc(len);
	if (!full_path)
		return (NULL);
	strcpy(full_path, dir);
	strcat(full_path, "/");
	strcat(full_path, cmd);
	return (full_path);
}

static char	*search_in_path(const char *cmd, const char *path_env)
{
	char	*paths;
	char	*dir;
	char	*full_path;

	paths = strdup(path_env);
	if (!paths)
		return (NULL);
	dir = strtok(paths, ":");
	while (dir)
	{
		full_path = join_path(dir, cmd);
		if (!full_path)
			break ;
		if (access(full_path, X_OK) == 0)
			return (free(paths), full_path);
		free(full_path);
		dir = strtok(NULL, ":");
	}
	free(paths);
	return (NULL);
}

int	main(int argc, char **argv)
{
	char		*cmd;
	char		*path_env;
	char		*full_path;

	if (argc != 2)
		return (printf("usage: %s <command>\n", argv[0]), 1);
	cmd = argv[1];
	if (strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (printf("direct path: %s\n", cmd), 0);
		return (printf("cannot execute: %s\n", cmd), 1);
	}
	path_env = getenv("PATH");
	if (!path_env)
		return (printf("PATH is not set\n"), 1);
	full_path = search_in_path(cmd, path_env);
	if (!full_path)
		return (printf("command not found in PATH: %s\n", cmd), 1);
	printf("found: %s\n", full_path);
	free(full_path);
	return (0);
}
