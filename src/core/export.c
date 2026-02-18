#include "export.h"
#include <stdlib.h>
#include "libft.h"
#include "png_codec.h" 

static void	fill_rgba_buffer(int *mlx_buffer, unsigned char *rgba, \
								int width, int height)
{
	int	x;
	int	y;
	int	pixel;
	int	idx;

	y = 0;
	while (y < height)
	{
		x = 0;
		while (x < width)
		{
			pixel = mlx_buffer[y * width + x];
			idx = (y * width + x) * 4;
			rgba[idx + 0] = (pixel >> 16) & 0xFF;
			rgba[idx + 1] = (pixel >> 8) & 0xFF;
			rgba[idx + 2] = pixel & 0xFF;
			rgba[idx + 3] = 255;
			x++;
		}
		y++;
	}
}

int	export_frame_to_png(t_img *img, int width, int height, char *filename)
{
	unsigned char	*rgba_buffer;
	int				status;

	rgba_buffer = malloc(width * height * 4);
		if (!rgba_buffer)
		return (0);
	fill_rgba_buffer((int *)img->addr, rgba_buffer, width, height);
	status = encode_png(filename, rgba_buffer, width, height);
	free(rgba_buffer);
	return (status == 0);
}