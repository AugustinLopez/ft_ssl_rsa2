/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cypher.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/03 14:57:02 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/17 12:46:11 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "cy_cmd.h"
#include "mylib/libft.h"
#include "mylib/ft_string.h"

static int check_with_next(char *src, char *next, t_cypher *cypher)
{
	char **tmp;

	cypher->option |= CYPHER_NEXT;
	if (ft_strcmp(src, "-i") == 0)
		tmp = &(cypher->av_input);
	else if (ft_strcmp(src, "-o") == 0)
		tmp = &(cypher->av_output);
	else if (ft_strcmp(src, "-k") == 0)
		tmp = &(cypher->av_key);
	else if (ft_strcmp(src, "-p") == 0)
		tmp = &(cypher->av_pass);
	else if (ft_strcmp(src, "-s") == 0)
		tmp = &(cypher->av_salt);
	else if (ft_strcmp(src, "-v") == 0)
		tmp = &(cypher->av_vector);
	else
		return (CYPHER_NOT_OPT);
	*tmp = next;
	if (next == NULL)
		return (CYPHER_NULL);
	return (0);
}

static int check_option(char *src, char *next, t_cypher *cypher)
{
	//first letter is '-'
	for (int i = 1; src[i] != 0; i++) {
		if (src[i] == 'd')
			cypher->option |= CYPHER_DECRYPT;
		else if (src[i] == 'e')
			cypher->option &= ~CYPHER_DECRYPT;
		else if (src[i] == 'a')
			cypher->option |= CYPHER_BASE64;
		else if (src[i] == 'P')
			cypher->option |= CYPHER_INFO;
		else if (src[i] == 'h') {
			cypher->option |= CYPHER_HELP;
			return (0);
		}
		else if (i == 1)
			return (check_with_next(src, next, cypher));
		else
			return (CYPHER_NOT_OPT);
	}
	return (0);
}

static t_cypher load_cypher_option(int ac, char **av)
{
	int i = 0;
	t_cypher ret;

	ft_memset(&ret, 0, sizeof(ret));
	ret.name = av[0];
	while (++i < ac) {
		if (av[i][0] != '-') {
			ret.err = CYPHER_NOT_OPT;
			break ;
		}
		if (i + 1 < ac)
			ret.err = check_option(av[i], av[i + 1], &ret);
		else
			ret.err = check_option(av[i], NULL, &ret);
		if (ret.err != 0 || (ret.option & CYPHER_HELP))
			break ;
		if (ret.option & CYPHER_NEXT) {
			ret.option &= ~CYPHER_NEXT;
			++i;
		}
	}
	if (ret.err == CYPHER_NOT_OPT)
		print_err(ret.name, av[i], "invalid option argument", 0);
	else if (ret.err == CYPHER_NULL)
		print_err(ret.name, av[i], "not followed by an argument", 0);
	return (ret);
}

int cypher_input(t_string **str, char *filename)
{
	int fd;

	if (filename) {
		fd = open(filename, O_RDONLY);
		if (fd < 0 || sfromfd(str, fd) == -1) {
			print_err("cypher", filename, 0, errno);
			if (fd >= 0 & close(fd) < 0)
				print_err("Warning", filename, 0, errno);
			return (-1);
		}
		if (close(fd) < 0)
			print_err("Warning", filename, 0, errno);
		return (0);
	}
	if (sfromfd(str, STDIN_FILENO) == -1) {
		print_err("cypher", 0, "stdin", errno);
		return (-1);
	}
	return (0);
}

int cypher_output(char *av_output, t_string *src, int is_ascii_64)
{
	int fd = STDOUT_FILENO;
	int check = 0;
	size_t i = slen(src) / 64;
	size_t j = slen(src) % 64;

	if (av_output) {
		fd = open(av_output, O_WRONLY | O_CREAT | O_TRUNC
		, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (fd < 0) {
			print_err("cypher", av_output, 0, errno);
			return (-1);
		}
	}
	if (is_ascii_64 == 0) {
		if (write(fd, sptr(src), slen(src)) < 0)
			check = -1;
	}
	else {
		for (size_t k = 0; k < i; k++) {
			if (write(fd, sptr(src) + k * 64, 64) < 0)
				check = -1;
			if (write(fd, "\n", 1) < 0)
				check = -1;
		}
		if (j) {
			if (write(fd, sptr(src) + i * 64, j) < 0)
				check = -1;
			if (write(fd, "\n", 1) < 0)
				check = -1;
		}
	}
	if (check == -1) {
		if (fd == STDOUT_FILENO)
			print_err("cypher", 0, "stdout", errno);
		else
			print_err("cypher", av_output, 0, errno);
	}
	if (av_output && close(fd) < 0)
		print_err("Warning", av_output, 0, errno);
	return (check);
}

int cypher(int ac, char **av, t_fun_cyp algo)
{
	t_cypher cypopt;
	int ret;

	cypopt = load_cypher_option(ac, av);
	if (cypopt.option & CYPHER_HELP) {
		write(STDOUT_FILENO, CYPHER_HELP_MSG, ft_strlen(CYPHER_HELP_MSG));
		return (0);
	}
	else if (cypopt.err != 0) {
		write(STDERR_FILENO, CYPHER_HELP_MSG, ft_strlen(CYPHER_HELP_MSG));
		return (-1);
	}
	ret = algo(&cypopt);
	if (ret < 0 && errno != 0)
		print_err(cypopt.name, 0,0, errno);
	return (ret);
}
