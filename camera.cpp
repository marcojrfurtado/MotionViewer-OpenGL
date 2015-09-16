#include "camera.hpp"
#include <cmath>

Camera::Camera() {
	startPosition();
}

void Camera::startPosition() {

	this->center.x = 0.0;
	this->center.y = 0.0;
	this->center.z = -1.0;


	this->eye.x = 0.0;
	this->eye.y = 0.0;
	this->eye.z = 0.0;

	this->up.x = 0.0;
	this->up.y = 1.0;
	this->up.z = 0.0;

	this->angle.x = 0.0;
	this->angle.y = 0.0;
	this->angle.z = 0.0;
}

void Camera::translate( float x, float y, float z, bool keep_center) {

	eye.x+=x;
	eye.y+=y;
	eye.z+=z;

	if ( !keep_center ) {
		center.x+=x;  // If these lines are commented
		center.y+=y;  // the camera keeps looking at the object while moving
		center.z+=z;
	}

}


void Camera::rotate( float angx, float angy, float angz) {

	
	angle.z+=angz;
	if (angle.z > 360.0) angle.z -= 360.0;
	if (angle.z < 0.0) angle.z += 360.0;
//	double zradian = angle.z / ( PI * 180 );

// NOT necessary	
//	up.x = sin(zradian);
//	up.y = cos(zradian);


	
	angle.y+=angy;
	if (angle.y > 360.0) angle.y -= 360.0;
	if (angle.y < 0.0) angle.y += 360.0;
//	double yradian = angle.y / ( PI * 180 );

//	center.x = sin(yradian) + eye.x;
//	center.z = -cos(yradian) + eye.z;

	

	angle.x+=angx;
	if (angle.x > 360.0) angle.x -= 360.0;
	if (angle.x < 0.0) angle.x += 360.0;
//	double xradian = angle.x / ( PI * 180 );

//	center.y= -sin(xradian) + eye.y;
//	center.z= -cos(xradian) + eye.z;
//	up.z = -sin(xradian);
//	up.y = cos(xradian);

}
