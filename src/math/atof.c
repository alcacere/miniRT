#include "minirt.h"
#include "libft.h"

static double	get_fractional(const char *str, int *i)
{
	double	frac;
	double	divisor;

	frac = 0.0;
	divisor = 10.0;
	if (str[*i] == '.')
	{
		(*i)++;
		while (ft_isdigit(str[*i]))
		{
			frac += (str[*i] - '0') / divisor;
			divisor *= 10.0;
			(*i)++;
		}
	}
	return (frac);
}

double	ft_atof(const char *str)
{
	int		i;
	double	sign;
	double	result;

	i = 0;
	sign = 1.0;
	result = 0.0;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == ' ')
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1.0;
		i++;
	}
	while (ft_isdigit(str[i]))
	{
		result = result * 10.0 + (str[i] - '0');
		i++;
	}
	result += get_fractional(str, &i);
	return (result * sign);
}