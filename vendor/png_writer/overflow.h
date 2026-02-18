/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   overflow.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:35:47 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:35:47 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OVERFLOW_H
#define OVERFLOW_H

/* Safely check if multiplying two integers will overflow (no undefined
behavior, compiler removing the code, etc...) and output result. */
static int lodepng_mulofl(size_t a, size_t b, size_t *result)
{
	*result = a * b;
	return (a != 0 && *result / a != b);
}

/* Safely check if adding two integers will overflow (no undefined
behavior, compiler removing the code, etc...) and output result. */
static int lodepng_addofl(size_t a, size_t b, size_t *result)
{
	*result = a + b;
	return *result < a;
}

/*Safely checks whether size_t overflow can be caused due to amount of pixels.
This check is overcautious rather than precise. If this check indicates no overflow,
you can safely compute in a size_t (but not an unsigned):
-(size_t)w * (size_t)h * 8
-amount of bytes in IDAT (including filter, padding and Adam7 bytes)
-amount of bytes in raw color model
Returns 1 if overflow possible, 0 if not.
*/
static int lodepng_pixel_overflow(unsigned w, unsigned h,
								  const LodePNGColorMode *pngcolor, const LodePNGColorMode *rawcolor)
{
	size_t bpp = LODEPNG_MAX(lodepng_get_bpp(pngcolor), lodepng_get_bpp(rawcolor));
	size_t numpixels, total;
	size_t line;

	if (lodepng_mulofl((size_t)w, (size_t)h, &numpixels))
		return 1;
	if (lodepng_mulofl(numpixels, 8, &total))
		return 1;

	if (lodepng_mulofl((size_t)(w / 8), bpp, &line))
		return 1;
	if (lodepng_addofl(line, ((w & 7) * bpp + 7) / 8, &line))
		return 1;

	if (lodepng_addofl(line, 5, &line))
		return 1;
	if (lodepng_mulofl(line, h, &total))
		return 1;

	return 0;
}

#endif