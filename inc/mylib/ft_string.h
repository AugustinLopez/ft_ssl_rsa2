/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_string.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/27 21:00:36 by aulopez           #+#    #+#             */
/*   Updated: 2021/05/31 17:49:31 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_STRING_H
# define FT_STRING_H

# include <stddef.h>

typedef struct s_string t_string;

int salloc(struct s_string **str, const char *src, const size_t len);
void sfree(struct s_string *str);
void sdel(struct s_string **str);
int salead(struct s_string *str, const char *append, const size_t add);
int satrail(struct s_string *str, const char *prepend, const size_t add);
int srlead(struct s_string *str, size_t remove);
int srtrail(struct s_string *str, size_t remove);
void sclear(struct s_string *str);
int sreset(struct s_string **str);
void strim(struct s_string *str);
size_t slen(struct s_string *str);
char *sptr(struct s_string *str);
int sfromfd(t_string **str, const int fd);

#endif
