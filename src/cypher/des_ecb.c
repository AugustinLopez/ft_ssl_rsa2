/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_ecb.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 13:00:56 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/07 20:26:01 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pwd.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include "mylib/ft_string.h"
#include "mylib/libft.h"
#include "ft_digest.h"
#include "ft_cypher.h"

static inline int	char_to_hex(char c)
{
	if ('0' <= c && c <= '9')
		return (c - '0');
	else if ('a' <= c && c <= 'f')
		return (c - 'a' + 10);
	else if ('A' <= c && c <= 'F')
		return (c - 'A' + 10);
	else
		return (-1);
}

int		c16_to_hex64(uint8_t *hex, char *src)
{
	uint8_t	tmp[16];
	int		right;
	int		left;
	size_t	len;

	//Up to 16 characters are read. If less than 16 are read, the rest is filled
	//with '0'. Characters must be valid hexadecimal digit.
	len = 0;
	ft_memset(tmp, '0', 16);
	while (*(src + len) != 0 && len < 16) {
		if (ft_strchr("0123456789abcdefABCDEF", *(src + len)) == NULL)
			return (-1);
		tmp[len] = src[len];
		 ++len;
	}
	len = 0;
	while (len < 8) {
		left = char_to_hex(tmp[len * 2]);
		right = char_to_hex(tmp[len * 2 + 1]);
		hex[len++] = left * 16 + right;
	}
	return (0);
}

int		key_provided(t_cypher *arg)
{
	int		len;

	len = (arg->option & CYPHER_3KEY) ? 3 : 1;
	for (int i = 0; i < len; i++) {
		if (c16_to_hex64((uint8_t *)arg->nkey[i], arg->av_key + 16 * i) == -1) {
			write(STDERR_FILENO, "non-hex_digit\ninvalid hex key value\n", 36);
			return (-1);
		}
	}
	if (!(arg->option & CYPHER_IV))
		return (0);
	if (arg->av_vector == NULL) {
		write(STDERR_FILENO, "iv undefined\n", 13);
		return (-1);
	}
	if (c16_to_hex64((uint8_t *)arg->vector, arg->av_vector) == -1) {
		write(STDERR_FILENO, "non-hex_digit\ninvalid hex vector value\n", 39);
		return (-1);
	}
	return (0);
}

char		*pbkdf(uint8_t *pwd, size_t len, uint8_t *salt, int key)
{
	char		*tmp;
	char		*tmp2;
	char		*tmp3;
	
	//We need 16 bytes per key + 16 bytes for the IV. MD5 is 32 bytes.
	//1 key: pbkdf is MD5(PWD | SALT);
	ft_memcpy(pwd + len, salt, 8);
	tmp = md5((char *)pwd, len + 8);
	if (tmp == NULL || key < 3 || ft_strlen(tmp) >= 64)
		return (tmp);

	//3 keys: pbkdf is MD5((MD5(PWD | SALT)) | PWD | SALT);
	ft_memmove(pwd + 16, pwd, len + 8);
	ft_memset(pwd, 0, 16);
	if (c16_to_hex64(pwd, tmp) == -1 || c16_to_hex64(pwd + 8, tmp + 16) == -1) {
		free(tmp);
		return (NULL);
	}
	tmp2 = md5((char *)pwd, len + 8 + 16);
	if (tmp2 == NULL) {
		free(tmp);
		return (NULL);
	}
	tmp3 = malloc(33 * sizeof(char));
	if (tmp3) {
		ft_strncpy(tmp3, tmp, 16);
		ft_strncpy(tmp3 + 16, tmp2, 16);
		tmp3[32] = 0;
	}
	free(tmp);
	free(tmp2);
	return (tmp3);
}

int			load_password(char *dest, int decrypt, char *algo)
{
	char	ver[_SC_PASS_MAX + 1];
	char	prompt[128];
	size_t  n;

	ft_memset(ver, 0, _SC_PASS_MAX + 1);
	ft_memset(prompt, 0, 128);
	n = ft_strlen(algo);
	ft_strncat(prompt, "Verifying - enter ", 18);
	ft_strncat(prompt + 18, algo, n);
	ft_strncat(prompt + 18 + n, " decryption password: ", 22);
	prompt[18 + n + 22] = 0;
	if (decrypt != 0) {
		ft_strncpy(dest, getpass(prompt), _SC_PASS_MAX);
		if (dest[0] == '\0')
			return (-1);
		return (0);
	}
	prompt[18 + n + 1] = 'e';
	prompt[18 + n + 2] = 'n';
	ft_strncpy(dest, getpass(prompt + 12), _SC_PASS_MAX);
	if (dest[0] != '\0')
		ft_strncpy(ver, getpass(prompt), _SC_PASS_MAX);
	if (dest[0] == '\0' || ver[0] == '\0')
		return (-1);
	else if (ft_strcmp(dest, ver) != 0) {
		write(STDERR_FILENO, "Verify failure\n", 15);
		return (-1);
	}
	return (0);
}

