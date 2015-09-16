#ifndef QUATERNION_H
 #define QUATERNION_H

#include <glm/glm.hpp>
#include <cmath>

 
#ifndef PI
 #define PI 3.14159265359
#endif


// Quaternion class, based on the program quaternionAnimation.cpp, from the Computer Graphics Through OpengGL book code.
class Quaternion{
public:
	Quaternion() {
		Quaternion(0,0,0,0);
	}

	Quaternion(double w,double x, double y, double z) {
		u.x=x;
		u.y=y;
		u.z=z;
		this->w=w;
       	}

	double w;
	glm::vec3 u;

	Quaternion  operator* (const Quaternion &q)
	{

		Quaternion tmp= *this;
		tmp.u.x = ((w * q.u.x) + (u.x * q.w) + (u.y * q.u.z) - (u.z * q.u.y));
		tmp.u.y = ((w * q.u.y) - (u.x * q.u.z) + (u.y * q.w) + (u.z * q.u.x));
		tmp.u.z = ((w * q.u.z) + (u.x * q.u.y) - (u.y * q.u.x) + (u.z * q.w));
		tmp.w = ((w * q.w) - (u.x * q.u.x) - (u.y * q.u.y) - (u.z * q.u.z));


		return tmp;
	}

	void to_matrix(double *matrix) 
	{
		float wx, wy, wz, xx, yy, yz, xy, xz, zz;
		
		xx = u.x * u.x;
		xy = u.x * u.y;
		xz = u.x * u.z;
		yy = u.y * u.y;
		zz = u.z * u.z;
		yz = u.y * u.z;

		wx = w * u.x;
		wy = w * u.y;
		wz = w * u.z;

		matrix[0] = 1.0f - 2.0f*(yy + zz);
		matrix[4] = 2.0f*(xy - wz);
		matrix[8] = 2.0f*(xz + wy);
		matrix[12] = 0.0;
	 
		matrix[1] = 2.0f*(xy + wz);
		matrix[5] = 1.0f - 2.0f*(xx + zz);
		matrix[9] = 2.0f*(yz - wx);
		matrix[13] = 0.0;

		matrix[2] = 2.0f*(xz - wy);
		matrix[6] = 2.0f*(yz + wx);
		matrix[10] = 1.0f - 2.0f*(xx + yy);
		matrix[14] = 0.0;

		matrix[3] = 0;
		matrix[7] = 0;
		matrix[11] = 0;
		matrix[15] = 1;
	}

	double scale() {
		return ((u.x*u.x) + (u.y*u.y) + (u.z*u.z));
	}

	bool is_identity() {
		return ( w == 1.0 && u.x == 0.0 && u.y == 0.0 && u.z == 0.0 );
	}

	// RotAxis should be:
	// double[4], where: {theta(radians), ax,ay,az}
	void to_rotation_axis(double *rotAxis) {
		double sc = scale();
		if (  sc == 0.0 ) {
			rotAxis[0] = 0.0;
			rotAxis[1] = 0.0;
			rotAxis[2] = 0.0;
			rotAxis[3] = 0.0;
		} else {
			rotAxis[0] = 2.0 * acos(w);
			rotAxis[1] = u.x / sc;
			rotAxis[2] = u.y / sc;
			rotAxis[3] = u.z / sc;
		}



	}
};

#endif
