#ifndef RTW_LODE_IMAGE_H
#define RTW_LODE_IMAGE_H

#ifndef LODEPNG_COMPILE_DISK
#define LODEPNG_COMPILE_DISK
#endif

#include <stdlib.h>

#include "../all.h"
#include "../error.h"

/* Owned RGBA8 image */
typedef struct s_lode_image
{
	unsigned char *rgba; /* owned */
	unsigned w;
	unsigned h;
	unsigned stride; /* bytes per row = w*4 */
} t_lode_image;

static inline void lode_image_init(t_lode_image *img)
{
	if (!img)
		return;
	img->rgba = NULL;
	img->w = 0;
	img->h = 0;
	img->stride = 0;
}

static inline void lode_image_cleanup(t_lode_image *img)
{
	if (!img)
		return;
	if (img->rgba)
		lodepng_free(img->rgba);
	lode_image_init(img);
}

static inline unsigned lode_image_load_png(t_lode_image *img, const char *filename)
{
	unsigned char *data = NULL;
	unsigned w = 0, h = 0;
	unsigned err;

	if (!img || !filename)
		return 48;

	err = lodepng_decode32_file(&data, &w, &h, filename);
	if (err)
		return err;

	lode_image_cleanup(img);
	img->rgba = data;
	img->w = w;
	img->h = h;
	img->stride = w * 4u;
	return 0;
}

/* returns pointer to RGB bytes (first 3 bytes of RGBA) */
static inline const unsigned char *lode_image_pixel_rgb(const t_lode_image *img, int x, int y)
{
	static unsigned char magenta[3] = {255, 0, 255};

	if (!img || !img->rgba || img->w == 0 || img->h == 0)
		return magenta;

	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (x >= (int)img->w)
		x = (int)img->w - 1;
	if (y >= (int)img->h)
		y = (int)img->h - 1;

	return img->rgba + (size_t)y * (size_t)img->stride + (size_t)x * 4u;
}

#endif /* RTW_LODE_IMAGE_H */
