/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cypher.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 16:45:40 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/07 16:19:18 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_CYPHER_H
# define FT_CYPHER_H

#include "base64.h"
#include <stdint.h>

# define CYPHER_DECRYPT 0x1
# define CYPHER_BASE64 0x2
# define CYPHER_IV 0x4
# define CYPHER_NOPAD 0x8
# define CYPHER_3KEY 0x10
# define CYPHER_HELP 0x20
# define CYPHER_INFO 0x40
# define CYPHER_NEXT 0x80

# define CYPHER_NOT_OPT -1
# define CYPHER_NULL -2

typedef struct s_cypher
{
	char		*name;
	char		*av_salt;
	char		*av_pass;
	char		*av_key;
	char		*av_vector;
	char		*av_input;
	char		*av_output;
	uint8_t		nkey[3][8];
	uint8_t		nr56key[3][17][7];
	uint8_t		nr48key[3][16][7];
	uint8_t		vector[8];
	uint8_t		salt[8];
	uint8_t		option;
	int8_t		err;
}				t_cypher;

# define CYPHER_HELP_MSG \
	"usage: [cyphername] [-deahP] [-i/o file] [-p text] [-/k/s/v hex]\n\n" \
	"-d      \tDecrypt the input data\n" \
	"-e      \tEncrypt the input data (default)\n" \
	"-a      \tPerform base64 encoding/decoding\n" \
	"-h      \tPrint this help in stdin (instead of stderr)\n" \
	"-P      \tPrint out the salt, key and IV used, then exit\n" \
	"        \t  (no encryption or decryption is performed)\n" \
	"-i file \tInput file to read from (default stdin)\n" \
	"-o file \tOutput file to write to (default stdout)\n" \
	"-p pass \tPassword to use, specified as an ascii string\n" \
	"-k key  \tKey to use, specified as a hexadecimal string\n" \
	"-s salt \tSalt to use, specified as a hexadecimal string\n" \
	"-v IV   \tIV to use, specified as a hexadecimal string\n"

typedef int (* t_fun_cyp)(t_cypher *);

int cypher(int ac, char **av, t_fun_cyp algo);
int cypher_output(char *av_output, t_string *src, int is_ascii_64);
int cypher_input(t_string **str, char *filename);

int base64_cypher(t_cypher *arg);
int des_ecb_cypher(t_cypher *arg);
#endif
