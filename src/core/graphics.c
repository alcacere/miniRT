#include "graphics.h"
#include "mlx.h"
#include <stdlib.h>

int	close_window(t_minirt *rt)
{
	free_scene(&rt->scene);
	mlx_destroy_window(rt->mlx, rt->win);
	exit(0);
	return (0);
}

int	key_hook(int keycode, t_minirt *rt)
{
	if (keycode == KEY_ESC)
		close_window(rt);
	return (0);
}

void	init_graphics(t_minirt *rt, t_img *img)
{
	int	w;
	int	h;

	w = rt->scene.camera.image_width;
	h = rt->scene.camera.image_height;
	rt->mlx = mlx_init();
	if (!rt->mlx)
	{
		ft_putendl_fd("Error\n).", 2);
		free_scene(&rt->scene);
		exit(1);
	}
	rt->win = mlx_new_window(rt->mlx, w, h, "miniRT - Pro Engine");
	if (!rt->win)
	{
		free_scene(&rt->scene);
		exit(1);
	}
	img->img = mlx_new_image(rt->mlx, w, h);
	img->addr = mlx_get_data_addr(img->img, &img->bpp, \
									&img->line_len, &img->endian);
}