Mat4 rMat(	u*u+(u*u + w*w)*cos(degrees),	u*v*(1-cos(degrees))-w*sin(degrees),	u*w*(1-cos(degrees))+v*sin(degrees),	(a*(v*v+w*w)-u*(b*v+c*w))*(1-cos(degrees))+(b*w-c*v)*sin(degrees),
			u*v*(1-cos(degrees))+w*sin(degrees),	v*v+(u*u+w*w)*cos(degrees),	v*w*(1-cos(degrees))-u*sin(degrees),	(b*(u*u+w*w)-v*(a*u+c*w))*(1-cos(degrees))+(c*u-a*w)*sin(degrees),
			u*w*(1-cos(degrees))-v*sin(degrees),	v*w*(1-cos(degrees))+u*sin(degrees),	w*w+(u*u+v*v)*cos(degrees),	(c*(u*u+v*v)-w*(a*u+b*v))*(1-cos(degrees))+(a*v-b*u)*sin(degrees),
			0,				0,				0,				1);

	Mat3 rMat3(	u*u+(v*v + w*w)*cos(degrees),		u*v*(1-cos(degrees))-w*sin(degrees),	u*w*(1-cos(degrees))+v*sin(degrees),
			u*v*(1-cos(degrees))+w*sin(degrees),	v*v+(u*u+w*w)*cos(degrees),		v*w*(1-cos(degrees))-u*sin(degrees),
			u*w*(1-cos(degrees))-v*sin(degrees),	v*w*(1-cos(degrees))+u*sin(degrees),	w*w+(u*u+v*v)*cos(degrees));

	Mat4 yMat(	cos(degrees),	0,	sin(degrees),	0,	
			0,		1,	0,		0,
			-sin(degrees),	0,	cos(degrees),	0,
			0,		0,	0,		1);

	Mat3 yMat3(	cos(degrees),	0,	sin(degrees),	
			0,		1,	0,	
			-sin(degrees),	0,	cos(degrees));

	Mat3 xMat3(	1,	0,		0,
			0,	cos(degrees),	-sin(degrees),
			0,	sin(degrees),	cos(degrees));


//centerPosition[2] = (float)(eyePosition[2] + sin(-angle_y)*vVector[0] + cos(-angle_y)*vVector[2]);
			//centerPosition[0] = (float)(eyePosition[0] + cos(-angle_y)*vVector[0] - sin(-angle_y)*vVector[2]);

			Mat3 zMat3(	cos(angle_z),	sin(angle_z),	0,	
					-sin(angle_z),	cos(angle_z),	0,	
					0,		0,		1);
			eyePosition = zMat3 * eyePosition;
			centerPosition = zMat3 * centerPosition;
			upVector = zMat3 * upVector;

			Mat3 yMat3(	cos(angle_z),	0,	sin(angle_z),	
					0,		1,	0,	
					-sin(angle_z),	0,	cos(angle_z));
			centerPosition = yMat3 * centerPosition;

			Mat3 xMat3(	1,	0,		0,
					0,	cos(-angle_z),	-sin(-angle_z),
					0,	sin(-angle_z),	cos(-angle_z));
			upVector = xMat3 * upVector;


