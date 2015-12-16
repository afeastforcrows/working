// 
// Michael Shafae
// mshafae at fullerton.edu
// 
// The Utah teapot floating in space. Camera can be rotated up/down and 
// left right. The camera's transformations are defined/implemented in
// transformations.h/cpp.
//
// This demonstration program requires that you provide your own GFXMath.h.
//
// $Id: picking_glfw.cpp 5918 2015-09-12 01:37:43Z mshafae $
//

#include <cstdlib>
#include <cstdio>
#include <sys/time.h>

#include "GLFWApp.h"
#include "GFXMath.h"
#include "GFXExtra.h"

#include "GLSLShader.h"
#include "transformations.h"
#include "PlyModel.h"
#include "glut_teapot.h"

void msglVersion(void){
  fprintf(stderr, "OpenGL Version Information:\n");
  fprintf(stderr, "\tVendor: %s\n", glGetString(GL_VENDOR));
  fprintf(stderr, "\tRenderer: %s\n", glGetString(GL_RENDERER));
  fprintf(stderr, "\tOpenGL Version: %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "\tGLSL Version: %s\n",
          glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void msglPrintMatrix16dv( char *varName, double matrix[16] ){
  int i = 0;
  if( varName != NULL ){
    fprintf( stderr, "%s = [\n", varName );
  }
  for( i = 0; i < 4; i++ ){
    fprintf( stderr, "%.5f %.5f %.5f %.5f\n", matrix[i + 0], matrix[i + 4],
    matrix[i + 8], matrix[i + 12] );
  }
  if( varName != NULL ){
    fprintf( stderr, " ]\n" );
  }
}

typedef struct ray{
  // origin
  Vec3 o;
  // direction
  Vec3 d;
}ray;

typedef struct triangle{
  Vec3 a;
  Vec3 b;
  Vec3 c;
}triangle3d;

typedef struct square{
  Vec3 a;
  Vec3 b;
  Vec3 c;
  Vec3 d;
}square3d;


typedef struct hit{
  double t;
  Vec3 point;
}hitd;

void rayEval3(Vec3& p, ray& r, double t){
  int i;
  for(i = 0; i < 3; i++){
    p[i] = r.o[i] + (t * r.d[i]); 
  }
}

bool rayTriangleIntersectiond(hit& h, ray& r, triangle& t){
  return(true);  
}


class PickingApp : public GLFWApp{
private:
	FaceList* myPly[4];
  triangle tri;
  triangle tri2;
  triangle tri3;
  triangle tri4;
	square mySquare[5];
	triangle myTri[4];
  
  float rotationDelta;

	Vec2 mousePosition;

  Vec3 centerPosition;
  Vec3 eyePosition;
  Vec3 upVector;

  Mat4 modelViewMatrix;
  Mat4 projectionMatrix;
  Mat4 normalMatrix;
  
  GLSLProgram shaderProgram;

  Vec4 light0;
  Vec4 light1; 

  // Variables to set uniform params for lighting fragment shader 
  unsigned int uModelViewMatrix; 
  unsigned int uProjectionMatrix; 
  unsigned int uNormalMatrix; 
  unsigned int uLight0_position;
  unsigned int uLight0_color;
  unsigned int uLight1_position;
  unsigned int uLight1_color;
  unsigned int uAmbient;
  unsigned int uDiffuse;
  unsigned int uSpecular;
  unsigned int uShininess;
  
public:
  PickingApp(int argc, char* argv[]) :
    GLFWApp(argc, argv, std::string("Camera Control").c_str( ), 500, 500){ }
  
  void initCenterPosition( ){
    centerPosition = Vec3(0.0, 0.0, 0.0);
  }
  
  void initEyePosition( ){
    eyePosition = Vec3(0.0, 0.0, 6.0);
  }

  void initUpVector( ){
    upVector = Vec3(0.0, 1.0, 0.0);
  }

  void initRotationDelta( ){
    rotationDelta = 0.5;
  }
 
	Vec4 translate4(Vec4 myVec, float x, float y, float z, float w){
			myVec[0]+=x;
			myVec[1]+=y;
			myVec[2]+=z;
			myVec[3]+=w;
		return myVec;
	}

	Vec4 scale4(Vec4 myVec, float s){
			myVec[0] *= s;
			myVec[1] *= s;
			myVec[2] *= s;
			myVec[3] *= s;
		return myVec;
	}
 
	void drawBB(Vec3 center, float width){
		//we really only need to draw the front face
		Vec4 vertices[8];
		vertices[0] = Vec4(	-0.5,	-0.5,	-0.5,	1.0);
	     	vertices[1] = Vec4(	0.5,	-0.5,	-0.5,	1.0);
	     	vertices[2] = Vec4(	0.5,	0.5,	-0.5,	1.0);
	    	vertices[3] = Vec4(	-0.5,	0.5,	-0.5,	1.0);
	    	vertices[4] = Vec4(	-0.5,	-0.5,	0.5,	1.0);
	     	vertices[5] = Vec4(	0.5,	-0.5,	0.5,	1.0);
	     	vertices[6] = Vec4(	0.5,	0.5,	0.5,	1.0);
	    	vertices[7] = Vec4(	-0.5,	0.5,	0.5,	1.0);
		for(int i = 0; i<8; i++){
			vertices[i] = scale4(vertices[i], width*2);
		}
		for(int i = 0; i<8; i++){
			vertices[i] = translate4(vertices[i], center[0], center[1], center[2], 1);
		}

		glBegin(GL_QUADS);      // draw a cube with 6 quads
    		glVertex3fv(vertices[0]);    //back
    		glVertex3fv(vertices[1]);
   		glVertex3fv(vertices[2]);
    		glVertex3fv(vertices[3]);

    		glVertex3fv(vertices[0]);	//left
    		glVertex3fv(vertices[3]);
    		glVertex3fv(vertices[7]);
    		glVertex3fv(vertices[4]);

    		glVertex3fv(vertices[1]);	//right
    		glVertex3fv(vertices[5]);
    		glVertex3fv(vertices[6]);
    		glVertex3fv(vertices[2]);

    		glVertex3fv(vertices[2]);	
    		glVertex3fv(vertices[3]);
    		glVertex3fv(vertices[7]);
    		glVertex3fv(vertices[6]);

		glVertex3fv(vertices[1]);	//bottom
    		glVertex3fv(vertices[0]);
    		glVertex3fv(vertices[4]);
    		glVertex3fv(vertices[5]);

		glVertex3fv(vertices[4]);	//front
    		glVertex3fv(vertices[7]);
    		glVertex3fv(vertices[6]);
    		glVertex3fv(vertices[5]);

		glEnd();
		
	}

	/*void draw_bbox(FaceList fl) {
	  if (fl.vc == 0)
	    return;

	  // Cube 1x1x1, centered on origin
	  GLfloat vertices[] = {
	    -0.5, -0.5, -0.5, 1.0,
	     0.5, -0.5, -0.5, 1.0,
	     0.5,  0.5, -0.5, 1.0,
	    -0.5,  0.5, -0.5, 1.0,
	    -0.5, -0.5,  0.5, 1.0,
	     0.5, -0.5,  0.5, 1.0,
	     0.5,  0.5,  0.5, 1.0,
	    -0.5,  0.5,  0.5, 1.0,
	  };
	  GLuint vbo_vertices;
	  glGenBuffers(1, &vbo_vertices);
	  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	  glBindBuffer(GL_ARRAY_BUFFER, 0);

	  GLushort elements[] = {
	    0, 1, 2, 3,
	    4, 5, 6, 7,
	    0, 4, 1, 5,
	    2, 6, 3, 7
	  };
	  GLuint ibo_elements;
	  glGenBuffers(1, &ibo_elements);
	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	  GLfloat
	    min_x, max_x,
	    min_y, max_y,
	    min_z, max_z;
	  min_x = max_x = fl.vertices[0][0];
	  min_y = max_y = fl.vertices[0][1];
	  min_z = max_z = fl.vertices[0][2];
	  for (int i = 0; i < fl.vc; i++) {
	    if (fl.vertices[i][0] < min_x) min_x = fl.vertices[i][0];
	    if (fl.vertices[i][0] > max_x) max_x = fl.vertices[i][0];
	    if (fl.vertices[i][1] < min_y) min_y = fl.vertices[i][1];
	    if (fl.vertices[i][1] > max_y) max_y = fl.vertices[i][1];
	    if (fl.vertices[i][2] < min_z) min_z = fl.vertices[i][2];
	    if (fl.vertices[i][2] > max_z) max_z = fl.vertices[i][2];
	  }
	  Vec3 size = Vec3(max_x-min_x, max_y-min_y, max_z-min_z);
	  Vec3 center = Vec3((min_x+max_x)/2, (min_y+max_y)/2, (min_z+max_z)/2);
	  Vat4 transform =  translate(Mat4(1), center)* scale(Mat4(1), size);

	  // Apply object's transformation matrix 
	  //Mat4 m = fl->object2world * transform;
	  glUniformMatrix4fv(uniform_m, 1, GL_FALSE, value_ptr(m));

	  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	  glEnableVertexAttribArray(attribute_v_coord);
	  glVertexAttribPointer(
	    attribute_v_coord,  // attribute
	    4,                  // number of elements per vertex, here (x,y,z,w)
	    GL_FLOAT,           // the type of each element
	    GL_FALSE,           // take our values as-is
	    0,                  // no extra data between each position
	    0                   // offset of first element
	  );

	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
	  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4*sizeof(GLushort)));
	  glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8*sizeof(GLushort)));
	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	  glDisableVertexAttribArray(attribute_v_coord);
	  glBindBuffer(GL_ARRAY_BUFFER, 0);

	  glDeleteBuffers(1, &vbo_vertices);
	  glDeleteBuffers(1, &ibo_elements);
	}*/

	void drawSphere(float radius, int slices, int stacks){
		GLUquadricObj *quadObj;
		quadObj = gluNewQuadric();
		assert(quadObj);
		
		//gluQuadricDrawStyle(quadObj, GLU_FILL);
  		//gluQuadricNormals(quadObj, GLU_SMOOTH);
  		
  		gluQuadricDrawStyle(quadObj, GLU_LINE);
  		gluQuadricNormals(quadObj, GLU_SMOOTH);

		glTranslatef(0.0f, 0.8f, 0.0f);
  		gluSphere(quadObj, radius, slices, stacks);
	}

	void initMySquare(){
		float distance = 15;
		//1
		mySquare[0].a[0]=-distance;
		mySquare[0].a[1]=-2;
		mySquare[0].a[2]=distance;

		mySquare[0].b[0]=-distance;
		mySquare[0].b[1]=20;
		mySquare[0].b[2]=distance;

		mySquare[0].c[0]=distance;
		mySquare[0].c[1]=20;
		mySquare[0].c[2]=distance;

		mySquare[0].d[0]=distance;
		mySquare[0].d[1]=-2;
		mySquare[0].d[2]=distance;
		//2////////////////////////////////////////////////////
		mySquare[1].a[0]=distance;
		mySquare[1].a[1]=-2;
		mySquare[1].a[2]=distance;

		mySquare[1].b[0]=distance;
		mySquare[1].b[1]=20;
		mySquare[1].b[2]=distance;

		mySquare[1].c[0]=distance;
		mySquare[1].c[1]=20;
		mySquare[1].c[2]=-distance;

		mySquare[1].d[0]=distance;
		mySquare[1].d[1]=-2;
		mySquare[1].d[2]=-distance;
		//3////////////////////////////////////////////////////////
		mySquare[2].a[0]=distance;
		mySquare[2].a[1]=-2;
		mySquare[2].a[2]=-distance;

		mySquare[2].b[0]=distance;
		mySquare[2].b[1]=20;
		mySquare[2].b[2]=-distance;

		mySquare[2].c[0]=-distance;
		mySquare[2].c[1]=20;
		mySquare[2].c[2]=-distance;

		mySquare[2].d[0]=-distance;
		mySquare[2].d[1]=-2;
		mySquare[2].d[2]=-distance;
		//4
		mySquare[3].a[0]=-distance;
		mySquare[3].a[1]=-2;
		mySquare[3].a[2]=-distance;

		mySquare[3].b[0]=-distance;
		mySquare[3].b[1]=20;
		mySquare[3].b[2]=-distance;

		mySquare[3].c[0]=-distance;
		mySquare[3].c[1]=20;
		mySquare[3].c[2]=distance;

		mySquare[3].d[0]=-distance;
		mySquare[3].d[1]=-2;
		mySquare[3].d[2]=distance;
		//5///////////////////////////
		mySquare[4].a[0]=-distance;
		mySquare[4].a[1]=20;
		mySquare[4].a[2]=-distance;

		mySquare[4].b[0]=-distance;
		mySquare[4].b[1]=20;
		mySquare[4].b[2]=distance;

		mySquare[4].c[0]=distance;
		mySquare[4].c[1]=20;
		mySquare[4].c[2]=distance;

		mySquare[4].d[0]=distance;
		mySquare[4].d[1]=20;
		mySquare[4].d[2]=-distance;
	}

	void initMyTri(){
		//4
		myTri[3].b[0] = 1.0;
   		myTri[3].b[1] = 1.0;
    		myTri[3].b[2] = 0.0;
  
    		myTri[3].a[0] = 0.0;
    		myTri[3].a[1] = -1.0;
    		myTri[3].a[2] = 1.0;
  
    		myTri[3].c[0] = 2.0;
    		myTri[3].c[1] = -1.0;
    		myTri[3].c[2] = 1.0;	
		//3
		myTri[2].a[0] = 1.0;
    		myTri[2].a[1] = 1.0;
    		myTri[2].a[2] = 0.0;
  
    		myTri[2].b[0] = 0.0;
    		myTri[2].b[1] = -1.0;
    		myTri[2].b[2] = 1.0;
  
    		myTri[2].c[0] = 1.0;
    		myTri[2].c[1] = -1.0;
    		myTri[2].c[2] = -1.0;
		//2
		myTri[1].a[0] = 1.0;
    		myTri[1].a[1] = 1.0;
    		myTri[1].a[2] = 0.0;
  
    		myTri[1].b[0] = 1.0;
    		myTri[1].b[1] = -1.0;
    		myTri[1].b[2] = -1.0;
  
    		myTri[1].c[0] = 2.0;
    		myTri[1].c[1] = -1.0;
    		myTri[1].c[2] = 1.0;
		//1
		myTri[0].a[0] = 0.0;
    		myTri[0].a[1] = -1.0;
    		myTri[0].a[2] = 1.0;
  
    		myTri[0].b[0] = 1.0;
    		myTri[0].b[1] = -1.0;
    		myTri[0].b[2] = -1.0;
  
    		myTri[0].c[0] = 2.0;
    		myTri[0].c[1] = -1.0;
    		myTri[0].c[2] = 1.0;
	}

	void updatePly(){
		for(int p = 0; p<4; p++){
			calcRitterBoundingSphere(myPly[p]->center, &(myPly[p]->radius), myPly[p]);
		}
	}

 
  bool begin( ){
    msglError( );
	initMyTri();
	initMySquare();
    initCenterPosition( );
    initEyePosition( );
    initUpVector( );
    initRotationDelta( );

	myPly[0] = readPlyModel("ply/trico.ply");
	//myPly[0]->translate(-2.0, 0.0, 2.0);
	myPly[0]->scale(2);
	calcRitterBoundingSphere(myPly[0]->center, &(myPly[0]->radius), myPly[0]);
	myPly[1] = readPlyModel("ply/spider.ply");
	myPly[1]->translate(2.0, 0.0, 2.0);
	myPly[2] = readPlyModel("ply/shark.ply");
	myPly[2]->translate(2.0, 4.0, 2.0);
	myPly[3] = readPlyModel("ply/urn.ply");
	myPly[3]->translate(-2.0, 4.0, 2.0);

    // Load the shader program
    const char* vertexShaderSource = "blinn_phong.vert.glsl";
    const char* fragmentShaderSource = "blinn_phong.frag.glsl";
    FragmentShader fragmentShader(fragmentShaderSource);
    VertexShader vertexShader(vertexShaderSource);
    shaderProgram.attach(vertexShader);
    shaderProgram.attach(fragmentShader);
    shaderProgram.link( );
    shaderProgram.activate( );
    
    printf("Shader program built from %s and %s.\n",
           vertexShaderSource, fragmentShaderSource);
    if( shaderProgram.isActive( ) ){
      printf("Shader program is loaded and active with id %d.\n", shaderProgram.id( ) );
    }else{
      printf("Shader program did not load and activate correctly. Exiting.");
      exit(1);
    }

    // Set up uniform variables
    uModelViewMatrix = glGetUniformLocation(shaderProgram.id( ),
                                            "modelViewMatrix");
    uProjectionMatrix = glGetUniformLocation(shaderProgram.id( ),
                                             "projectionMatrix");
    uNormalMatrix = glGetUniformLocation(shaderProgram.id( ),
                                         "normalMatrix");
    uLight0_position = glGetUniformLocation(shaderProgram.id( ),
                                            "light0_position");
    uLight0_color = glGetUniformLocation(shaderProgram.id( ),
                                         "light0_color");
    uLight1_position = glGetUniformLocation(shaderProgram.id( ),
                                            "light1_position");
    uLight1_color = glGetUniformLocation(shaderProgram.id( ),
                                         "light1_color");
    uAmbient = glGetUniformLocation(shaderProgram.id( ), "ambient");
    uDiffuse = glGetUniformLocation(shaderProgram.id( ), "diffuse");
    uSpecular = glGetUniformLocation(shaderProgram.id( ), "specular");
    uShininess = glGetUniformLocation(shaderProgram.id( ), "shininess");
    
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    msglVersion( );
    
    return !msglError( );
  }
  
  bool end( ){
    windowShouldClose( );
    return true;
  }
   
	void trackBall( float x1, float y1, float x2, float y2, float& angle, Vec3& axisOfRotation )
	{
		x1 = (2.0f * x1 - 500.0f)/500.0f;
		y1 = (2.0f * y1 - 500.0f)/500.0f;
		x2 = (2.0f * x2 - 500.0f)/500.0f;
		y2 = (2.0f * y2 - 500.0f)/500.0f;

		float changeX = (x1-x2);
		float changeY = (y1-y2);
		fprintf( stderr, "change it: %f, %f -- %f, %f\n", x1, y1, x2, y2);
		if(changeX==0&&changeY==0){
			return;
		}

		
		eyePosition[1] += changeY;
		eyePosition[0] += changeX;

		Vec3 vVector = eyePosition-centerPosition;		

		eyePosition[2] = (float)(eyePosition[2] + sin(changeX/1000)*vVector[0] + cos(changeX/1000)*vVector[2]);
		//eyePosition[0] = (float)(eyePosition[0] + cos(speed)*vVector[0] - sin(speed)*vVector[2]);
		/*
		//draw vectors from our two mouse points to the center of the sphere(?)
		Vec3 endVec((2.0f*x1-500.0f)/500.0f, (2.0f*y1-500.0f)/500.0f, 1.0f);
		Vec3 startVec((2.0f*x2-500.0f)/500.0f, (2.0f*y2-500.0f)/500.0f, 1.0f);

		endVec = endVec-eyePosition;
		startVec = startVec-eyePosition;

		axisOfRotation = cross(startVec, endVec);*/
		/*float angle = dot(startVec, endVec);

		float xx  = sqrt(abs(startVec[0])+abs(startVec[1])+abs(startVec[2]));
		float yy  = sqrt(abs(endVec[0])+abs(endVec[1])+abs(endVec[2]));
		angle = acos((angle/xx)/yy);*/
		/*
		float dot = startVec[0]*endVec[0] + startVec[1]*endVec[1] + startVec[2]*endVec[2];
		float lenSq1 = startVec[0]*startVec[0] + startVec[1]*startVec[1] + startVec[2]*startVec[2];
		float lenSq2 = endVec[0]*endVec[0] + endVec[1]*endVec[1] + endVec[2]*endVec[2];
		angle = acos(dot/sqrt(lenSq1 * lenSq2));

		fprintf( stderr, "Angle: %f\n", angle);
		fprintf( stderr, "Axis of Rotation: (%f, %f, %f)\n", axisOfRotation[0], axisOfRotation[1], axisOfRotation[2] );*/
	}
  
  bool render( ){
    Vec4 light0_position(10.0, 5.0, 10.0, 1.0);
    Vec4 light1_position(0.0, 14.0 ,0.0 ,1);
    const Vec4 light0_specular(0.6,0.2,0.0,1);
    const Vec4 light1_specular(0.0,0.2,0.6,1);
	const Vec4 purple_light(0.5,0.0,0.5,1);
	const Vec4 white_light(1,1,1,1);
    // specular on teapot
    const Vec4 one(1,1,1,1);
    // diffuse on teapot
    const Vec4 medium(0.5,0.5,0.5,1);
    // ambient on teapot
    const Vec4 small(0.2,0.2,0.2,1);
    // shininess of teapot
    const Vec1 high(100);


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Vec2i w = windowSize( );
    double ratio = double(w[0]) / double(w[1]);

    projectionMatrix = perspective(90.0, ratio, 1.0, 50.0);
    modelViewMatrix = lookat(eyePosition, centerPosition, upVector);
    normalMatrix = modelViewMatrix.inverse().transpose( );

    // Set light & material properties for the teapot;
    // lights are transformed by current modelview matrix
    // such that they are positioned correctly in the scene.
    light0 = modelViewMatrix * light0_position;
    light1 = modelViewMatrix * light1_position;
    
    glUniformMatrix4fv(uModelViewMatrix, 1, false, modelViewMatrix);
    glUniformMatrix4fv(uProjectionMatrix, 1, false, projectionMatrix);
    glUniformMatrix4fv(uNormalMatrix, 1, false, normalMatrix);
    glUniform4fv(uLight0_position, 1, light0); 
    glUniform4fv(uLight0_color, 1, light0_specular); 
    glUniform4fv(uLight1_position, 1, light1); 
    glUniform4fv(uLight1_color, 1, white_light); 

	updatePly();

	Vec3 tmpCenter = centerPosition;
	if(isKeyPressed('1')){
		centerPosition = Vec3(1, 20, 1);
		//drawSphere(tricoModel->radius, 32, 32);
		for(int p = 0; p <4; p++){
			//myPly[p]->draw();
			drawBB(Vec3(myPly[p]->center[0], myPly[p]->center[1], myPly[p]->center[2]), myPly[p]->radius);
			//drawSphere(myPly[0]->radius, 32, 32);
			//tricoModel->draw( );
			//tricoModel = readPlyModel("ply/shark.ply");
			if(isKeyPressed('2')){
				myPly[0]->translate(-0.1, 0, 0);
				//tricoModel = readPlyModel("ply/shark.ply");
				//tricoModel->translate();
				//drawSphere(tricoModel->radius, 32, 32);
				//tricoModel->translate();
			}
		}
		centerPosition = tmpCenter;
	}
    glUniform4fv(uAmbient, 1, medium); 
    glUniform4fv(uDiffuse, 1, medium); 
    glUniform4fv(uSpecular, 1, one); 
    glUniform1fv(uShininess, 1, high); 

//drawSphere(2, 32, 32);

//skybox
for(int x=0; x<5; x++)
{
	glBegin(GL_POLYGON);
		glVertex3fv( (float*)(mySquare[x].a) );
		glVertex3fv( (float*)(mySquare[x].b) );
		glVertex3fv( (float*)(mySquare[x].c) );
		glVertex3fv( (float*)(mySquare[x].d) );
	glEnd();
}
glUniform4fv(uLight0_color, 1, light1_specular);
//ground plane

glBegin(GL_POLYGON);
glColor3d( 1.0, 0.0, 0.0 );
glVertex3f(  100, -2, -100 );
glColor3d( 1.0, 0.0, 0.0 );
glVertex3f(  100, -2,  100 );
glColor3d( 1.0, 0.0, 0.0 );
glVertex3f( -100, -2,  100 );
glColor3d( 1.0, 0.0, 0.0 );
glVertex3f( -100, -2, -100 );
glEnd();

//pyramid
/*
	glUniform4fv(uLight0_color, 1, purple_light);
	for(int x=0; x<4; x++)
	{
		glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv( (float*)(myTri[x].a) );
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv( (float*)(myTri[x].b) );
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3fv( (float*)(myTri[x].c) );
		glColor3f(1.0f, 0.0f, 0.0f);
			glEnd( ); 
	}
*/   
/////////////////////////////TRACKBALL STATE////////////////////////////
	if (isKeyPressed(GLFW_KEY_LEFT_SHIFT)||isKeyPressed(GLFW_KEY_RIGHT_SHIFT))
	{
		Vec2 oldPosition = mousePosition;
		mousePosition = mouseCurrentPosition( );
		if(mouseButtonFlags( ) == GLFWApp::MOUSE_BUTTON_LEFT){		
			float angle_y  = 0.0f;				
			float angle_z  = 0.0f;							
			
			//Vec3 f = normalize(-eyePosition);//gaze
			//Vec3 _up = normalize(upVector);//up			
			//Vec3 rVec = normalize(cross(f, _up));//right

			// Get the direction from the mouse cursor
			angle_y = (float)( (oldPosition[0] - mousePosition[0]) ) / 500;		
			angle_z = (float)( (oldPosition[1] - mousePosition[1]) ) / 500;

			// Get the view vector
			Vec3 vVector = centerPosition - eyePosition;
	
			centerPosition[2] = (float)(eyePosition[2] + sin(-angle_y)*vVector[0] + cos(-angle_y)*vVector[2]);
			centerPosition[0] = (float)(eyePosition[0] + cos(-angle_y)*vVector[0] - sin(-angle_y)*vVector[2]);

			Mat3 yMat3(	cos(angle_z),	0,	sin(angle_z),	
					0,		1,	0,	
					-sin(angle_z),	0,	cos(angle_z));
			centerPosition = yMat3 * centerPosition;

			Mat3 xMat3(	1,	0,		0,
					0,	cos(-angle_z),	-sin(-angle_z),
					0,	sin(-angle_z),	cos(-angle_z));
			upVector = xMat3 * upVector;			
    		}
	}
		

    if(isKeyPressed('Q')){
      end( );      
    }else if(isKeyPressed(GLFW_KEY_EQUAL)){
      rotationDelta += 1.0;
      printf( "Rotation delta set to %g\n", rotationDelta );
    }else if(isKeyPressed(GLFW_KEY_MINUS)){
      rotationDelta -= 1.0;
      printf( "Rotation delta set to %g\n", rotationDelta );
    }else if(isKeyPressed('R')){
      initEyePosition( );
      initUpVector( );
      initRotationDelta( );
      printf("Eye position, up vector and rotation delta reset.\n");
    }else if(isKeyPressed('A')){
	for(int x=0; x<4; x++)
	{
      		myTri[x].a[0]+=0.1;
      		myTri[x].b[0]+=0.1;
      		myTri[x].c[0]+=0.1;
	}
    }else if(isKeyPressed(GLFW_KEY_LEFT)){
      rotateCameraLeft(-rotationDelta, eyePosition,
                       centerPosition, upVector);
    }else if(isKeyPressed(GLFW_KEY_RIGHT)){
      rotateCameraLeft(rotationDelta, eyePosition,
                       centerPosition, upVector);
    }else if(isKeyPressed(GLFW_KEY_UP)){
      rotateCameraUp(-rotationDelta, eyePosition,
                     centerPosition, upVector);
    }else if(isKeyPressed(GLFW_KEY_DOWN)){
      rotateCameraUp(rotationDelta, eyePosition,
                     centerPosition, upVector);
    }
    return !msglError( );
  }
   
};


int main(int argc, char* argv[]){
	//glfwInit(&argc,argv);
	//glfwInitDisplayMode(GLFW_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  PickingApp app(argc, argv);
  return app();
}

