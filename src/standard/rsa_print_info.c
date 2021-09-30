/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rsa_print_info.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/30 12:41:56 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mylib/libft.h"
#include "st_cmd.h"
#include <unistd.h>
#include <errno.h>

void print_modulus(int fd, uint8_t *number, int size) {
	char buff[2];

	write(fd, "Modulus=",8);
	for (int i = 0; i < size; i++) {
		buff[0] = number[i] / 16;
		buff[1] = number[i] % 16;
		buff[0] += buff[0] < 10 ? '0': 'A' - 10;
		buff[1] += buff[1] < 10 ? '0': 'A' - 10;
		write(fd, buff, 2);
	}
	write(fd, "\n", 1);
}

static void ft_numwrite(int fd, uint64_t n) {
	size_t len;
	size_t save;
	uint64_t m;
	char buff[30];

	len = 1;
	m = n;
	while ((m /= 10))
		++len;
	buff[len] = 0;
	save = len;
	while (len > 0) {
		buff[--len] = '0' + n % 10;
		n /= 10;
	}
	write(fd, buff, save);
}

static inline void small_number(int fd, uint8_t *number, size_t len) {
	uint64_t temp = 0;
	char buff[2];

	for (size_t i = 0; i < len; i++)
		temp = (temp << 8) + number[i];
	ft_numwrite(fd, temp);
	write(fd, " (0x", 4);
	for (size_t i = 0; i < len; i++) {
		buff[0] = number[i] / 16;
		buff[1] = number[i] % 16;
		buff[0] += buff[0] < 10 ? '0': 'a' - 10;
		buff[1] += buff[1] < 10 ? '0': 'a' - 10;
		write(fd, buff, 2);
	}
	write(fd, ")\n", 2);
}

static inline void large_number(int fd, uint8_t *number, size_t len) {
	size_t last = len - 1;
	int padded = 0;
	char buff[3];

	buff[2] = ':';
	for (size_t i = 0; i < len; i++) {
		if ((i + padded) % 15 == 0)
			write(fd, "\n    ", 5);
		buff[0] = number[i] / 16;
		buff[1] = number[i] % 16;
		if (i == 0 && buff[0] > 8) {
			write(fd, "00:", 3);
			padded = 1;
		}
		buff[0] += buff[0] < 10 ? '0': 'a' - 10;
		buff[1] += buff[1] < 10 ? '0': 'a' - 10;
		if (i < last)
			write(fd, buff, 3);
		else
			write(fd, buff, 2);
	}
	write(fd, "\n", 1);
}

static void print_number(int fd, uint8_t *number, size_t len)
{
	if (len <= 8)
		small_number(fd, number, len);
	else
		large_number(fd, number, len);
}

void print_text(int fd, t_rsa *rsa, int pubin)
{
	if (pubin == 1) {
		write(fd, "Public-Key : (", 14);
		ft_numwrite(fd, (uint64_t)(rsa->size[MODULO] * 8));
		write(fd, " bit)\nModulus: ", 15);
		print_number(fd, rsa->member[MODULO], rsa->size[MODULO]);
		write(fd, "Exponent: ", 10);
		print_number(fd, rsa->member[PUBLIC], rsa->size[PUBLIC]);
		return;
	}
	else {
		write(fd, "Private-Key : (", 15);
		ft_numwrite(fd, (uint64_t)(rsa->size[MODULO] * 8));
		write(fd, " bit)\nmodulus: ", 15);
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

void print_check(int fd, t_rsa *rsa)
{
	uint64_t member[RSA_MEMBER_COUNT];
	int err = 0;

	for (int i = 0; i < RSA_MEMBER_COUNT; i++) {
		if (rsa->size[i] > 8) {
			print_err(NULL, NULL, "Cannot analyze RSA element above 64 bit", 0);
			return ;
		}
		member[i] = 0;
		for (int j = 0; j < rsa->size[i]; j++)
			member[i] = (member[i] << 8) + rsa->member[i][j];
	}
	if ((member[PRIME2] == 0 ||
		member[PRIME1] > 0xffffffffffffffff / member[PRIME2] ||
		member[PRIME1] * member[PRIME2] != member[MODULO]) && ++err)
		write(fd, "n does not equal pq\n", 20);
	if (deterministic_miller_rabbin(member[PRIME1]) == 0 && ++err)
		write(fd, "p not a prime\n", 14);
	if (deterministic_miller_rabbin(member[PRIME2]) == 0 && ++err)
		write(fd, "q not a prime\n", 14);
	if (member[PRIVATE] != modmulinv(member[PUBLIC], (member[PRIME1] - 1) * (member[PRIME2] - 1)) && ++err)
		write(fd, "private exponent not mod mult inv of e, d\n", 42);
	if (member[EXP1] != member[PRIVATE] % (member[PRIME1] - 1) && ++err)
		write(fd, "exp1 does not equal d mod p - 1\n", 32);
	if (member[EXP2] != member[PRIVATE] % (member[PRIME2] - 1) && ++err)
		write(fd, "exp2 does not equal d mod q - 1\n", 32);
	if (member[COEF] != modmulinv(member[PRIME2], member[PRIME1]) && ++err)
		write(fd, "coefficient not mod mult inv of q, p\n", 37);
	if (err == 0)
		write(fd, "RSA key ok\n", 11);
}
