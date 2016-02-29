// 
// Michael Shafae
// mshafae at fullerton.edu
// 
// Procedural module that implements transformations used in
// the homework assignment.
//
// $Id: transformations.cpp 5554 2015-02-19 06:59:50Z mshafae $
//
// STUDENTS _MUST_ ADD THEIR CODE INTO THIS FILE
//

#include "transformations.h"

typedef struct quaternion{
	float x,y,z,w;

	quaternion multi(quaternion B){
  		quaternion C;

  		C.x = w*B.x + x*B.w + y*B.z - z*B.y;
  		C.y = w*B.y - x*B.z + y*B.w + z*B.x;
  		C.z = w*B.z + x*B.y - y*B.x + z*B.w;
  		C.w = w*B.w - x*B.x - y*B.y - z*B.z;
  		return C;
	}

	quaternion conjugate(quaternion Q){
		Q.x = -Q.x;
  		Q.y = -Q.y;
  		Q.z = -Q.z;
		return Q;
	}

	

}quaternion;

void translate(Vec3& centerPosition, float x, float y, float z){
	centerPosition[0]+=x;
	centerPosition[1]+=y;
	centerPosition[2]+=z;
}

void moveLeft(Vec3& centerPosition){
	if(true){	centerPosition[1] = centerPosition[1] + 0.01f; }
	else{	centerPosition[1] = centerPosition[1] - 0.01f; }
}

void quatRotate(Vec3& centerPosition, float x, float y, float z, float w){
	quaternion temp, quat_view, result, conjugate;

	temp.x = x * sin(w/2);
	temp.y = y * sin(w/2);
	temp.z = z * sin(w/2);
	temp.w = cos(w/2);

	w *= 50.0; //Scale w to increase/decrease trackball speed

	quat_view.x = centerPosition[0];
	quat_view.y = centerPosition[1];
	quat_view.z = centerPosition[2];
	quat_view.w = 0;

	conjugate = conjugate.conjugate(temp);
	result = temp.multi(quat_view);
	result = result.multi(conjugate);


	centerPosition = Vec3(result.x,result.y,result.z);
}


void rotateCameraLeft(float degrees, Vec3& eyePosition, Vec3& centerPosition, Vec3& upVector){
	degrees /= 50;	
	Mat3 yMat3(	cos(degrees),	0,	sin(degrees),	
			0,		1,	0,	
			-sin(degrees),	0,	cos(degrees));
	eyePosition = yMat3 * eyePosition;
	centerPosition = yMat3 * centerPosition;
	upVector = yMat3 * upVector;

}

void rotateCameraUp(float degrees, Vec3& eyePosition, Vec3& centerPosition, Vec3& upVector){
	degrees /= 50;
	Mat3 xMat3(	1,	0,		0,	
			0,	cos(degrees),	-sin(degrees),	
			0,	sin(degrees),	cos(degrees));
	eyePosition = xMat3 * eyePosition;
	centerPosition = xMat3 * centerPosition;
	upVector = xMat3 * upVector;
}


