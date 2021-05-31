/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   av_digest.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 16:45:40 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/31 17:56:20 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "ft_digest.h"
#include "mylib/libft.h"

static t_digest	add_list_arg(t_digest dig, char *add, int is_file)
{
	t_list_arg	*elem;

	if (add == NULL) {
		dig.err = DIGEST_NULL;
		return (dig);
	}
	elem = (t_list_arg *)malloc(sizeof(t_list_arg));
	if (elem == NULL) {
		dig.err = DIGEST_ERRNO;
		return (dig);
	}
	elem->next = NULL;
	elem->str = add;
	elem->is_file = is_file;
	if (dig.end == NULL)
		dig.begin = elem;
	else
		dig.end->next = elem;
	dig.end = elem;
	return (dig);
}

void	free_digest(t_digest *dig)
{
	t_list_arg	*elem;
	t_list_arg	*tmp;

	elem = dig->begin;
	while (elem) {
		tmp = elem;
		elem = elem->next;
		free(tmp);
		tmp = NULL;
	}
}

static int		check_option(char *src, uint8_t *option)
{
	int	i;

	i = 0;
	while (src[++i])
	{
		if (src[i] == 'p')
			*option |= DIGEST_ECHO;
		else if (src[i] == 'q')
			*option |= DIGEST_QUIET;
		else if (src[i] == 'r')
			*option |= DIGEST_REVERSE;
		else if (src[i] == 'h')
		{
			*option |= DIGEST_HELP;
			break ;
		}
		else if (src[i] == 's')
			return (DIGEST_INVALID_S);
		else
			return (DIGEST_NOT_OPT);
	}
	return (0);
}

t_digest	load_digest_option(int ac, char **av)
{
	t_digest	ret;
	char		*next;
	int			stop;
	int			i;

	i = 0;
	stop = 0;
	ft_memset(&ret, 0, sizeof(ret));
	while (++i < ac)
	{
		next = NULL;
		if (i + 1 < ac)
			next = av[i + 1];
		if (stop == 0 && ft_strcmp(av[i], "--") == 0)
			stop = 1;
		else if (stop == 0 && av[i][0] == '-' && ft_strcmp(av[i], "-s") != 0)
			ret.err = check_option(av[i], &(ret.option));
		else if (stop == 0 && ft_strcmp(av[i], "-s") == 0 && ++i)
			ret = add_list_arg(ret, next, 0);
		else
			ret = add_list_arg(ret, av[i], 1);
		if (ret.err != 0 || (ret.option & DIGEST_HELP))
			break ;
	}
	return (ret);
}

static void print_dig_err(int err)
{
	char *tmp;

	if (err == DIGEST_NOT_OPT)
		write(STDERR_FILENO, "illegal option\n", 15);
	else if (err == DIGEST_NULL)
		write(STDERR_FILENO, "option 's' requires an argument\n", 32);
	else if (err == DIGEST_INVALID_S)
		write(STDERR_FILENO, "incorrect use of option 's'\n", 28);
	else {
		tmp = strerror(errno);
		write(STDERR_FILENO, tmp, ft_strlen(tmp));
		write(STDERR_FILENO, "\n", 1);
	}
}

static void digest_stdin(t_fun_dig algo, t_digest argv)
{
	
}

int digest(int ac, char **av, t_fun_dig algo)
{
	t_digest argv;
	t_list_arg *elem;

	argv = load_digest_option(ac, av);
	if (argv.option & DIGEST_HELP) {
		write(STDOUT_FILENO, DIGEST_HELP_MSG, ft_strlen(DIGEST_HELP_MSG));
		free_digest(&argv);
		return (0);
	}
	else if (argv.err != 0) {
		print_dig_err(argv.err);
		write(STDERR_FILENO, DIGEST_HELP_MSG, ft_strlen(DIGEST_HELP_MSG));
		free_digest(&argv);
		return (-1);
	}
	if ((argv.option & DIGEST_ECHO) || argv.begin == NULL)
		digest_stdin(algo, argv);
/*	for (elem = argv.begin; elem; elem = elem->next) {
		if (elem->is_file)
			loop_file(algorithm, argv, elem);
		else
			loop_text(algorithm, argv, elem);
	}*/
	free_digest(&argv);
	return (0);
}
