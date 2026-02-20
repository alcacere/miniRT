#ifndef CAMERA_H
# define CAMERA_H

# include "minirt.h"
# include "hittable.h"
# include <pthread.h>

# define THREAD_COUNT 8

#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288
#endif

typedef struct s_render_ctx
{
	t_camera	*cam;
	t_hittable	*world;
	t_scene		*scene;
	int			*image_buffer;
	int			start_y;
	int			end_y;
}	t_render_ctx;

void	camera_render_threaded(t_camera *cam, t_hittable *world,
	 int *buffer, t_scene *s);
t_ray	get_ray_stratified(t_camera *c, int coords[2], int s_coords[2]);
t_vec3	ray_color(t_ray *r, t_scene *scene, t_hittable *world);
void	init_camera(t_camera *c);

#endif