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

void translate(Vec3& centerPosition, float x, float y, float z){
	centerPosition[0]+=x;
	centerPosition[1]+=y;
	centerPosition[2]+=z;
}

void moveLeft(Vec3& centerPosition){
	centerPosition[0] = centerPosition[0] + 0.01f;
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

void arbitratyRotation(float degrees, Vec3& eyePosition, Vec3& centerPosition, Vec3& upVector, Vec3& axis){
	normalize(axis);
	float u = axis[0], v = axis[1], w = axis[2];	
	//float l = u*u+v*v+ w*w;
	float a = 0, b = 0, c = 0;
	degrees = degrees/5;	

	Mat4 rMat(	u*u+(u*u + w*w)*cos(degrees),	u*v*(1-cos(degrees))-w*sin(degrees),	u*w*(1-cos(degrees))+v*sin(degrees),	(a*(v*v+w*w)-u*(b*v+c*w))*(1-cos(degrees))+(b*w-c*v)*sin(degrees),
			u*v*(1-cos(degrees))+w*sin(degrees),	v*v+(u*u+w*w)*cos(degrees),	v*w*(1-cos(degrees))-u*sin(degrees),	(b*(u*u+w*w)-v*(a*u+c*w))*(1-cos(degrees))+(c*u-a*w)*sin(degrees),
			u*w*(1-cos(degrees))-v*sin(degrees),	v*w*(1-cos(degrees))+u*sin(degrees),	w*w+(u*u+v*v)*cos(degrees),	(c*(u*u+v*v)-w*(a*u+b*v))*(1-cos(degrees))+(a*v-b*u)*sin(degrees),
			0,				0,				0,				1);

	Mat3 rMat3(	u*u+(v*v + w*w)*cos(degrees),		u*v*(1-cos(degrees))-w*sin(degrees),	u*w*(1-cos(degrees))+v*sin(degrees),
			u*v*(1-cos(degrees))+w*sin(degrees),	v*v+(u*u+w*w)*cos(degrees),		v*w*(1-cos(degrees))-u*sin(degrees),
			u*w*(1-cos(degrees))-v*sin(degrees),	v*w*(1-cos(degrees))+u*sin(degrees),	w*w+(u*u+v*v)*cos(degrees));

	//axis = Vec3(1,0,0);
	u = axis[0], v = axis[1], w = axis[2];
	degrees = 0.01f;

	float t = (1- cos(degrees));
	float C = cos(degrees);
	float S = sin(degrees);
	Mat3 rMat2(	t*u*u+C,	t*u*v-S*w,	t*u*w+S*v,
			t*u*v+S*w,	t*v*v+C,	t*v*w-S*u,
			t*u*w-S*v,	t*v*w+S*u,	t*w*w+C);
	/*
	Vec4 myEye(eyePosition, 0);
	myEye = rMat * myEye;
	eyePosition = Vec3(myEye[0], myEye[1], myEye[2]);

	Vec4 myCenter(centerPosition, 0);
	myCenter = rMat * myCenter;
	centerPosition = Vec3(myCenter[0], myCenter[1], myCenter[2]);

	Vec4 myUp(upVector, 0);
	myUp = rMat * myUp;
	upVector = Vec3(myUp[0], myUp[1], myUp[2]);
	*/
	eyePosition = rMat2 * eyePosition;
	centerPosition = rMat2 * centerPosition;
	upVector = rMat2 * upVector;

	//Vec3 f = normalize(-eyePosition);//gaze
	//Vec3 _up = normalize(upVector);//up
	//Vec3 s = normalize(cross(f, _up));//right
  	//Vec3 u = cross(s, f);
 	//Mat3 m = rotate3(-degrees, axis);
  	//upVector = axis;
  	//eyePosition = m * eyePosition;
	//eyePosition[1] += degrees;
}