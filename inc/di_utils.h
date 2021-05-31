/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   di_utils.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/27 19:09:44 by aulopez           #+#    #+#             */
/*   Updated: 2021/06/01 00:04:20 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DI_UTILS_H
# define DI_UTILS_H

#include <stddef.h>
#include <stdint.h>

void swap_byte(uint8_t *a, uint8_t *b);
uint32_t l_32rot(uint32_t x, size_t shift);
uint32_t r_32rot(uint32_t x, size_t shift);
uint64_t l_64rot(uint64_t x, size_t shift);
uint64_t r_64rot(uint64_t x, size_t shift);
void *uint8to32(uint32_t *dest, uint8_t *src, size_t len, int little_endian);
void *uint8to64(uint64_t *dest, uint8_t *src, size_t len, int little_endian);
char *di_preprocessing(size_t *padded_len, char *tocrypt, size_t len, int little_endian);
char *di_lpreprocessing(size_t *padded_len, char *tocrypt, size_t len, int little_endian);
#endif
