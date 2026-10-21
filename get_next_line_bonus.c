/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kseltenr <kseltenr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 03:06:54 by kseltenr          #+#    #+#             */
/*   Updated: 2026/10/21 05:08:00 by kseltenr        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"
#include <stdlib.h>
#include <unistd.h>

static t_gnl	*gnl_get_node(t_gnl **head, int fd)
{
	t_gnl	*node;

	node = *head;
	while (node && node->fd != fd)
		node = node->next;
	if (node)
		return (node);
	node = malloc(sizeof(t_gnl));
	if (!node)
		return (NULL);
	node->buf = NULL;
	node->start = 0;
	node->end = 0;
	node->scan = 0;
	node->cap = 0;
	node->fd = fd;
	node->next = *head;
	*head = node;
	return (node);
}

static char	*gnl_free_node(t_gnl **head, t_gnl *node, char *ret)
{
	t_gnl	**link;

	link = head;
	while (*link != node)
		link = &(*link)->next;
	*link = node->next;
	free(node->buf);
	free(node);
	return (ret);
}

static ssize_t	gnl_fill(t_gnl *node, size_t *stop)
{
	ssize_t	bytes;

	*stop = gnl_find_nl(node);
	while (!*stop)
	{
		if (!gnl_reserve(node))
			return (-1);
		bytes = read(node->fd, node->buf + node->end, BUFFER_SIZE);
		if (bytes <= 0)
			return (bytes);
		node->end += bytes;
		*stop = gnl_find_nl(node);
	}
	return (1);
}

char	*get_next_line(int fd)
{
	static t_gnl	*head;
	t_gnl			*node;
	size_t			stop;
	ssize_t			status;
	char			*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	node = gnl_get_node(&head, fd);
	if (!node)
		return (NULL);
	status = gnl_fill(node, &stop);
	if (status < 0)
		return (gnl_free_node(&head, node, NULL));
	if (status == 0)
	{
		if (node->start == node->end)
			return (gnl_free_node(&head, node, NULL));
		stop = node->end;
	}
	line = gnl_extract(node, stop);
	if (!line || status == 0)
		return (gnl_free_node(&head, node, line));
	return (line);
}
