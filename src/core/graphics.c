/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   graphics.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:19:54 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/10 22:48:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "graphics.h"
#include "mlx.h"
#include <stdlib.h>

extern void	free_bvh(void *node);

int	close_window(t_minirt *rt)
{
	if (rt->img_ptr)
		mlx_destroy_image(rt->mlx, rt->img_ptr);
	free_bvh(rt->world);
	rt->world = NULL;
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

int	expose_hook(t_minirt *rt)
{
	if (rt->img_ptr)
		mlx_put_image_to_window(rt->mlx, rt->win, rt->img_ptr, 0, 0);
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
		ft_putendl_fd("Error\nmlx_init failed.", 2);
		free_scene(&rt->scene);
		exit(1);
	}
	rt->win = mlx_new_window(rt->mlx, w, h, "miniRT");
	if (!rt->win)
	{
		free_scene(&rt->scene);
		exit(1);
	}
	img->img = mlx_new_image(rt->mlx, w, h);
	img->addr = mlx_get_data_addr(img->img, &img->bpp,
			&img->line_len, &img->endian);
	rt->img_ptr = img->img;
}
