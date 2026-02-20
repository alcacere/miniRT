#include "minirt.h"
#include "parse.h"
#include "graphics.h"
#include "camera.h"
#include "libft.h"
#include "mlx.h"

extern t_hittable	*build_world(t_object *objects);

static void	setup_and_render(t_minirt *rt, t_img *img)
{
	t_hittable	*world;

	world = build_world(rt->scene.objects);
	if (!world)
	{
		free_scene(&rt->scene);
		exit(1);
	}
	init_graphics(rt, img);
	camera_render_threaded(&rt->scene.camera, world, (int *)img->addr, &rt->scene);
	mlx_put_image_to_window(rt->mlx, rt->win, img->img, 0, 0);
}

int	main(int argc, char **argv)
{
	t_minirt	rt;
	t_img		img;

	if (argc != 2)
	{
		ft_putendl_fd("Error\nUso: ./miniRT <escena.rt>", 2);
		return (1);
	}
	ft_bzero(&rt, sizeof(t_minirt));
	if (!parse_file(argv[1], &rt.scene))
	{
		ft_putendl_fd("Error\nArchivo de escena inválido o malformado", 2);
		return (1);
	}
	init_camera(&rt.scene.camera);
	setup_and_render(&rt, &img);
	mlx_hook(rt.win, 2, 1L << 0, key_hook, &rt);
	mlx_hook(rt.win, 17, 0, close_window, &rt);
	mlx_loop(rt.mlx);
	return (0);
}