static int	dpassword_provided(t_cypher *arg, t_string **input)
{
	char	pwd[_SC_PASS_MAX + 8 + 16 + 1]; //+ salt + some place for 3key
	int		n;
	t_string *tmp;
	char *digest;
	size_t len;

	//password
	ft_memset(pwd, 0, _SC_PASS_MAX + 1);
	if (arg->av_pass != NULL)
		ft_strncpy(pwd, arg->av_pass, _SC_PASS_MAX);
	else if (load_password(pwd, arg->option & CYPHER_DECRYPT, arg->name) == -1) {
		write(STDERR_FILENO, "bad password read\n", 18);
		return (-1);
	}

	//read text (needed for salt. Could optimize but too much error handling
	if (arg->option & CYPHER_BASE64) {
		if (cypher_input(&tmp, arg->av_input) == -1)
			return (-1);
		n = decode_base64(input, sptr(tmp), slen(tmp));
		sfree(tmp);
	}
	else
		n = cypher_input(input, arg->av_input);
	if (n == -1)
		return (-1);
	
	//salt
	len = ft_strlen(pwd);
	if (slen(*input) < 8 || ft_strcmp(sptr(*input), "Salted__") != 0 || slen(*input) < 16) {
		write(STDERR_FILENO, "bad magic number\n", 17);
		sdel(input);
		return (-1);
	}
	ft_memcpy(arg->salt, sptr(*input) + 8, 8);
	ft_memcpy(pwd + len, arg->salt, 8);
	//remove salt from saved input
	if (srlead(*input, 16) == 1)
		return (-1);

	//convert pass and salt.
	n = (arg->option & CYPHER_3KEY) ? 3 : 1;
	digest = pbkdf((uint8_t *)pwd, len, arg->salt, n);
	if (digest == NULL)
		return (-1);

	//Put the result in our key(s) and IV (if not provided/required).
	arg->av_key = digest;
	if (arg->av_vector == NULL)
		arg->av_vector = arg->av_key + n * 16;
	n = key_provided(arg);
	free(digest);
	if (arg->av_vector == arg->av_key + n * 16)
		arg->av_vector = NULL;
	arg->av_key = NULL;
	return (n);
}

static int	epassword_provided(t_cypher *arg)
{
	char	pwd[_SC_PASS_MAX + 8 + 16 + 1]; //+ salt + some place for 3key
	char	*tmp;
	size_t	len;
	int		n;

	//password
	ft_memset(pwd, 0, _SC_PASS_MAX + 1);
	if (arg->av_pass != NULL)
		ft_strncpy(pwd, arg->av_pass, _SC_PASS_MAX);
	else if (load_password(pwd, arg->option & CYPHER_DECRYPT, arg->name) == -1) {
		write(STDERR_FILENO, "bad password read\n", 18);
		return (-1);
	}

	//salt
	len = ft_strlen(pwd);
	if (arg->av_salt == NULL) {
		for (int i = 0; i < 8; i ++)
			arg->salt[i] = (uint8_t)ft_rand();
	}
	else if (c16_to_hex64((uint8_t *)arg->salt, arg->av_salt) == -1) {
		write(STDERR_FILENO, "non-hex digit\ninvalid hex salt value\n", 37);
		return (-1);
	}

	//convert pass and salt.
	n = (arg->option & CYPHER_3KEY) ? 3 : 1;
	tmp = pbkdf((uint8_t *)pwd, len, arg->salt, n);
	if (tmp == NULL)
		return (-1);

	//Put the result in our key(s) and IV (if not provided/required).
	arg->av_key = tmp;
	if (arg->av_vector == NULL)
		arg->av_vector = tmp + n * 16;
	n = key_provided(arg);
	free(tmp);
	arg->av_key = NULL;
	if (arg->av_vector == tmp + n * 16)
		arg->av_vector = NULL;
	return (n);
}

