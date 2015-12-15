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