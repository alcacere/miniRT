/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atof.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alcacere <alcacere@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 09:33:13 by alcacere          #+#    #+#             */
/*   Updated: 2026/02/16 10:48:35 by alcacere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"
#include "libft.h"
#include <stdio.h>

int ft_issign(char c)
{
	return (c == '+' || c == '-');
}

static int	is_valid_number(char *str)
{
	int	is_dot;
	int	is_digit_flag;

	is_dot = 0;
	is_digit_flag = 0;
	if (*str == '\0')
		return (0);
	if (ft_issign(*str))
		str++;
	while (*str)
	{
		if (ft_isdigit(*str))
			is_digit_flag = 1;
		if (*str == '.')
		{
			if (!*(str + 1))
				return (0);
			is_dot++;
			str++;
		}
		if (!is_digit_flag || is_dot > 1 || !ft_isdigit(*str))
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
		return (printf("lolllllllllll, que mal jeje"), 0);
	if (ft_issign(*str))
	{
		if (*str == '-')
			sign = -1.0;
		str++;
	}
	res = get_value(str);
	return (res * sign);
}

int main(int ac, char **av)
{
	if (ac == 1)
		return 0;
	char *str = av[1];
	printf("%.2f\n", ft_atof(str));
}
