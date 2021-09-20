/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_rsa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/20 17:16:38 by aulopez          ###   ########.fr       */
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

static int fdoutput(char *out) {
	int fdout;

	if (out == NULL)
		fdout = STDOUT_FILENO;
	else {
		fdout = open(out, O_WRONLY | O_CREAT | O_TRUNC,
		S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (fdout < 0) {
			print_err("rsa", out, 0, errno);
			return (-1);
		}
	}
	return (fdout);
}

static int fdinput(char *in) {
	int fdin;
	
	if (in == NULL)
		fdin = STDIN_FILENO;
	else {
		fdin = open(in, O_RDONLY);
		if (fdin < 0) {
			print_err("rsa", in, 0, errno);
			return (-1);
		}
	}
	return (fdin);
}

static int parsing(int ac, char **av, t_sslrsa *arg) {
	int errmem;
	char *fin;
	char *fout;
	fin = NULL;
	fout = NULL;

	for (int i = 1; i < ac; i++) {
		if (ft_strcmp(av[i], "-h") == 0) {
			write(STDOUT_FILENO, RSAUTL_HELP_MSG, ft_strlen(RSAUTL_HELP_MSG));
			return (0);
		}
		else if ((ft_strcmp(av[i], "-inform") == 0 || ft_strcmp(av[i], "-outform") == 0)
		&& i < ac - 1 && ft_strcmp(av[++i], "PEM") == 0)
			continue;
		else if (ft_strcmp(av[i], "-in") == 0 && i < ac - 1)
			fin = av[++i];
		else if (ft_strcmp(av[i], "-out") == 0 && i < ac - 1)
			fout = av[++i];
		else if (ft_strcmp(av[i], "-passin") == 0 && i < ac - 1)
			arg->argin = av[++i];
		else if (ft_strcmp(av[i], "-passout") == 0 && i < ac - 1)
			arg->argout = av[++i];
		else if (ft_strcmp(av[i], "-pubin") == 0)
			arg->pubin = 1;
		else if (ft_strcmp(av[i], "-pubout") == 0)
			arg->pubout = 1;
		else if (ft_strcmp(av[i], "-des") == 0)
			arg->des = 1;
		else if (ft_strcmp(av[i], "-text") == 0)
			arg->text = 1;
		else if (ft_strcmp(av[i], "-noout") == 0)
			arg->noout = 1;
		else if (ft_strcmp(av[i], "-modulus") == 0)
			arg->modulus = 1;
		else if (ft_strcmp(av[i], "-check") == 0)
			arg->check = 1;
		else {
			print_err("rsa", av[i], "invalid option", 0);
			return (-1);
		}
	}
	arg->fdin = fdinput(fin);
	arg->fdout = fdoutput(fout);
	if (arg->fdin == -1 || arg->fdout == -1) {
		errmem = errno;
		close(arg->fdin);
		close(arg->fdout);
		errno = errmem;
		return (-1);
	}
	return (1);
}

static int free_sslrsa(t_sslrsa *arg, int ret)
{
	int errmem;

	errmem = errno;
	sdel(&(arg->sin));
	sdel(&(arg->sout));
	close(arg->fdin);
	close(arg->fdout);
	errno = errmem;
	return (ret);
}

static void print_number(int fd, uint8_t *number, size_t len)
{
	uint64_t temp;
	size_t i;
	size_t tmp;
	char j;

	if (len <= 8) {
		temp = 0;
		for (i = 0; i < len; i++)
			temp = (temp << 8) + number[i];
		ft_numwrite(fd, temp);
		write(fd, " (0x", 4);
		for (i = 0; i < len; i++) {
			j = number[i] / 16;
			j = j < 10 ? j + '0': j - 10 + 'a';
			if (!(i == 0 && j == '0'))
				write(fd, &j, 1);
			j = number[i] % 16;
			j = j < 10 ? j + '0': j - 10 + 'a';
			write(fd, &j, 1);
		}
		write(fd, ")\n", 2);
	}
	else {
		tmp = 0;
		for (i = 0; i < len; i++) {
			if ((i + tmp) % 15 == 0)
				write(fd, "\n    ", 5);
			j = number[i] / 16;
			if (i == 0 && j > 8) {
				write(fd, "00:", 3);
				tmp = 1;
			}
			j = j < 10 ? j + '0': j - 10 + 'a';
			write(fd, &j, 1);
			j = number[i] % 16;
			j = j < 10 ? j + '0': j - 10 + 'a';
			write(fd, &j, 1);
			if (i < len - 1)
				write(fd, ":", 1);
		}
		write(fd, "\n", 1);
	}
}

void print_text(int fd, t_rsa *rsa, int pubin)
{
	if (pubin == 1) {
		write(fd, "Public-Key : (", 14);
		ft_numwrite(fd, (uint64_t)(rsa->size[MODULO] * 8));
		write(fd, " bit)\n", 6);
		write(fd, "Modulus: ", 9);
		print_number(fd, rsa->member[MODULO], rsa->size[MODULO]);
		write(fd, "Exponent: ", 10);
		print_number(fd, rsa->member[PUBLIC], rsa->size[PUBLIC]);
		return;
	}
	else {
		write(fd, "Private-Key : (", 15);
		ft_numwrite(fd, (uint64_t)(rsa->size[MODULO] * 8));
		write(fd, " bit)\n", 6);
		write(fd, "modulus: ", 9);
		print_number(fd, rsa->member[MODULO], rsa->size[MODULO]);
		write(fd, "publicExponent: ", 16);
		print_number(fd, rsa->member[PUBLIC], rsa->size[PUBLIC]);
		write(fd, "privateExponent: ", 17);
		print_number(fd, rsa->member[PRIVATE], rsa->size[PRIVATE]);
		write(fd, "prime1: ", 8);
		print_number(fd, rsa->member[PRIME1], rsa->size[PRIME1]);
		write(fd, "prime2: ", 8);
		print_number(fd, rsa->member[PRIME2], rsa->size[PRIME2]);
		write(fd, "exponent1: ", 11);
		print_number(fd, rsa->member[EXP1], rsa->size[EXP1]);
		write(fd, "exponent2: ", 11);
		print_number(fd, rsa->member[EXP2], rsa->size[EXP2]);
		write(fd, "coefficient: ", 13);
		print_number(fd, rsa->member[COEF], rsa->size[COEF]);
	}
}

void print_modulus(int fd, uint8_t *number, int size)
{
	char j;
	int i;

	write(fd, "Modulus=",8);
	for (i = 0; i < size; i++) {
		j = number[i] / 16;
		j = j < 10 ? j + '0': j - 10 + 'A';
		write(fd, &j, 1);
		j = number[i] % 16;
		j = j < 10 ? j + '0': j - 10 + 'A';
		write(fd, &j, 1);
	}
	write(fd, "\n", 1);
}

void print_check(int fd, t_rsa *rsa)
{
	uint64_t pq;
	uint64_t p;
	uint64_t q;
	int i;

	if (rsa->size[MODULO] > 8) {
		print_err(NULL, NULL, "Check not available for key size above 64 bit", 0);
		return ;
	}
	if (rsa->size[PRIME1] > 8 || rsa->size[PRIME2] > 8) {
		print_err(NULL, NULL, "Prime sizes inconsistent with key size", 0);
		return ;
	}
	p = 0;
	q = 0;
	pq = 0;
	for (i = 0; i < rsa->size[PRIME1]; i++)
		p = (p << 8) + rsa->member[PRIME1][i];
	for (i = 0; i < rsa->size[PRIME2]; i++)
		q = (q << 8) + rsa->member[PRIME2][i];
	for (i = 0; i < rsa->size[MODULO]; i++)
		pq = (pq << 8) + rsa->member[MODULO][i];
	if (p > 0xffffffffffffffff / q)
		print_err(NULL, NULL, "Prime sizes inconsistent with key size", 0);
	if (p * q == pq)
		write(fd, "RSA key ok\n", 11);
	else
		write(fd, "RSA key KO: n does not equal p q\n", 33);
}

int cmd_rsa(int ac, char **av)
{
	int i;
	t_sslrsa arg;
	t_rsa rsa;

	ft_memset(&arg, 0, sizeof(arg));
	ft_memset(&rsa, 0, sizeof(rsa));
	i = parsing(ac, av, &arg);
	if (i != 1)
		return (i);
	if (arg.pubin == 1 && arg.check == 1) {
		print_err(NULL, NULL, "Only private keys can be checked\n", 0);
	}
	if (sfromfd(&(arg.sin), arg.fdin) == -1)
		return (free_sslrsa(&arg, -1));
	if (decode_rsa(&arg, &rsa) == -1) {
		if (arg.pubin == 0)
			print_err(NULL, NULL, "unable to load Private Key", 0);
		else
			print_err(NULL, NULL, "unable to load Public Key", 0);
		return (free_sslrsa(&arg, -1));
	}
	if (arg.text == 1)
		print_text(STDOUT_FILENO, &rsa, arg.pubin);
	if (arg.modulus == 1)
		print_modulus(STDOUT_FILENO, rsa.member[MODULO], rsa.size[MODULO]);
	if (arg.check == 1)
		print_check(arg.fdout, &rsa);
/*	if (arg.noout == 0) {
		write(STDOUT_FILENO, "writing RSA key\n", 16);
		write(arg.fdout, sptr(arg.sin), slen(arg.sin));
	}*/
	return (free_sslrsa(&arg, 0));
}
