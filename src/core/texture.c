#include "texture.h"
#include "light.h"
#include "minirt.h"
#include <math.h>

t_color	apply_checkerboard(t_vec3 hit_point, t_color base_color)
{
	double	scale;
	int		spatial_wave;
	t_color	secondary_color;

	scale = 5.0; 
	secondary_color = vec3_create(0.05, 0.05, 0.05);
	spatial_wave = (int)(floor(hit_point.x * scale) + 
							floor(hit_point.y * scale) + 
							floor(hit_point.z * scale));
	
	if (spatial_wave % 2 == 0)
		return (base_color);
	return (secondary_color);
}