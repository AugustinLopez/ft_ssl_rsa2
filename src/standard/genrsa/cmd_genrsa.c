/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_genrsa.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/10/04 08:15:24 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/ft_funinfo.h"
#include "mylib/libft.h"
#include "mylib/ft_string.h"
#include "st_cmd.h"
#include "ft_cypher.h"
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>

static int parsing(int ac, char **av, char **in, char **out) {

	for (int i = 1; i < ac; i++) {
		if (ft_strcmp(av[i], "-h") == 0) {
			write(STDOUT_FILENO, GENRSA_HELP_MSG, ft_strlen(GENRSA_HELP_MSG));
			return (0);
		}
		else if (i == ac - 1) {
			print_err("genrsa", av[i], "invalid use of option", 0);
			return (-1);
		}
		else if (ft_strcmp(av[i], "-i") == 0)
			*in = av[++i];
		else if (ft_strcmp(av[i], "-o") == 0)
			*out = av[++i];
		else {
			print_err("genrsa", av[i], "invalid option", 0);
			return (-1);
		}
	}
	return (1);
}

#include <stdio.h>

static int nbr_in(char *in, uint64_t *a, uint64_t *b) {
	int fdin;

	if (in == NULL) {
		*a = find_prime32(0, 0, 1, 0);
		*b = find_prime32(0, 0, 1, *a);
	}
	else {
		fdin = fdinput(in, "genrsa");
		if (fdin < 0)
			return (-1);
		if (read(fdin, a, 8) < 0) {
			print_err("genrsa", in, 0, errno);
			if (fdin != STDIN_FILENO && close(fdin) < 0)
				print_err("Warning", in, 0, errno);
			return (-1);
		}
		if (fdin != STDIN_FILENO && close(fdin) < 0)
			print_err("Warning", in, 0, errno);
		*a = *a & 0xffff;
		*b = (*a >> 32) & 0xffff;
		*a = find_prime32(*a, 1, 1, 0);
		*b = find_prime32(*b, 1, 1, *a);
	}
	return (0);
}

static void print_rsa_private(t_string *str, char *out)
{
	int fdout;

	fdout = fdoutput(out, "genrsa");
	if (fdout == -1)
		return ;
	write(fdout, "-----BEGIN RSA PRIVATE KEY-----\n", 32);
	while (slen(str)) {
		if (slen(str) > 64)
			write(fdout, sptr(str), 64);
		else
			write(fdout, sptr(str), slen(str));
		srlead(str, 64);
		write(fdout, "\n", 1);
	}
	write(fdout, "-----END RSA PRIVATE KEY-----\n", 30);
	if (fdout != STDOUT_FILENO  && close(fdout) < 0)
		print_err("Warning", out, 0, errno);
}

int cmd_genrsa(int ac, char **av)
{
	uint64_t p = 0;
	uint64_t q = 0;
	char *fin = NULL;
	char *fout = NULL;
	t_string *str;
	int ret;

	ret= parsing(ac, av, &fin, &fout);
	if (ret != 1)
		return (ret);
	write(STDERR_FILENO, "Generating RSA private key, 64 bit long modulus\n", 48);
	if (nbr_in(fin, &p, &q) == -1)
		return (-1);
	write(STDERR_FILENO, "e is 65537 (0x10001)\n", 21);
	str = genrsa(p, q, 0x010001);
	if (str == NULL) {
		print_err("genrsa", NULL, NULL, errno);
		return (-1);
	}
	print_rsa_private(str, fout);
	sfree(str);
	return (0);
}
