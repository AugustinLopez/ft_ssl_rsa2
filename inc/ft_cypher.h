/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cypher.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 16:45:40 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/11 01:38:43 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_CYPHER_H
# define FT_CYPHER_H

#include <stdint.h>
#include <stddef.h>
#include "mylib/ft_string.h"

int	decode_base64(t_string **str, char *src, size_t len);
int encode_base64(t_string **str, char *src, size_t len);
void des_ecb(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
void des3_ecb(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
void des_cbc(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
void des3_cbc(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
void des_ofb(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
void des3_ofb(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
void des_cfb(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
void des3_cfb(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
#endif
