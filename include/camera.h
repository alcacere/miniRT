/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 16:12:52 by alcacere          #+#    #+#             */
/*   Updated: 2026/03/08 16:12:54 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAMERA_H
# define CAMERA_H

# include "minirt.h"
# include "hittable.h"
# include <pthread.h>

# define THREAD_COUNT 32

# ifndef M_PI
#  define M_PI    3.14159265358979323846264338327950288
# endif

typedef struct s_render_ctx	t_render_ctx;

typedef struct s_render_ctx
{
	t_camera	*cam;
	t_hittable	*world;
	t_scene		*scene;
	int			*image_buffer;
	int			start_y;
	int			end_y;
}	t_render_ctx;

t_vec3	ray_color(t_ray *r, t_render_ctx *ctx, int depth, uint32_t *seed);
void	camera_render_threaded(t_camera *cam, t_hittable *world,
			int *buffer, t_scene *s);
t_ray	get_ray_stratified(t_camera *c, int coords[2], int s_coords[2]);
void	init_camera(t_camera *c);
t_ray	get_ray_stratified(t_camera *c, int coords[2], int s_coords[2]);
int		scatter(t_ray rays[2], t_hit_record *rec, t_color *att, uint32_t *seed);

#endif
