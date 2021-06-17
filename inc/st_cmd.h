/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   st_cmd.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/14 10:41:48 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/17 13:35:31 by aulopez          ###   ########.fr       */
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

uint32_t find_prime32(uint32_t seed, int use_seed, int output);
int deterministic_miller_rabbin(uint64_t n);
int cmd_genrsa(int ac, char **av);
#endif
