/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kseltenr <kseltenr@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 14:41:55 by kseltenr          #+#    #+#             */
/*   Updated: 2026/09/28 14:41:58 by kseltenr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 42
#endif

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

typedef struct	buffer
{
	char	*b_buf;
	int		b_len;
	int		b_cap;
}			buffer;

int		ft_strlen(char *s);
char	*ft_strjoin(buffer *buf, char *str, int len);
char	*delete_line(buffer *buf, int len);
char	*return_line(buffer buf);
int		find_line(buffer buf);
char	*get_next_line(int fd);

#endif
