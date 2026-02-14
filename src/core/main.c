#include "minirt.h"
#include <stdio.h> // Solo permitido temporalmente para pruebas

/* Función auxiliar para imprimir vectores */
void	print_vec(char *name, t_vec3 v)
{
	printf("%s: (%.2f, %.2f, %.2f)\n", name, v.x, v.y, v.z);
}

int	main(void)
{
	printf("=== TEST DE VECTORES ===\n");
	t_vec3 v_x = vec3_init(1.0, 0.0, 0.0); // Eje X
	t_vec3 v_y = vec3_init(0.0, 1.0, 0.0); // Eje Y

	print_vec("Vector X", v_x);
	print_vec("Vector Y", v_y);

	/* Test Producto Punto (debe ser 0 porque son perpendiculares) */
	double dot = vec3_dot(v_x, v_y);
	printf("Producto Punto X.Y (Debe ser 0.00): %.2f\n", dot);

	/* Test Producto Cruz (debe darnos el Eje Z) */
	t_vec3 v_z = vec3_cross(v_x, v_y);
	print_vec("Producto Cruz XxY (Debe ser Z: 0.00, 0.00, 1.00)", v_z);

	/* Test Normalización y Longitud */
	t_vec3 v_random = vec3_init(0.0, 3.0, 4.0); // Triángulo pitagórico 3-4-5
	printf("Longitud de (0,3,4) (Debe ser 5.00): %.2f\n", vec3_length(v_random));
	print_vec("Normalizado de (0,3,4) (Debe ser 0.00, 0.60, 0.80)", vec3_normalize(v_random));

	printf("\n=== TEST DE MATRICES ===\n");
	t_matrix4 mat;
	matrix4_set_identity(&mat);

	/* Trucamos la matriz identidad para añadirle una TRASLACIÓN de +5 en X, Y, Z */
	mat.elements[0][3] = 5.0;
	mat.elements[1][3] = 5.0;
	mat.elements[2][3] = 5.0;

	t_point3 p = vec3_init(1.0, 1.0, 1.0); // Es un Punto (Posición)
	t_vec3 dir = vec3_init(1.0, 1.0, 1.0); // Es un Vector (Dirección)

	t_point3 p_trans = matrix4_mul_point3(&mat, &p);
	t_vec3 dir_trans = matrix4_mul_vec3(&mat, &dir);

	/* El punto debería moverse a (6, 6, 6), pero la dirección NO debe cambiar */
	print_vec("Punto trasladado     (Debe ser 6.00, 6.00, 6.00)", p_trans);
	print_vec("Direccion trasladada (Debe ser 1.00, 1.00, 1.00)", dir_trans);

	printf("\n¡Si todos los 'Debe ser' coinciden, el Modulo 1 es un EXITO!\n");
	return (0);
}
