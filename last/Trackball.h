/* Programmer: Brian Mitzel
 * Email: bmitzel@csu.fullerton.edu
 * Course: CPSC 486
 *
 * Filename: Trackball.h
 *
 * A C++ module implementing a virtual trackball for
 * calculating a rotation quaternion based on two input
 * window coordinates which, when projected onto the
 * trackball's surface, define the beginning and end
 * of the trackball's path.
 */

#ifndef TRACKBALL_H_
#define TRACKBALL_H_

#define _USE_MATH_DEFINES
#include <cmath>

#include "Point3.h"
#include "Quaternion.h"
#include "Vec3.h"
#include "VecMath.h"

class Trackball{
public:
	Vec2 center;
	Vec3 startPoint;          /* the first point used in calculating the trackball's rotation */
	Vec3 endPoint;          /* the second point used in calculating the trackball's rotation */
	float radius;           /* the trackball's radius */
	bool state;              /* the trackball's on/off state */

	Trackball();
	
	void setPoints(Vec2 start, Vec2 end);
	void SetCenter(int x, int y);

}; /* Trackball class */

#endif /* TRACKBALL_H_ */
