/* Programmer: Brian Mitzel
 * Email: bmitzel@csu.fullerton.edu
 * Course: CPSC 486
 *
 * Filename: Trackball.cpp
 *
 * A C++ module implementing a virtual trackball for
 * calculating a rotation quaternion based on two input
 * window coordinates which, when projected onto the
 * trackball's surface, define the beginning and end
 * of the trackball's path.
 */

#include "Trackball.h"

/**
 * Default constructor initializes the trackball with zero rotation
 */
Trackball::Trackball(){
	//blank	
};


void Trackball::setPoints(Vec2 start, Vec2 end){
	endPoint = ProjectToSphere(end[0], end[1]);
	startPoint = ProjectToSphere(start[0], start[1]);
}

void Trackball::SetCenter(int x, int y){
	center[0] = x;
	center[1] = y;
}

Quat Trackball::GetRotation(){
    /* Check for unequal starting and ending points */
    if (startPoint[0] != endPoint[0] || startPoint[1] != endPoint[1])
    {
	/* Calculate the axis and distance of the trackball's rotation */
	Vec3 a = normalize(Vec3(startPoint));
	Vec3 b = normalize(Vec3(endPoint));
	Vec3 axis = normalize(cross(a, b));
	Vec3 difference = endPoint - startPoint;
	float distance = length(difference) / (2.0f * radius);

	/* Clamp the distance to values between 0.0 and 1.0 */
	if (distance > 1.0f)
	{
	    distance = 1.0f;
	}

	/* Calculate the angle of rotation */
	float angle = 2.0f * asin(distance);

	/* Add the new axis and angle to the trackball's rotation */
	return Quat(axis, angle);
    }
    else
    {
	return Quat(); // zero rotation
    }
}

Vec3 Trackball::ProjectToSphere(float x, float y){
	float z;

	/* Adjust the x and y components to be in relation to the center of the trackball */
	x -= center[0];
	y -= center[1];

	/* The distance from the center of the trackball to the window coordinates */
	float dist = sqrtf((x - center[0]) * (x - center[0]) + (y - center[1]) * (y - center[1]));

	/* Constant for determining whether to project to sphere or hyperbolic sheet */
	float t = radius * M_SQRT1_2;

	/* Tracking on the sphere */
	if (dist < t)
	{
		z = -sqrtf(radius * radius - dist * dist);
	}
	/* Tracking outside the sphere on the hyperbolic sheet
	 * i.e. near the corners of the graphical window
	 */
	else
	{
		z = -(t * t / dist);
	}

	return Vec3(x, y, z);
}