static int	pkcs7_padding(t_string **topad)
{
	char	val;

	val  = 8 - slen(*topad) % 8;
	for (int len = val; len > 0; len --) {
		if (satrail(*topad, &val, 1) == -1) {
			sdel(topad);
			return (-1);
		}
	}
	return (0);
}

int		read_tocrypt_des(t_cypher *arg, t_string **input)
{
	if (arg->av_key != NULL && key_provided(arg) == -1)
		return (-1);
	else if (arg->av_key == NULL && epassword_provided(arg) == -1)
		return (-1);
	else if (arg->option & CYPHER_INFO)
		return (0);
	if (arg->av_key == NULL && arg->av_output == NULL && !(arg->option & CYPHER_BASE64)) {
		write(STDOUT_FILENO, "Salted__", 8);
		write(STDOUT_FILENO, arg->salt, 8);
	}
	if (cypher_input(input, arg->av_input) == -1)
		return (-1);
	if (arg->option & CYPHER_NOPAD)
		return (0);
	return (pkcs7_padding(input));
}

int		read_todecrypt_des(t_cypher *arg, t_string **input)
{
	t_string *tmp;
	int n;

	if (arg->av_key != NULL && key_provided(arg) == -1)
		return (-1);
	else if (arg->av_key == NULL) {
		if (dpassword_provided(arg, input) == -1)
			return (-1);
	if (arg->option & CYPHER_INFO)
		sfree(*input);
		return (0);
	}
	if (arg->option & CYPHER_INFO)
		return (0);
	if (arg->option & CYPHER_BASE64) {
		if (cypher_input(&tmp, arg->av_input) == -1)
			return (-1);
		n = decode_base64(input, sptr(tmp), slen(tmp));
		sfree(tmp);
	}
	else
		n = cypher_input(input, arg->av_input);
	if (n == -1)
		return (-1);
	return (n);
}

static void	des_print_info(t_cypher *arg)
{
	int	n;
	uint8_t *tab[5];
	char buffer[16];
	uint8_t c;

	ft_memset(tab, 0, 5 * sizeof(*tab));
	n = (arg->option & CYPHER_3KEY) ? 3 : 1;
	for (int i = 0 ; i < n; i++)
		tab[i] = arg->nkey[i];
	if (arg->option & CYPHER_IV)
		tab[3] = arg->vector;
	if (arg->av_key == NULL)
		tab[4] = arg->salt;
	for (int i = 0; i < 5; i++) {
		if (tab[i] == 0)
			continue ;
		else if (i == 0)
			write(STDOUT_FILENO, "key=", 4);
		else if (i == 3)
			write(STDOUT_FILENO, "\niv =", 5);
		else if (i == 4)
			write(STDOUT_FILENO, "\nsalt=", 6);
		ft_memset(buffer, 0, 16);
		for (int j = 0; j < 8; j++) {
			c = (tab[i][j] >> 4);
			if (c < 10)
				buffer[j * 2] = '0' + c;
			else
				buffer[j * 2] = 'A' + c - 10;
			c = (tab[i][j] & 0xf);
			if (c < 10)
				buffer[j * 2 + 1] = '0' + c;
			else
				buffer[j * 2 + 1] = 'A' + c - 10;
		}
		write(STDOUT_FILENO, buffer, 16);
	}
	write(STDOUT_FILENO, "\n", 1);
}

int		des_ecb_cypher(t_cypher *arg)
{
	t_string	*input;
//	t_string	*output;
	int			ret;

	if (arg->option & CYPHER_DECRYPT)
		ret = read_todecrypt_des(arg, &input);
	else
		ret = read_tocrypt_des(arg, &input);
	if (ret == -1)
		return (-1);
	if (arg->option & CYPHER_INFO) {
		des_print_info(arg);
		return (0);
	}
	sfree(input);
	return (0);
/*	if (arg->option & CYPHER_DECRYPT)
		ret = prepare_decrypted_des(arg, input, &output);
	//algo();
	sfree(input);
	if (ret == -1)
		return (-1);
	if (arg->option & CYPHER_DECRYPT)
		ret = cypher_output(arg->av_output, output, 0);
	else
		ret = cypher_output(arg->av_output, output, 1);
	sfree(output);
	return (ret);*/
}
