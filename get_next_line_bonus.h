/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kseltenr <kseltenr@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 14:41:42 by kseltenr          #+#    #+#             */
/*   Updated: 2026/10/21 05:08:41 by kseltenr        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

# include <stddef.h>

typedef struct s_gnl
{
	char			*buf;
	size_t			start;
	size_t			end;
	size_t			scan;
	size_t			cap;
	int				fd;
	struct s_gnl	*next;
}	t_gnl;

char	*get_next_line(int fd);
char	*gnl_extract(t_gnl *node, size_t stop);
size_t	gnl_find_nl(t_gnl *node);
int		gnl_reserve(t_gnl *node);

#endif
