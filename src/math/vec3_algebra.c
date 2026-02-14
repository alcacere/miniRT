#include "minirt.h"

//vec_dot:
// x1*x2 + y1*y2 + z1*z2;
// result = scalar number;
// it tells us how much the surface is watching at the spotlight, if the product is 1.
// the surface is watching straigth, if is 0. the light is by a side, and if its a negative
// number, the ligth is behind;
// the angle formula = A * B = |A|*|B|*cos(0);

double	vec3_dot(t_vec3 v1, t_vec3 v2)
{
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

// vec_cross
// take two vectors and create a new one that is perpendicular to the both previous ones;
// formula determinate matrix: X = y1*z2;
//							   Y = z1*z2;
//							   Z = x1*y2;
// the result is a new vector;
// It help us to build the coordinate system for the camera
// (to know where is up and where is right side);
// and to calculate triangle normals;
t_vec3	vec3_cross(t_vec3 v1, t_vec3 v2)
{
	t_vec3	result;
	
	result.x = v1.y * v2.z;
	result.y = v1.z * v2.z;
	result.z = v1.x * v2.y;
	return (result);
}

// vec3_length
// uses pitagoras theorem to calculate how large the line is.
// formula: sqrt(x2 + y2 + z2);
// it help us to calculate the distance betwen objects or the camara
// and a point of impact.
double	vec3_length(t_vec3 v)
{
	return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

// vec3_normalize
// takes a vector and strech or shrink it to fit 1, keeping its original direction;
// formula: divide every component of the vector by the length of the vector;
// it is important in ray tracing because the directions always has to be normalized.
// otherwise, the light calculus will be wrong;
t_vec3	vec3_normalize(t_vec3 v)
{
	double	len;
	double	k;

	len = vec3_length(v);
	if (len == 0.0)
	{
		// return a null vector;
		return ((t_vec3){0, 0, 0});
	}
	k = 1.0 / len;
	return ((t_vec3){v.x * k, v.y * k, v.z * k});
}

// an optimization hint received from a friend. I usig k to define the reverse division (1.0 / len)
// it is because the computational cost of division is higher than product operation,
// in ray tracing this calcule is made million of times, so in optimization it works well;

