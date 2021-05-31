/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   digest.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 16:45:40 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/31 22:30:59 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include "di_cmd.h"
#include "mylib/libft.h"
#include "mylib/ft_string.h"

static int	add_list_arg(t_digest *dig, char *add, int is_file)
{
	t_list_arg	*elem;

	if (add == NULL) {
		dig->err = DIGEST_NULL;
		return (-1);
	}
	elem = (t_list_arg *)malloc(sizeof(t_list_arg));
	if (elem == NULL) {
		dig->err = DIGEST_ERRNO;
		return (-1);
	}
	elem->next = NULL;
	elem->str = add;
	elem->is_file = is_file;
	if (dig->end == NULL)
		dig->begin = elem;
	else
		dig->end->next = elem;
	dig->end = elem;
	return (0);
}

static void	free_digest(t_digest *dig)
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

static int	check_option(char *src, uint8_t *option)
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

static t_digest	load_digest_option(int ac, char **av)
{
	t_digest	ret;
	char		*next;
	int			stop = 0;
	int			i = 0;
	int			j = 0;

	ft_memset(&ret, 0, sizeof(ret));
	ret.name = av[0];
	while (++i < ac) {
		next = NULL;
		if (i + 1 < ac)
			next = av[i + 1];
		if (stop == 0 && ft_strcmp(av[i], "--") == 0)
			stop = 1;
		else if (stop == 0 && av[i][0] == '-' && ft_strcmp(av[i], "-s") != 0)
			ret.err = check_option(av[i], &(ret.option));
		else if (stop == 0 && ft_strcmp(av[i], "-s") == 0 && ++i)
			j = add_list_arg(&ret, next, 0);
		else {
			j = add_list_arg(&ret, av[i], 1);
			stop = 1;
		}
		if (j == -1 || (ret.option & DIGEST_HELP))
			break ;
	}
	return (ret);
}

static void print_dig_err(int err)
{
	if (err == DIGEST_NOT_OPT)
		write(STDERR_FILENO, "illegal option\n", 15);
	else if (err == DIGEST_NULL)
		write(STDERR_FILENO, "option 's' requires an argument\n", 32);
	else if (err == DIGEST_INVALID_S)
		write(STDERR_FILENO, "incorrect use of option 's'\n", 28);
	else
		print_err(0, 0, 0, errno);
}

static void dig_source_stdin(t_fun_dig algo, t_digest *digopt)
{
	t_string *str;
	char *crypt;
	int ret = 0;

	if (sfromfd(&str, STDIN_FILENO) == -1) {
		print_err(digopt->name, 0, "stdin", errno);
		return ;
	}
	else if (digopt->option & DIGEST_ECHO)
		write(STDOUT_FILENO, sptr(str), slen(str));
	crypt = algo(sptr(str), slen(str));
	if (crypt == NULL) {
		print_err(digopt->name, 0, "stdin", errno);
		sfree(str);
		return ;
	}
	sclear(str);
	ret = ret == -1 ? -1 : satrail(str, crypt, ft_strlen(crypt));
	ret = ret == -1 ? -1 : satrail(str, "\n", 1);
	if (ret == -1) {
		print_err(digopt->name, 0, "stdin", errno);
		return ;
	}
	write(STDOUT_FILENO, sptr(str), slen(str));
	free(crypt);
	sfree(str);
}

