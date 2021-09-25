/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rsa_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 13:23:08 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/25 17:41:46 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include "mylib/libft.h"
#include "ft_cypher.h"
#include "st_cmd.h"
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

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

int readsequence(char *str, size_t *index, size_t *len)
{
	size_t j;

	if (str[(*index)++] != 0x30)
		return (-1);
	*len = (uint8_t)str[(*index)++];
	if (*len >= 0x80) {
		j = *len - 0x80;
		if (j > 2) //enough for 65536 bytes > more than enough for 4096 bits key
			return (-1);
		*len = 0;
		while (j-- > 0)
			*len = (*len << 8) + (uint8_t)str[(*index)++];
	}
	return (0);
}

int readnumber(char *str, size_t *index, uint8_t *num, int *numsize)
{
	int imax;
	size_t j;

	if (str[(*index)++] != 0x02)
		return (-1);
	imax = (uint8_t)str[(*index)++];
	if (imax >= 0x80) {
		j = imax - 0x80;
		if (j > 2) //enough for 65536 bytes, more than enough for 4096 bits key.
			return (-1);
		imax = 0;
		while (j-- > 0)
			imax = (imax << 8) + (uint8_t)str[(*index)++];
	}
	else if (imax == 0)
		return (-1);
	while (imax-- > 0) {
		if (*numsize == 0 && str[*index] == 0)
			++*index;
		else
			num[(*numsize)++] = (uint8_t)str[(*index)++];
	}
	return (0);
}

int rsa_load_pass(char *pass, char *buff, char *memory)
{
	int fd;
	int i;
	char *ptr;
	ssize_t len;

	if (pass == NULL || ft_strcmp(pass, "stdin") == 0)
		return (0);
	else if (ft_strncmp(pass, "pass:", 5) == 0) {
		if (pass[5] == 0)
			return (-1);
		ft_strncpy(buff, pass + 5, _SC_PASS_MAX);
		return (0);
	}
	else if (ft_strncmp(pass, "fd:", 3) == 0) {
		i = 3;
		fd = 0;
		while (pass[i]) {
			if (ft_strchr("0123456789", pass[i]) != NULL) {
				fd = fd * 10 + pass[i++] - '0';
			}
			else
				return (-1);
		}
		if (i == 3)
			return (-1);
		len = read(fd, buff, _SC_PASS_MAX);
		if (fd == STDIN_FILENO) {
			ptr = ft_strchr(buff, '\n');
			if (ptr)
				ptr[0] = '\0';
		}
		return (-1 * (len < 0));
	}
	else if (ft_strncmp(pass, "env:", 4) == 0) {
		ptr = getenv(pass + 4);
		if (ptr == NULL)
			return (-1);
		ft_strncpy(buff, ptr, _SC_PASS_MAX);
		return (0);
	}
	else if (ft_strncmp(pass, "file:", 5) == 0) {
		fd = fdinput(pass + 5);
		if (fd == -1)
			return (-1);
		len = read(fd, buff, _SC_PASS_MAX);
		if (len < 0) {
			close(fd);
			return (-1);
		}
		ptr = ft_strchr(buff, '\n');
		if (memory == pass) {
			while (len > 0 || ptr == NULL) {
				len = read(fd, buff, _SC_PASS_MAX);
				ptr = ft_strchr(buff, '\n');
			}
			if (ptr == NULL || len == -1) {
				close(fd);
				return (-1);
			}
			len = _SC_PASS_MAX - (ssize_t)(ptr - buff) + 1;
			if (len)
				ft_memmove(buff, ptr, len);
			else
				buff[0] = 0;
			len = read(fd, buff + len, _SC_PASS_MAX - len);
			if (len == -1 || buff[0] == 0) {
				close(fd);
				return (-1);
			}
		}
		ptr = ft_strchr(buff, '\n');
		if (ptr)
			ptr[0] = '\0';
		close(fd);
		return (0);
	}
	return (-1);
}

int rsa_load_key(t_rsa *rsa, char *pass)
{
	char	pwd[_SC_PASS_MAX + 8 + 16 + 1]; //+ salt + some place for 3key
	char	*digest;

	ft_memset(pwd, 0, _SC_PASS_MAX + 1);
	if (pass != NULL && ft_strcmp(pass, "stdin") != 0) {
		if (rsa_load_pass(pass, pwd, NULL) == -1)
			return (-1);
	}
	else if (load_password(pwd, rsa->decrypt, "PEM") == -1) {
		write(STDERR_FILENO, "bad password read\n", 18);
		return (-1);
	}
	if (rsa->decrypt == 0) {
		for (int i = 0; i < 8; i ++)
			rsa->salt[i] = (uint8_t)ft_rand();
	}
	digest = pbkdf((uint8_t *)pwd, ft_strlen(pwd), rsa->salt, rsa->key_count);
	if (digest == NULL)
		return (-1);
	for (int i = 0; i < rsa->key_count; i++) {
		if (c16_to_hex64(rsa->nkey + i * 8, digest + i * 16) == -1) {
			free(digest);
			write(STDERR_FILENO, "non-hex_digit\ninvalid hex key value\n", 36);
			return (-1);
		}
	}
	free(digest);
	return (0);
}


