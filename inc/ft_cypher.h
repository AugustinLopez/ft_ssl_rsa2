/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cypher.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 16:45:40 by aulopez           #+#    #+#             */
/*   Updated: 2021/09/23 16:14:48 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_CYPHER_H
# define FT_CYPHER_H

#include <stdint.h>
#include <stddef.h>
#include "mylib/ft_string.h"

int	decode_base64(t_string **str, char *src, size_t len);
int encode_base64(t_string **str, char *src, size_t len);
int load_password(char *dest, int decrypt, char *algo);
char *pbkdf(uint8_t *pwd, size_t len, uint8_t *salt, int key);
int c16_to_hex64(uint8_t *hex, char *src);
void des_ecb(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
void des3_ecb(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
void des_cbc(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
void des3_cbc(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
void des_ofb(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
void des3_ofb(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
void des_cfb(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
void des3_cfb(uint8_t *str, size_t len, uint8_t *nkey, uint8_t *vector, int decrypt);
int pkcs7_padding(t_string **topad);
#endif
