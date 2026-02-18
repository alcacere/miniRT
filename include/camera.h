#ifndef CAMERA_H
# define CAMERA_H

# include "minirt.h"
# include "hittable.h"
# include <pthread.h>

# define THREAD_COUNT 8

typedef struct s_camera
{
	double		aspect_ratio;
	double		samples_per_pixel;
	double		pixel_samples_scale;
	int			image_width;
	int			image_height;
	int			max_depth;
	double		vfov;
	t_point3	lookfrom;
	t_point3	lookat;
	t_vec3		vup;
	double		defocus_angle;
	double		focus_dist;
	t_color		background;
	t_point3	center;
	t_point3	pixel00_loc;
	t_vec3		pixel_delta_u;
	t_vec3		pixel_delta_v;
	t_vec3		u;
	t_vec3		v;
	t_vec3		w;
	t_vec3		defocus_disk_u;
	t_vec3		defocus_disk_v;
	int			sqrt_spp;
	double		recip_sqrt_spp;
}	t_camera;

typedef struct s_render_ctx
{
	t_camera	*cam;
	t_hittable	*world;
	int			*image_buffer;// (0xRRGGBB)
	int			start_y;
	int			end_y;
}	t_render_ctx;

void	camera_render_threaded(t_camera *cam, t_hittable *world, int *buffer);

#endif