static void dig_source_text(t_fun_dig algo, t_digest *digopt, t_list_arg *elem)
{
	t_string *str;
	char *crypt;
	int ret = 0;

	if (salloc(&str, elem->str, ft_strlen(elem->str)) == -1) {
		print_err(digopt->name, elem->str, 0, errno);
		return ;
	}
	crypt = algo(sptr(str), slen(str));
	if (crypt == NULL) {
		print_err(digopt->name, 0, "stdin", errno);
		sfree(str);
		return ;
	}
	sclear(str);
	if (digopt->option & DIGEST_QUIET)
		ret = ret == -1 ? -1 : satrail(str, crypt, ft_strlen(crypt));
	else if (digopt->option & DIGEST_REVERSE) {
		ret = ret == -1 ? -1 : satrail(str, crypt, ft_strlen(crypt));
		ret = ret == -1 ? -1 : satrail(str, " \"", 2);
		ret = ret == -1 ? -1 : satrail(str, elem->str, ft_strlen(elem->str));
		ret = ret == -1 ? -1 : satrail(str, "\"", 1);
	}
	else {
		ret = ret == -1 ? -1 : satrail(str, digopt->name, ft_strlen(digopt->name));
		ret = ret == -1 ? -1 : satrail(str, " (\"", 3);
		ret = ret == -1 ? -1 : satrail(str, elem->str, ft_strlen(elem->str));
		ret = ret == -1 ? -1 : satrail(str, "\") = ", 5);
		ret = ret == -1 ? -1 : satrail(str, crypt, ft_strlen(crypt));
	}
	ret = ret == -1 ? -1 : satrail(str, "\n", 1);
	if (ret == -1) {
		print_err(digopt->name, 0, "stdin", errno);
		return ;
	}
	write(STDOUT_FILENO, sptr(str), slen(str));
	free(crypt);
	sfree(str);
}

static void dig_source_file(t_fun_dig algo, t_digest *digopt, t_list_arg *elem)
{
	t_string *str;
	int fd;
	int ret = 0;
	char *crypt;

	fd = open(elem->str, O_RDONLY);
	if (fd < 0 || sfromfd(&str, fd) == -1) {
		print_err(digopt->name, elem->str, 0, errno);
		if (fd >= 0 && close(fd) < 0)
			print_err("Warning", elem->str, 0, errno);
		return ;
	}
	if (close(fd) < 0)
		print_err("Warning", elem->str, 0, errno);
	crypt = algo(sptr(str), slen(str));
	if (crypt == NULL) {
		print_err(digopt->name, 0, "stdin", errno);
		sfree(str);
		return ;
	}
	sclear(str);
	if (digopt->option & DIGEST_QUIET)
		ret = ret == -1 ? -1 : satrail(str, crypt, ft_strlen(crypt));
	else if (digopt->option & DIGEST_REVERSE) {
		ret = ret == -1 ? -1 : satrail(str, crypt, ft_strlen(crypt));
		ret = ret == -1 ? -1 : satrail(str, " ", 1);
		ret = ret == -1 ? -1 : satrail(str, elem->str, ft_strlen(elem->str));
	}
	else {
		ret = ret == -1 ? -1 : satrail(str, digopt->name, ft_strlen(digopt->name));
		ret = ret == -1 ? -1 : satrail(str, " (", 2);
		ret = ret == -1 ? -1 : satrail(str, elem->str, ft_strlen(elem->str));
		ret = ret == -1 ? -1 : satrail(str, ") = ", 4);
		ret = ret == -1 ? -1 : satrail(str, crypt, ft_strlen(crypt));
	}
	ret = ret == -1 ? -1 : satrail(str, "\n", 1);
	if (ret == -1) {
		print_err(digopt->name, 0, "stdin", errno);
		return ;
	}
	write(STDOUT_FILENO, sptr(str), slen(str));
	free(crypt);
	sfree(str);
}

int digest(int ac, char **av, t_fun_dig algo)
{
	t_digest digopt;
	t_list_arg *elem;

	digopt = load_digest_option(ac, av);
	if (digopt.option & DIGEST_HELP) {
		write(STDOUT_FILENO, DIGEST_HELP_MSG, ft_strlen(DIGEST_HELP_MSG));
		free_digest(&digopt);
		return (0);
	}
	else if (digopt.err != 0) {
		print_dig_err(digopt.err);
		write(STDERR_FILENO, DIGEST_HELP_MSG, ft_strlen(DIGEST_HELP_MSG));
		free_digest(&digopt);
		return (-1);
	}
	if ((digopt.option & DIGEST_ECHO) || digopt.begin == NULL)
		dig_source_stdin(algo, &digopt);
	for (elem = digopt.begin; elem; elem = elem->next) {
		if (elem->is_file)
			dig_source_file(algo, &digopt, elem);
		else
			dig_source_text(algo, &digopt, elem);
	}
	free_digest(&digopt);
	return (0);
}
