/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_test.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takenakatakeshiichirouta <takenakatakes    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/08 20:44:09 by takenakatak       #+#    #+#             */
/*   Updated: 2026/05/08 20:44:09 by takenakatak      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_TEST_H
# define ENV_TEST_H

# include "env.h"
# include "test.h"

int		env_count(t_env *env);
t_env	*env_find(t_env *env, char *key);
int		env_array_len(char **array);
void	free_env_array(char **array);
int		capture_export_output(t_env *env, char *buffer, size_t size);
int		test_env_init(void);
int		test_env_set_insert(void);
int		test_env_set_update(void);
int		test_env_set_null_value(void);
int		test_env_unset(void);
int		test_env_invalid_args(void);
int		test_env_to_array(void);
int		test_split_key_value(void);
int		test_env_print_export(void);

#endif
