#include "parse.h"
#include "libft.h"

int	is_valid_double(char *str)
{
	int	dot_count;
	int	digit_count;

	dot_count = 0;
	digit_count = 0;
	if (!str || !str[0])
		return (0);
	if (*str == '-' || *str == '+')
		str++;
	while (*str)
	{
		if (*str == '.')
			dot_count++;
		else if (ft_isdigit(*str))
			digit_count++;
		else
			return (0);
		str++;
	}
	if (dot_count > 1 || digit_count == 0)
		return (0);
	return (1);
}

int	parse_color(char *str, t_color *color)
{
	char	**rgb;

	rgb = ft_split(str, ',');
	if (!rgb || !rgb[0] || !rgb[1] || !rgb[2] || rgb[3])
		return (ft_free_arr(rgb), 0);
	if (!is_valid_double(rgb[0]) || !is_valid_double(rgb[1]) || 
	!is_valid_double(rgb[2]))
		return (ft_free_arr(rgb), 0);
	color->x = ft_atoi(rgb[0]) / 255.0;
	color->y = ft_atoi(rgb[1]) / 255.0;
	color->z = ft_atoi(rgb[2]) / 255.0;
	ft_free_arr(rgb);
	return (is_valid_color(*color));
}

int	parse_vec3(char *str, t_vec3 *vec)
{
	char	**xyz;

	xyz = ft_split(str, ',');
	if (!xyz || !xyz[0] || !xyz[1] || !xyz[2] || xyz[3])
		return (ft_free_arr(xyz), 0);
	if (!is_valid_double(xyz[0]) || !is_valid_double(xyz[1]) || 
	!is_valid_double(xyz[2]))
		return (ft_free_arr(xyz), 0);
	vec->x = ft_atof(xyz[0]);
	vec->y = ft_atof(xyz[1]);
	vec->z = ft_atof(xyz[2]);
	ft_free_arr(xyz);
	return (1);
}