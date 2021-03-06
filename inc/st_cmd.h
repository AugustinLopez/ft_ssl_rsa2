/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   st_cmd.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/14 10:41:48 by aulopez           #+#    #+#             */
/*   Updated: 2021/10/04 08:17:17 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ST_CMD_H
# define ST_CMD_H

#include <stdint.h>
#include "ft_standard.h"
#include "mylib/ft_string.h"

# define GENRSA_HELP_MSG \
	"usage: genrsa [-h] [-i infile] [-o outfile]\n\n" \
	"-h      \tPrint this help\n" \
	"-i file \tSeed file for number generator (default /dev/urandom)\n" \
	"-o file \tOutput file to write to (default stdout)\n"

# define RSAUTL_HELP_MSG \
	"usage: rsautl [-in file] [-out file] [-inkey file] [-pubin] [-encrypt]" \
	" [-decrypt] [-hexdump]\n\n"

# define RSA_HELP_MSG \
	"usage: rsa [-h] [-in key] [-out file] [-pubin] [-pubout] [-des] [-des3]" \
	"[-passin arg] [-passout arg] [-modulus] [-text] [-noout] [-check]\n\n" \
	"-h           \tPrint this help\n" \
	"-in key      \tRSA key to analyze (default: Private)\n" \
	"-out file    \tOutput file for the key (default: same as input key)\n" \
	"-pubin       \tInput is a public key\n" \
	"-pubout      \tOutput is a public key\n" \
	"-des/des3    \tEncode the input with DES-CBC/DES-EDE3-CBC\n" \
	"-passin arg  \tPassword argument for DES to decode the input\n" \
	"-passout arg \tPassword argument for DES to encode the output\n" \
	"-modulus     \tPrint the RSA modulus\n" \
	"-text        \tPrint the RSA data\n" \
	"-noout       \tDo not write the output on the STDOUT file descriptor\n" \
	"-check       \tCheck the consistency of the RSA data\n"
	
typedef struct	s_sslrsa {
	t_string *sin;
	t_string *sout;
	t_string *skey;
	char *argin;
	char *argout;
	int fdout;
	int fdin;
	int fdkey;
	int decrypt;
	int hexdump;
	int pubin;
	int pubout;
	int des;
	int text;
	int noout;
	int modulus;
	int check;
}				t_sslrsa;

enum e_rsa_member{MODULO, PUBLIC, PRIVATE, PRIME1, PRIME2, EXP1, EXP2, COEF, RSA_MEMBER_COUNT};

typedef struct	s_rsa {
	uint8_t member[RSA_MEMBER_COUNT][512];
	int size[RSA_MEMBER_COUNT];
	uint8_t nkey[24];
	uint8_t salt[8];
	int key_count;
	int decrypt;
}				t_rsa;

uint32_t find_prime32(uint32_t seed, int use_seed, int output, uint32_t ignore);
int deterministic_miller_rabbin(uint64_t n);
uint64_t modmulinv(uint64_t a, uint64_t b);
uint64_t power_mod(uint64_t a, uint64_t b, uint64_t mod);
void print_text(int fd, t_rsa *rsa, int pubin);
void print_check(int fd, t_rsa *rsa);
void print_modulus(int fd, uint8_t *number, int size);

int cmd_genrsa(int ac, char **av);
int cmd_rsautl(int ac, char **av);
int cmd_rsa(int ac, char **av);
int rsa_decrypt(t_rsa *rsa, t_sslrsa *arg, size_t start, size_t end);
int decode_rsa(t_sslrsa *arg, t_rsa *rsa);
int encode_rsa(t_sslrsa *arg, t_rsa *rsa);
int decode_private_rsa(t_sslrsa *arg, t_rsa *rsa);
int decode_private(t_sslrsa *arg, t_rsa *rsa);
int readsequence(char *str, size_t *index, size_t *len, size_t max);
int readnumber(char *str, size_t *index, uint8_t *num, int *numsize, size_t max);
int rsa_load_key(t_rsa *rsa, char *pass, char *memory);
int rsa_load_pass(char *pass, char *buff, char *memory);
#endif
