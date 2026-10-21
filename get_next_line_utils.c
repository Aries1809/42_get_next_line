/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kseltenr <kseltenr@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 21:54:35 by kseltenr          #+#    #+#             */
/*   Updated: 2026/10/21 05:42:47 by kseltenr        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stdlib.h>

int	ft_strlen(char *s)
{
	int	len;

	len = 0;
	while (s[len])
		len++;
	return (len);
}

char	*ft_allocjoin(buffer *buf, char *str, int total)
{
	int		i;
	int		n;
	char	*ret;

	ret = malloc(sizeof(char) * (total + buf->b_cap + 1));
	if (!ret)
	{
		free(buf->b_buf);
		return (NULL);
	}
	n = -1;
	i = -1;
	while (++i < buf->b_len)
		ret[i] = buf->b_buf[i];
	while (str[++n])
		ret[i++] = str[n];
	ret[i] = '\0';
	buf->b_len = total;
	buf->b_cap += total;
	free(buf->b_buf);
	return (ret);

}

char	*ft_strjoin(buffer *buf, char *str, int len)
{
	int		i;
	int		total;

	i = -1;
	total = buf->b_len + len;
	if (total < buf->b_cap)
	{
		while (str[++i])
			buf->b_buf[buf->b_len++] = str[i];
		buf->b_buf[buf->b_len] = '\0';
		return (buf->b_buf);
	}
	return (ft_allocjoin(&*buf, str, total));
}

char	*delete_line(buffer *buf, int len)
{
	int		i;
	char	*ret;

	if (!buf->b_buf)
		return (NULL);
	if (len >= buf->b_len)
	{
		free(buf->b_buf);
		return (NULL);
	}
	buf->b_len -= len;
	buf->b_cap -= len;
	ret = malloc(sizeof(char) * (buf->b_len + buf->b_cap + 1));
	if (!ret)
	{
		free(buf->b_buf);
		buf->b_len = 0;
		return (NULL);
	}
	i = 0;
	while (buf->b_buf[len])
		ret[i++] = buf->b_buf[len++];
	ret[i] = '\0';
	free(buf->b_buf);
	return (ret);
}

char	*return_line(buffer buf)
{
	int		i;
	int		n;
	char	*ret;

	i = 0;
	n = 0;
	while (buf.b_buf[i] != '\n' && buf.b_buf[i])
		i++;
	if (buf.b_buf[i] == '\n')
		i++;
	ret = malloc(sizeof(char) * (i + 1));
	if (!ret)
		return (NULL);
	while (n != i)
	{
		ret[n] = buf.b_buf[n];
		n++;
	}
	ret[n] = '\0';
	return (ret);
}
