/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cypher.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 16:45:40 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/10 22:07:49 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_CYPHER_H
# define FT_CYPHER_H

#include <stdint.h>
#include <stddef.h>
#include "mylib/ft_string.h"

int	decode_base64(t_string **str, char *src, size_t len);
int encode_base64(t_string **str, char *src, size_t len);

typedef struct s_cypher t_cypher;
int base64_cypher(t_cypher *arg);
int des_ecb_cypher(t_cypher *arg);
int des3_ecb_cypher(t_cypher *arg);
int des_cbc_cypher(t_cypher *arg);
int des3_cbc_cypher(t_cypher *arg);
int des_cfb_cypher(t_cypher *arg);
int des3_cfb_cypher(t_cypher *arg);
int des_ofb_cypher(t_cypher *arg);
int des3_ofb_cypher(t_cypher *arg);
#endif
