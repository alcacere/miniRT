/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_png.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 18:00:00 by marvin            #+#    #+#             */
/*   Updated: 2026/01/03 18:13:55 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define LODEPNG_COMPILE_DISK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../all.h"
#include "../error.h"

int main(int argc, char **argv)
{
	(void)lodepng_error_text; /* touch symbol to ensure link */

	if (argc < 2)
	{
		printf("Usage: %s <png_file>\n", argv[0]);
		printf("Simple PNG decoder test\n");
		return 0;
	}

	unsigned char *image = NULL;
	unsigned w = 0, h = 0;
	unsigned error = lodepng_decode32_file(&image, &w, &h, argv[1]);

	if (error)
	{
		fprintf(stderr, "Error %u: %s\n", error, lodepng_error_text(error));
		return 1;
	}

	printf("Successfully decoded PNG: %s\n", argv[1]);
	printf("  Width: %u pixels\n", w);
	printf("  Height: %u pixels\n", h);
	printf("  Total pixels: %u\n", w * h);
	printf("  Image data size: %u bytes\n", w * h * 4);

	lodepng_free(image);
	return 0;
}
