/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:20:24 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/11 02:00:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "parse.h"
#include "graphics.h"
#include "camera.h"
#include "libft.h"
#include "mlx.h"

extern t_hittable	*build_world(t_object *objects);

static int	check_extension(const char *filename)
{
	size_t	len;

	len = ft_strlen(filename);
	if (len < 4)
		return (0);
	if (ft_strncmp(filename + len - 3, ".rt", 4) != 0)
		return (0);
	return (1);
}

static void	setup_and_render(t_minirt *rt, t_img *img)
{
	t_hittable	*world;

	world = build_world(rt->scene.objects);
	if (!world)
	{
		free_scene(&rt->scene);
		exit(1);
	}
	rt->world = world;
	init_graphics(rt, img);
	camera_render_threaded(&rt->scene.camera, world,
		(int *)img->addr, &rt->scene);
	mlx_put_image_to_window(rt->mlx, rt->win, img->img, 0, 0);
}

static int	validate_and_parse(int argc, char **argv, t_minirt *rt)
{
	if (argc != 2)
		return (ft_putendl_fd("Error\nUsage: ./miniRT <scene.rt>", 2), 0);
	if (!check_extension(argv[1]))
		return (ft_putendl_fd("Error\nFile must have .rt extension", 2), 0);
	ft_bzero(rt, sizeof(t_minirt));
	if (!parse_file(argv[1], &rt->scene))
		return (ft_putendl_fd("Error\nInvalid .rt file", 2), 0);
	if (!rt->scene.has_camera || !rt->scene.has_ambient)
	{
		ft_putendl_fd("Error\nMissing mandatory A or C element", 2);
		free_scene(&rt->scene);
		return (0);
	}
	return (1);
}

int	main(int argc, char **argv)
{
	t_minirt	rt;
	t_img		img;

	if (!validate_and_parse(argc, argv, &rt))
		return (1);
	init_camera(&rt.scene.camera);
	setup_and_render(&rt, &img);
	mlx_hook(rt.win, 2, 1L << 0, key_hook, &rt);
	mlx_hook(rt.win, 17, 0, close_window, &rt);
	mlx_hook(rt.win, 12, 1L << 15, expose_hook, &rt);
	mlx_loop(rt.mlx);
	return (0);
}
