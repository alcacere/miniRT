
#include "structures.h"

/*
** Inicializa un vector con valores x, y, z.
** Es el constructor básico.
*/
t_vec3	vec3_init(double x, double y, double z)
{
	t_vec3	vec;

	vec.x = x;
	vec.y = y;
	vec.z = z;
	return (vec);
}

/*
** Suma dos vectores componente a componente.
** Retorna un nuevo vector resultante.
*/
t_vec3	vec3_add(t_vec3 v1, t_vec3 v2)
{
	t_vec3	result;

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return (result);
}

/*
** Resta v2 de v1 (v1 - v2).
** Importante para calcular vectores de dirección (Destino - Origen).
*/
t_vec3	vec3_sub(t_vec3 v1, t_vec3 v2)
{
	t_vec3	result;

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return (result);
}

/*
** Multiplica el vector 'v' por un número escalar 'scale'.
** Usado para cambiar la longitud (magnitud) del vector.
*/
t_vec3	vec3_scale(t_vec3 v, double scale)
{
	t_vec3	result;

	result.x = v.x * scale;
	result.y = v.y * scale;
	result.z = v.z * scale;
	return (result);
}

/*
** Multiplicación componente a componente (Producto de Hadamard).
** v1.x * v2.x, etc.
** ESENCIAL para mezclar colores (Luz * Albedo del material).
*/
t_vec3	vec3_mul(t_vec3 v1, t_vec3 v2)
{
	t_vec3	result;

	result.x = v1.x * v2.x;
	result.y = v1.y * v2.y;
	result.z = v1.z * v2.z;
	return (result);
}
