#include "parse.h"
#include "libft.h"

int	parse_color(char *str, t_color *color)
{
	int	r;
	int	g;
	int	b;

	if (!str)
		return (0);
	r = ft_atoi(str);
	while (*str && *str != ',')
		str++;
	if (!*str)
		return (0);
	str++;
	g = ft_atoi(str);
	while (*str && *str != ',')
		str++;
	if (!*str)
		return (0);
	str++;
	b = ft_atoi(str);
	color->x = r / 255.0;
	color->y = g / 255.0;
	color->z = b / 255.0;
	return (is_valid_color(*color));
}

int	parse_vec3(char *str, t_vec3 *vec)
{
	if (!str)
		return (0);
	vec->x = ft_atof(str);
	while (*str && *str != ',')
		str++;
	if (!*str)
		return (0);
	str++;
	vec->y = ft_atof(str);
	while (*str && *str != ',')
		str++;
	if (!*str)
		return (0);
	str++;
	vec->z = ft_atof(str);
	return (1);
}

int	is_valid_color(t_color c)
{
	if (c.x < 0.0 || c.x > 1.0)
		return (0);
	if (c.y < 0.0 || c.y > 1.0)
		return (0);
	if (c.z < 0.0 || c.z > 1.0)
		return (0);
	return (1);
}

int	is_normalized(t_vec3 v)
{
	if (v.x < -1.0 || v.x > 1.0)
		return (0);
	if (v.y < -1.0 || v.y > 1.0)
		return (0);
	if (v.z < -1.0 || v.z > 1.0)
		return (0);
	return (1);
}