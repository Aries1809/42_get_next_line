/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kseltenr <kseltenr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 03:05:52 by kseltenr          #+#    #+#             */
/*   Updated: 2026/10/21 05:13:30 by kseltenr        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"
#include <stdlib.h>

static int	gnl_compact(t_gnl *node)
{
	size_t	i;

	if (node->end + BUFFER_SIZE <= node->cap)
		return (1);
	if (node->start == 0)
		return (0);
	i = 0;
	while (node->start + i < node->end)
	{
		node->buf[i] = node->buf[node->start + i];
		i++;
	}
	node->end -= node->start;
	node->scan -= node->start;
	node->start = 0;
	return (node->end + BUFFER_SIZE <= node->cap);
}

int	gnl_reserve(t_gnl *node)
{
	char	*new;
	size_t	cap;
	size_t	i;

	if (gnl_compact(node))
		return (1);
	cap = node->cap * 2;
	if (cap < node->end + BUFFER_SIZE)
		cap = node->end + BUFFER_SIZE;
	new = malloc(cap);
	if (!new)
		return (0);
	i = 0;
	while (i < node->end)
	{
		new[i] = node->buf[i];
		i++;
	}
	free(node->buf);
	node->buf = new;
	node->cap = cap;
	return (1);
}

size_t	gnl_find_nl(t_gnl *node)
{
	if (node->scan < node->start)
		node->scan = node->start;
	while (node->scan < node->end)
	{
		if (node->buf[node->scan++] == '\n')
			return (node->scan);
	}
	return (0);
}

char	*gnl_extract(t_gnl *node, size_t stop)
{
	char	*line;
	size_t	i;

	line = malloc(stop - node->start + 1);
	if (!line)
		return (NULL);
	i = 0;
	while (node->start < stop)
		line[i++] = node->buf[node->start++];
	line[i] = '\0';
	if (node->start == node->end)
	{
		node->start = 0;
		node->end = 0;
		node->scan = 0;
	}
	return (line);
}
