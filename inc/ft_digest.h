/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_digest.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/31 16:45:40 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/31 21:50:16 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_DIGEST_H
# define FT_DIGEST_H

#include <stddef.h>

char		*md5(char *tocrypt, size_t len);
char		*sha224(char *tocrypt, size_t len);
char		*sha256(char *tocrypt, size_t len);
char		*sha384(char *tocrypt, size_t len);
char		*sha512(char *tocrypt, size_t len);
char		*sha512_224(char *tocrypt, size_t len);
char		*sha512_256(char *tocrypt, size_t len);
#endif
