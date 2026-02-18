/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   img_file.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 22:54:23 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 22:54:23 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IMG_FILE_H
#define IMG_FILE_H

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

static long lodepng_filesize(const char *filename)
{
	FILE *file;
	long size;
	file = fopen(filename, "rb");
	if (!file)
		return -1;

	if (fseek(file, 0, SEEK_END) != 0)
	{
		fclose(file);
		return -1;
	}

	size = ftell(file);

	if (size == LONG_MAX)
		size = -1;

	fclose(file);
	return size;
}

static inline unsigned lodepng_buffer_file(unsigned char *out, size_t size, const char *filename)
{
	FILE *file;
	size_t readsize;
	file = fopen(filename, "rb");
	if (!file)
		return 78;

	readsize = fread(out, 1, size, file);
	fclose(file);

	if (readsize != size)
		return 78;
	return 0;
}

static inline unsigned lodepng_load_file(unsigned char **out, size_t *outsize, const char *filename)
{
	long size = lodepng_filesize(filename);
	if (size < 0)
		return 78;
	*outsize = (size_t)size;

	*out = (unsigned char *)lodepng_malloc((size_t)size);
	if (!(*out) && size > 0)
		return 83;

	return lodepng_buffer_file(*out, (size_t)size, filename);
}

static inline unsigned lodepng_save_file(const unsigned char *buffer, size_t buffersize, const char *filename)
{
	FILE *file;
	file = fopen(filename, "wb");
	if (!file)
		return 79;
	fwrite(buffer, 1, buffersize, file);
	fclose(file);
	return 0;
}

#endif