#include "minirt.h"

void	free_split(char **split)
{
	int	i;

	if (!split)
		return ;
	i = 0;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
}

static int	is_valid_number(char *str)
{
	int	is_dot;

	is_dot = 0;
	if (*str == '\0')
		return (0);
	if (ft_issign(*str))
		str++;
	while (*str)
	{
		if (*str == '.' && *(str + 1))
		{
			is_dot++;
			str++;
		}
		if (!ft_isdigit(*str) || is_dot > 1)
			return (0);
		str++;
	}
	return (1);
}

static double	get_value(char *str)
{
	double	res;
	double	pow;

	res = 0.0;
	pow = 1.0;
	while (ft_isdigit(*str))
	{
		res = res * 10.0 + (*str - '0');
		str++;
	}
	if (*str == '.')
		str++;
	while (ft_isdigit(*str))
	{
		pow /= 10;
		res = res * 10.0 + (*str - '0');
		str++;
	}
	return (res * pow);
}

double	ft_atof(char *str)
{
	double	res;
	double	sign;

	res = 0.0;
	sign = 1.0;
	if (!is_valid_number(str))
		//error handler;
	if (ft_issign(*str))
	{
		if (*str == '-')
			sign = -1.0;
		str++;
	}
	res = get_value(str);
	return (res * sign);
}

t_vec3	parse_vec3(char *str)
{
	t_vec3	vec;
	char	**parts;

	vec = vec3_init(0, 0, 0);
	parts = ft_split(str, ',');
	if (!parts)
		return (vec);
	if (parts[0] && parts[1] && parts[2])
	{
		vec.x = ft_atof(parts[0]);
		vec.y = ft_atof(parts[1]);
		vec.z = ft_atof(parts[2]);
	}
	free_split(parts);
	return (vec);
}

t_color	parse_color(char *str)
{
	return (parse_vec3(str));
}
