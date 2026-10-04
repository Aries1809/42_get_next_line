/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kseltenr <kseltenr@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 21:54:35 by kseltenr          #+#    #+#             */
/*   Updated: 2026/09/11 02:58:09 by kseltenr        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stdlib.h>
#include <unistd.h>

int	find_line(buffer buf)
{
	int	i;

	if (!buf.b_buf)
		return (1);
	i = 0;
	while (buf.b_buf[i])
	{
		if (buf.b_buf[i] == '\n')
			return (0);
		i++;
	}
	return (1);
}

void	read_to_buf(int fd, buffer *buf)
{
	char	*temp;
	int		i;

	i = 0;
	temp = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!temp)
		return ;
	while (find_line(*buf))
	{
		i = read(fd, temp, BUFFER_SIZE);
		if (i == -1)
		{
			free(temp);
			free(buf->b_buf);
			buf->b_buf = NULL;
			return ;
		}
		else if (i == 0)
			break ;
		temp[i] = '\0';
		buf->b_buf = ft_strjoin(&*buf, temp, i);
	}
	free(temp);
	return ;
}

char	*get_next_line(int fd)
{
	static buffer		buf;
	char				*ret;

	if (!buf.b_buf)
	{
		buf.b_cap = BUFFER_SIZE;
		buf.b_len = 0;
		buf.b_buf = malloc(buf.b_cap);
		if (!buf.b_buf)
			return (NULL);
		buf.b_buf[0] = '\0';
	}
	read_to_buf(fd, &buf);
	if (!buf.b_buf)
		return (NULL);
	if (buf.b_buf[0] == '\0')
	{
		free(buf.b_buf);
		buf.b_buf = NULL;
		return (NULL);
	}
	ret = return_line(buf);
	buf.b_buf = delete_line(&buf, ft_strlen(ret));
	return (ret);
}
