/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   md5.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/27 18:58:36 by aulopez           #+#    #+#             */
/*   Updated: 2021/03/15 17:57:26 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MD5_H
# define MD5_H

# include <stddef.h>
# include <stdint.h>
# include "common/ssl_string.h"

typedef struct s_md5
{
	size_t		chunk;
	uint32_t	a0;
	uint32_t	b0;
	uint32_t	c0;
	uint32_t	d0;
	uint32_t	a;
	uint32_t	b;
	uint32_t	c;
	uint32_t	d;
	uint32_t	g;
	uint32_t	f;
}				t_md5;

t_string	dig_preprocessing(uint8_t *tocrypt, size_t len, int little_endian);
void		*uint8to32(uint32_t *dest, uint8_t *src, size_t len, int endian);
char		*md5(uint8_t *tocrypt, size_t len);
#endif
