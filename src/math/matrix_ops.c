#include "minirt.h"

// in this file, i declare functions to handle traslation of objects;
// i use 4x4 matix to represent a point in 3d space and an extra dimension
// to represent traslation;


// Init 4x4 matrix identity (diagonal = 1.0, rest = 0), this is usefull to multiplication operation over
// matrix elements because identity represent a neutral value to each element in the vectors;
// it is equivalent to number 1 in multiplication;

void	matrix4_set_identity(t_matrix4 *mat)
{
	int	i;
	int	j;

	i = 0;
	while (i < 4)
	{
		j = 0;
		while (j < 4)
		{
			if (i == j)
				mat->elements[i][j] = 1.0;
			else
				mat->elements[i][j] = 0;
			j++;
		}
		i++;
	}
}

// multiplication of 4x4 matrix operation, retult = A * B; the order of elements cares and the transformation
// accumulates;

t_matrix4	matrix4_mul(const t_matrix4 *a, const t_matrix4 *b)
{
	t_matrix4	res;
	int			i;
	int			j;
	int			k;

	i = 0;
	while (i < 4)
	{
		j = 0;
		while (j < 4)
		{
			res.elements[i][j] = 0.0;
			k = 0;
			while (k < 4)
			{
				res.elements[i][j] += a->elements[i][k] * b->elements[k][j];	
				k++;
			}
			j++;
		}
		i++;
	}
	return (res);
}

// multiplication of a matrix by vector DIRECTION = (w = 0);
// ignores the traslation (4th column of the matrix);
// just rotate the scale.

t_vec3	matrix4_mul_vec3(const t_matrix4 *mat, const t_vec3 *v)
{
	t_vec3	res;

	res.x = mat->elements[0][0] * v->x + mat->elements[0][1] * v->y
		+ mat->elements[0][2] * v->z;
	res.y = mat->elements[1][0] * v->x + mat->elements[1][1] * v->y
		+ mat->elements[0][2] * v->z;
	res.z = mat->elements[2][0] * v->x + mat->elements[2][1] * v->y
		+ mat->elements[2][2] * v->z;
	return (res);
}

// multiplication of matrix by point (w = 1);
// aplies rotation, traslation and scale;

t_point3	matrix4_mul_point3(const t_matrix4 *mat, const t_point3 *p)
{
	t_point3	res;
	double		w;

	res.x = mat->elements[0][0] * p->x + mat->elements[0][1] * p->y
		+ mat->elements[0][2] * p->z + mat->elements[0][3];
	res.y = mat->elements[1][0] * p->x + mat->elements[1][1] * p->y
		+ mat->elements[1][2] * p->z + mat->elements[1][3];
	res.z = mat->elements[2][0] * p->x + mat->elements[2][1] * p->y
		+ mat->elements[2][2] * p->z + mat->elements[2][3];
	w = mat->elements[3][0] * p->x + mat->elements[3][1] * p->y
		+ mat->elements[3][2] * p->z + mat->elements[3][3];
	if (w != 1.0 && w != 0.0)
	{
		res.x /= w;
		res.y /= w;
		res.z /= w;
	}
	return (res);
}
