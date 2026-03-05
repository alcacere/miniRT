#ifndef TEXTURE_H
# define TEXTURE_H

# include "structures.h"

t_color	apply_checkerboard(t_vec3 hit_point, t_color base_color);
void	apply_bumpmap(t_vec3 p, t_vec3 *normal);

#endif