//failure below
float angle_y = 0.0f;				
			float angle_x = 0.0f;	
			float angle_z = 0.0f;						
			
			//Vec3 f = normalize(-eyePosition);//gaze
			//Vec3 _up = normalize(upVector);//up			
			//Vec3 rVec = normalize(cross(f, _up));//right

			// Get the direction from the mouse cursor
			angle_y = (float)( (oldPosition[0] - mousePosition[0]) ) / 500;		
			angle_x = (float)( (oldPosition[1] - mousePosition[1]) ) / 500;
			angle_z = sqrt(1-angle_y*angle_y-angle_x*angle_x);//cos(angle_y) + sin(angle_x);

			// Get the view vector
			Vec3 vVector = centerPosition - eyePosition;
			
			//Vec3 direction(cos(angle_y) * sin(angle_x), sin(angle_y), cos(angle_y) * cos(angle_x));
			Vec3 direction(angle_x, angle_y, 1);
			
			//centerPosition[2] = (float)(eyePosition[2] + sin(-angle_y)*vVector[0] + cos(-angle_y)*vVector[2]);
			//centerPosition[0] = (float)(eyePosition[0] + cos(-angle_y)*vVector[0] - sin(-angle_y)*vVector[2]);
			
			Mat3 zMat3(	cos(angle_z),	sin(angle_z),	0,	
					-sin(angle_z),	cos(angle_z),	0,	
					0,		0,		1);
			//eyePosition = zMat3 * eyePosition;
			centerPosition = zMat3 * centerPosition;
			//upVector = zMat3 * upVector;

			Mat3 yMat3(	cos(angle_y),	0,	sin(angle_y),	
					0,		1,	0,	
					-sin(angle_y),	0,	cos(angle_y));
			//eyePosition = yMat3 * eyePosition;
			centerPosition = yMat3 * centerPosition;
			upVector = yMat3 * upVector;

			Mat3 xMat3(	1,	0,		0,
					0,	cos(-angle_x),	-sin(-angle_x),
					0,	sin(-angle_x),	cos(-angle_x));
			//eyePosition = xMat3 * eyePosition;
			centerPosition = xMat3 * centerPosition;
			upVector = xMat3 * upVector;



			Vec3 right( sin(angle_x - 3.14f/2.0f),  0, cos(angle_x - 3.14f/2.0f));

			modelViewMatrix = lookat(Vec3(0,0,0), Vec3(0,0,-6), Vec3(0,1,0));



void Camera::SetViewByMouse(void)
{
  // the coordinates of our mouse coordinates
  int MouseX, MouseY;

  // the middle of the screen in the x direction
  int MiddleX = SCREENWIDTH/2;

  // the middle of the screen in the y direction
  int MiddleY = SCREENHEIGHT/2;

  // vector that describes mouseposition - center
  Vector MouseDirection(0, 0, 0);

  // static variable to store the rotation about the x-axis, since
  // we want to limit how far up or down we can look.
  // We don't need to cap the rotation about the y-axis as we
  // want to be able to turn around 360 degrees
  static double CurrentRotationAboutX = 0.0;

  // The maximum angle we can look up or down, in radians
  double maxAngle = 1;

  // This function gets the position of the mouse
  SDL_GetMouseState(&MouseX, &MouseY);

  // if the mouse hasn't moved, return without doing
  // anything to our view
  if((MouseX == MiddleX) && (MouseY == MiddleY))
    return;

  // otherwise move the mouse back to the middle of the screen
  SDL_WarpMouse(MiddleX, MiddleY);

  // get the distance and direction the mouse moved in x (in
  // pixels). We can't use the actual number of pixels in radians,
  // as only six pixels  would cause a full 360 degree rotation.
  // So we use a mousesensitivity variable that can be changed to
  // vary how many radians we want to turn in the x-direction for
  // a given mouse movement distance

  // We have to remember that positive rotation is counter-clockwise. 
  // Moving the mouse down is a negative rotation about the x axis
  // Moving the mouse right is a negative rotation about the y axis
  MouseDirection.x = (MiddleX - MouseX)/MouseSensitivity; 
  MouseDirection.y = (MiddleY - MouseY)/MouseSensitivity;

  CurrentRotationX += MouseDirection.y;
  
  // We don't want to rotate up more than one radian, so we cap it.
  if(CurrentRotationX > 1)
  {
    CurrentRotationX = 1;
    return;
  }
  // We don't want to rotate down more than one radian, so we cap it.
  if(CurrentRotationX < -1)
  {
    CurrentRotationX = -1;
    return;
  }
  else
  {
    // get the axis to rotate around the x-axis. 
    Vector Axis = CrossProduct(View - Position, Up);
    // To be able to use the quaternion conjugate, the axis to
    // rotate around must be normalized.
    Axis = Normalize(Axis);

    // Rotate around the y axis
    RotateCamera(MouseDirection.y, Axis.x, Axis.y, Axis.z);
    // Rotate around the x axis
    RotateCamera(MouseDirection.x, 0, 1, 0);
  }
}

void RotateCamera(double Angle, double x, double y, double z)
{
  quaternion temp, quat_view, result;

  temp.x = x * sin(Angle/2);
  temp.y = y * sin(Angle/2);
  temp.z = z * sin(Angle/2);
  temp.w = cos(Angle/2);

  quat_view.x = View.x;
  quat_view.y = View.y;
  quat_view.z = View.z;
  quat_view.w = 0;

  result = mult(mult(temp, quat_view), conjugate(temp));

  View.x = result.x;
  View.y = result.y;
  View.z = result.z;
}
