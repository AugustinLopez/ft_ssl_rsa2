/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   st_cmd.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/14 10:41:48 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/13 17:07:16 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ST_CMD_H
# define ST_CMD_H

#include <stdint.h>
#include "ft_standard.h"

# define GENRSA_HELP_MSG \
	"usage: genrsa [-h] [-i infile] [-o outfile]\n\n" \
	"-h      \tPrint this help\n" \
	"-i file \tSeed file for number generator (default /dev/urandom)\n" \
	"-o file \tOutput file to write to (default stdout)\n"

# define RSAUTL_HELP_MSG \
	"usage: rsautl [-in file] [-out file] [-inkey file] [-pubin] [-encrypt]" \
	" [-decrypt] [-hexdump]\n\n"

typedef struct	s_utl {
	t_string *sin;
	t_string *sout;
	t_string *skey;
	char *fin;
	char *fout;
	char *fkey;
	int fdout;
	int fdin;
	int fdkey;
	int pubin;
	int decrypt;
	int hexdump;
}				t_utl;

enum e_rsa_member{MODULO, PUBLIC, PRIVATE, PRIME1, PRIME2, EXP1, EXP2, COEF, RSA_MEMBER_COUNT};

typedef struct	s_rsa {
	uint8_t member[RSA_MEMBER_COUNT][512];
	int size[RSA_MEMBER_COUNT];
	int is_public;

}				t_rsa;

uint32_t find_prime32(uint32_t seed, int use_seed, int output);
int deterministic_miller_rabbin(uint64_t n);
int cmd_genrsa(int ac, char **av);
int cmd_rsautl(int ac, char **av);
int cmd_rsa(int ac, char **av);
#endif
