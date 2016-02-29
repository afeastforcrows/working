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
#include "binary-search-tree/Tree.h"

const int numObj = 5;


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

typedef struct quaternion{
	float x,y,z,w;
	
	quaternion mult(quaternion A, quaternion B){
  		quaternion C;

  		C.x = A.w*B.x + A.x*B.w + A.y*B.z - A.z*B.y;
  		C.y = A.w*B.y - A.x*B.z + A.y*B.w + A.z*B.x;
  		C.z = A.w*B.z + A.x*B.y - A.y*B.x + A.z*B.w;
  		C.w = A.w*B.w - A.x*B.x - A.y*B.y - A.z*B.z;
  		return C;
	}

	quaternion multi(quaternion B){
  		quaternion C;

  		C.x = w*B.x + x*B.w + y*B.z - z*B.y;
  		C.y = w*B.y - x*B.z + y*B.w + z*B.x;
  		C.z = w*B.z + x*B.y - y*B.x + z*B.w;
  		C.w = w*B.w - x*B.x - y*B.y - z*B.z;
  		return C;
	}

	void conjugate(){
  		x = -x;
  		y = -y;
  		z = -z;
	}

	

}quaternion;

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

typedef struct bBox{
	Vec3 center;
	float width;
	Vec4 vertices[8];
	
	void align(){
		//
	}

	Vec3 frontNorm(){
		Vec3 edge1, edge2;
		edge1 = Vec3(vertices[4][0],vertices[4][1],vertices[4][2])-Vec3(vertices[7][0],vertices[7][1],vertices[7][2]);
		edge2 = Vec3(vertices[6][0],vertices[6][1],vertices[6][2])-Vec3(vertices[7][0],vertices[7][1],vertices[7][2]);
		//Vec3 temp = normalize(cross(edge1,edge2));
		//fprintf( stderr, "Cross: %f, %f, %f\n", temp[0], temp[1], temp[2]);
		return normalize(cross(edge1,edge2));
	}

	Vec3 topNorm(){
		Vec3 edge1, edge2;
		edge1 = Vec3(vertices[2][0],vertices[2][1],vertices[2][2])-Vec3(vertices[7][0],vertices[7][1],vertices[7][2]);
		edge2 = Vec3(vertices[3][0],vertices[3][1],vertices[3][2])-Vec3(vertices[7][0],vertices[7][1],vertices[7][2]);
		//Vec3 temp = cross(edge1,edge2);
		//fprintf( stderr, "Cross: %f, %f, %f\n", temp[0], temp[1], temp[2]);
		return normalize(cross(edge1,edge2));
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

	void update(Vec3 c, float w){
		center = c;
		width = w;
	}

	void drawBB(){//Vec3 center, float width){
		vertices[0] = Vec4(	-0.5,	-0.5,	-0.5,	1.0);
		vertices[1] = Vec4(	0.5,	-0.5,	-0.5,	1.0);
		vertices[2] = Vec4(	0.5,	0.5,	-0.5,	1.0);
		vertices[3] = Vec4(	-0.5,	0.5,	-0.5,	1.0);
		vertices[4] = Vec4(	-0.5,	-0.5,	0.5,	1.0);
		vertices[5] = Vec4(	0.5,	-0.5,	0.5,	1.0);
		vertices[6] = Vec4(	0.5,	0.5,	0.5,	1.0);
		vertices[7] = Vec4(	-0.5,	0.5,	0.5,	1.0);
		glLineWidth(2.5);
		for(int i = 0; i<8; i++){
			vertices[i] = scale4(vertices[i], width*2);
		}
		for(int i = 0; i<8; i++){
			vertices[i] = translate4(vertices[i], center[0], center[1], center[2], 1);
		}
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glBegin(GL_QUADS);      // draw a cube with 6 quads
    		glVertex3fv(vertices[0]);	//back
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

    		glVertex3fv(vertices[2]);	//top
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
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	}
}bBoxd;

typedef struct sceneObj{
	//basic data structure to act as objects in scene graph
	public:
	std::string name;
	sceneObj *parent;
	sceneObj *children[5];
	int numChildren;
	bBox BB;
	FaceList *FL = readPlyModel("ply/trico.ply");

	void init(std::string n, bBox bb, FaceList *fl){
		name = n;
		parent = NULL;
		numChildren = 0;
		BB = bb;
		FL = fl;
	}

	void addParent(sceneObj *p){
		parent = p;
	}

	void addChild(sceneObj *c){
		for(int n = 0; n < numChildren; n++)
		{
			if(children[n] == c){
				return;
			}
		}
		children[numChildren] = c;
		numChildren++;
	}

	void removeChild(sceneObj *c){
		children[numChildren] = NULL;
		numChildren--;
	}

	bool isRoot(){
		return (parent==NULL);
	}
	
	bool isLeaf(){
		return (numChildren==0);
	}
	
}sceneObjd;

typedef struct sceneGraph{
	//checkpoint
	//basic data structure to act as scene graph
	sceneObj *root;
	sceneObj myObjs[numObj];
	bBox worldBB;

	int showBB; //keep track of which bounding volume to show
	bool boolBB; //A switch used when toggling bounding volumes on/off
	bool hitFlag; //keep track of weather the pick() hit a model

	void init(){
		showBB = -1; //keep track of which bounding volume to show
		boolBB = true;
		hitFlag = false; //keep track of whether the pick() hit a model
		//myObjs[0] is the world
		myObjs[1].FL = readPlyModel("ply/trico.ply");
		myObjs[1].FL->translate(-2.0, 0.0, 0.0);
		myObjs[1].FL->scale(2);
		//calcRitterBoundingSphere(myObjs[0]->center, &(myObjs[0]->radius), myObjs[0]);
		myObjs[2].FL = readPlyModel("ply/spider.ply");
		myObjs[2].FL->translate(2.0, 0.0, 2.0);
		myObjs[3].FL = readPlyModel("ply/shark.ply");
		myObjs[3].FL->translate(2.0, 4.0, 2.0);
		myObjs[4].FL = readPlyModel("ply/urn.ply");
		myObjs[4].FL->translate(-2.0, 4.0, 2.0);
		myObjs[5].FL = readPlyModel("ply/urn.ply");
		myObjs[5].FL->translate(-2.0, -20.0, 2.0);
		std::string myNames[6]={"World", "Trico", "Spider", "Shark", "Urn", "Test"};
		for(int i = 0; i < numObj; i++){
			calcRitterBoundingSphere(myObjs[i].FL->center, &(myObjs[i].FL->radius), myObjs[i].FL);
			myObjs[i].BB.update(Vec3(myObjs[i].FL->center[0], myObjs[i].FL->center[1], myObjs[i].FL->center[2]), myObjs[i].FL->radius);
		}

		worldBB.update(Vec3(0,0,0), 10);
		myObjs[0].init("World", worldBB, NULL);	//scene object that represents the world
		for(int n = 1; n < numObj; n++){			//All objects begin as children of World
			myObjs[n].init(myNames[n], myObjs[n].BB, myObjs[n].FL);
			myObjs[0].addChild(&myObjs[n]);
			myObjs[n].addParent(&myObjs[0]);
		}
	}

	void updatePly(){
		for(int p = 1; p<numObj; p++){
			calcRitterBoundingSphere(myObjs[p].FL->center, &(myObjs[p].FL->radius), myObjs[p].FL);
		}
	}

	bool cullIt(Vec3 centerPosition, Vec3 eyePosition, Vec3 upVector, Vec3 midPoint){		
		Vec3 planes[4];
		planes[0] = Vec3(0,1,1);
		planes[1] = Vec3(-1,0,1);
		planes[2] = Vec3(1,0,1);
		planes[3] = Vec3(0,-1,1);
		Vec3 gaze = normalize(centerPosition-eyePosition);
		Vec3 right = cross(gaze,upVector);
		float angle = acos(sqrt(gaze[0]*gaze[0]+gaze[2]*gaze[2])/length(gaze));
		Vec3 testVec;
		
		/*for(int x=0; x<4; x++){
			float total = midPoint[0]*planes[x][0] + midPoint[1]*planes[x][1] + midPoint[2]*planes[x][2];
			if(total>0){
				return false;
			}
		}*/

		

		
		glLineWidth(5);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
		glVertex3f(centerPosition[0], centerPosition[1], centerPosition[2]);
		glVertex3f(planes[0][0], planes[0][1], planes[0][2]);
		glEnd();

		
		//Vec3 planes[4];
		/*
		planes[0] = Vec3(0,1,1).rotate(angle, right[0], right[1], right[2]);
		planes[1] = Vec3(-1,0,1).rotate(angle, right[0], right[1], right[2]);
		planes[2] = Vec3(1,0,1).rotate(angle, right[0], right[1], right[2]);
		planes[3] = Vec3(0,-1,1).rotate(angle, right[0], right[1], right[2]);
		*/
		/*
		planes[0] = normalize(Vec3(centerPosition[0]*gaze[0],(centerPosition[1]+1)*gaze[1],(centerPosition[2]+1)*gaze[2]));
		planes[1] = normalize(Vec3((centerPosition[0]-1)*gaze[0],centerPosition[1]*gaze[1],(centerPosition[2]+1)*gaze[2]));
		planes[2] = normalize(Vec3((centerPosition[0]+1)*gaze[0],centerPosition[1]*gaze[1],(centerPosition[2]+1)*gaze[2]));
		planes[3] = normalize(Vec3(centerPosition[0]*gaze[0],(centerPosition[1]-1)*gaze[1],(centerPosition[2]+1)*gaze[2]));
		*/

		for(int x=0; x<4; x++){
			testVec = normalize(midPoint);
			testVec -= centerPosition;
			
				//printf("Dot %d: %f \n", x, dot(testVec,planes[x]));
			
			if((dot(testVec,planes[x]))>0){
				//printf("boop: (%f, %f, %f) - %d \n", centerPosition[0], centerPosition[1], centerPosition[2], x);			
				//return false;
			}
		}

		/*		
		Vec3 myNorms[4];
		myNorms[0] = normalize(Vec3(centerPosition[0],centerPosition[0],centerPosition[0])-Vec3(0,sqrt(1),sqrt(1)));
		myNorms[1] = normalize(Vec3(centerPosition[0],centerPosition[0],centerPosition[0])-Vec3(-sqrt(1),0,sqrt(1)));
		myNorms[2] = normalize(Vec3(centerPosition[0],centerPosition[0],centerPosition[0])-Vec3(sqrt(1),0,sqrt(1)));
		myNorms[3] = normalize(Vec3(centerPosition[0],centerPosition[0],centerPosition[0])-Vec3(0,-sqrt(1),sqrt(1)));
		Vec3 planes[4];
		planes[0] = Vec3(0,sqrt(1),sqrt(1));
		planes[1] = Vec3(-sqrt(1),0,sqrt(1));
		planes[2] = Vec3(sqrt(1),0,sqrt(1));
		planes[3] = Vec3(0,-sqrt(1),sqrt(1));
		Vec3 testVec;
			glLineWidth(5);
			glColor3f(1.0, 0.0, 0.0);
			glBegin(GL_LINES);
			glVertex3f(centerPosition[0], centerPosition[1], centerPosition[2]);
			glVertex3f(midPoint[0], midPoint[1], midPoint[2]);
			glEnd();
		for(int x=0; x<4; x++){
			testVec = normalize(midPoint);
			testVec -= planes[x];
			
				printf("Dot %d: %f \n", x, dot(testVec,myNorms[x]));
			
			if((dot(testVec,myNorms[x]))<0.5){
				printf("boop: (%f, %f, %f) - %d \n", centerPosition[0], centerPosition[1], centerPosition[2], x);			
				return false;
			}
		}*/
		return true;
	}

	void draw(Vec3 centerPosition, Vec3 eyePosition, Vec3 upVector){
		for(int p = 1; p <numObj; p++){
			if(cullIt(normalize(centerPosition), eyePosition, upVector, Vec3(myObjs[p].FL->center[0], myObjs[p].FL->center[1], myObjs[p].FL->center[2])))
			{
				myObjs[p].FL->draw();//call FaceList draw() function
				//myObjs[p].BB.drawBB();
			}
			myObjs[p].BB.update(Vec3(myObjs[p].FL->center[0], myObjs[p].FL->center[1], myObjs[p].FL->center[2]), myObjs[p].FL->radius);
		}
	}

	void update(Vec3 centerPosition, Vec3 eyePosition, Vec3 upVector){
		//float dist;
		//check for collisions
		/*for(int x = 0; x < numObj; x++){
			for(int y = x+1; y < numObj; y++){
				dist = distance(myObjs[x].BB.center, myObjs[y].BB.center);
				if(dist<=(float(myObjs[x].BB.width+ myObjs[y].BB.width))){
					//printf("COLLISION with %s and %s -- %f\n", myObjs[x].name.c_str(), myObjs[y].name.c_str(), dist );
					if(myObjs[x].BB.width > myObjs[y].BB.width){
						//myObjs[x].BB is larger and will become the parent
						//but first we have to make sure the smaller obj doesn't already have a parent
					}
					else{
						//myObjs[y].BB is larger and will become the parent
						//but first we have to make sure the smaller obj doesn't already have a parent
					}
				}
			}
		}*/
		//update parents/children
		updatePly();
		draw(centerPosition, eyePosition, upVector);
	}

	float distance(Vec3 a, Vec3 b){
		return sqrt(	( abs(a[0]-b[0]) ) *( abs(a[0]-b[0]) )+
				( abs(a[1]-b[1]) ) *( abs(a[1]-b[1]) )+
				( abs(a[2]-b[2]) ) *( abs(a[2]-b[2]) ));
	}

	void testPar(){
		//myObjs[1].addChild(&myObjs[2]);
	}

	void translate(sceneObj *s, float n){
		s->FL->translate(0,n,0);
		for(int n = 0; n < s->numChildren; n++){
			s->children[n]->FL->translate(0,n,0);
		}
	}

	void drawSphere(float radius, int slices, int stacks, double x, double y, double z){
		GLUquadricObj *quadObj;
		quadObj = gluNewQuadric();
		//assert(quadObj);
		
		//gluQuadricDrawStyle(quadObj, GLU_FILL);
  		//gluQuadricNormals(quadObj, GLU_SMOOTH);
  		
  		gluQuadricDrawStyle(quadObj, GLU_LINE);
  		//gluQuadricNormals(quadObj, GLU_SMOOTH);

		glTranslatef(.05f,0.1f,z);
  		gluSphere(quadObj, radius, slices, stacks);

		

	}

}sceneGraphd;

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
	
  triangle tri;
  triangle tri3;
  triangle tri4;
	square mySquare[5];
	triangle myTri[4];
	
	sceneGraph myGraph;
  
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
    centerPosition = Vec3(0.0, 0.0, 1.0);
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
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
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
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	}

	
	

	void initMySquare(){
		float distance = 7;
		//1
		mySquare[0].a[0]=-distance;
		mySquare[0].a[1]=-2;
		mySquare[0].a[2]=distance;

		mySquare[0].b[0]=-distance;
		mySquare[0].b[1]=distance;
		mySquare[0].b[2]=distance;

		mySquare[0].c[0]=distance;
		mySquare[0].c[1]=distance;
		mySquare[0].c[2]=distance;

		mySquare[0].d[0]=distance;
		mySquare[0].d[1]=-2;
		mySquare[0].d[2]=distance;
		//2////////////////////////////////////////////////////
		mySquare[1].a[0]=distance;
		mySquare[1].a[1]=-2;
		mySquare[1].a[2]=distance;

		mySquare[1].b[0]=distance;
		mySquare[1].b[1]=distance;
		mySquare[1].b[2]=distance;

		mySquare[1].c[0]=distance;
		mySquare[1].c[1]=distance;
		mySquare[1].c[2]=-distance;

		mySquare[1].d[0]=distance;
		mySquare[1].d[1]=-2;
		mySquare[1].d[2]=-distance;
		//3////////////////////////////////////////////////////////
		mySquare[2].a[0]=distance;
		mySquare[2].a[1]=-2;
		mySquare[2].a[2]=-distance;

		mySquare[2].b[0]=distance;
		mySquare[2].b[1]=distance;
		mySquare[2].b[2]=-distance;

		mySquare[2].c[0]=-distance;
		mySquare[2].c[1]=distance;
		mySquare[2].c[2]=-distance;

		mySquare[2].d[0]=-distance;
		mySquare[2].d[1]=-2;
		mySquare[2].d[2]=-distance;
		//4
		mySquare[3].a[0]=-distance;
		mySquare[3].a[1]=-2;
		mySquare[3].a[2]=-distance;

		mySquare[3].b[0]=-distance;
		mySquare[3].b[1]=distance;
		mySquare[3].b[2]=-distance;

		mySquare[3].c[0]=-distance;
		mySquare[3].c[1]=distance;
		mySquare[3].c[2]=distance;

		mySquare[3].d[0]=-distance;
		mySquare[3].d[1]=-2;
		mySquare[3].d[2]=distance;
		//5///////////////////////////
		mySquare[4].a[0]=-distance;
		mySquare[4].a[1]=distance;
		mySquare[4].a[2]=-distance;

		mySquare[4].b[0]=-distance;
		mySquare[4].b[1]=distance;
		mySquare[4].b[2]=distance;

		mySquare[4].c[0]=distance;
		mySquare[4].c[1]=distance;
		mySquare[4].c[2]=distance;

		mySquare[4].d[0]=distance;
		mySquare[4].d[1]=distance;
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

 
  bool begin( ){
    msglError( );
	initMyTri();
	initMySquare();
    initCenterPosition( );
    initEyePosition( );
    initUpVector( );
    initRotationDelta( );
	myGraph.init();




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
	
	glEnable(GL_LIGHT0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    msglVersion( );
    
    return !msglError( );
  }
  
  bool end( ){
    windowShouldClose( );
    return true;
  }
   
	

	bool pick(int x, int y, FaceList *fl){
		bool result = false;
    		float center[3];
		for(int i = 0; i < 3; i++){
			center[i] = fl->center[i];
		}
		Point3 cc(center);
		BoundingSphere boundingSphere(cc, fl->radius);
		GLViewPort vp;
		    /*******
		     * With the double unproject technique
		     */
		    // origin at the lower left corner; flip the y
		int flipped_y = vp.height( ) - y - 1;
		    
		Vec3 nearWinCoord(x, flipped_y, 0);
		Vec3 farWinCoord(x, flipped_y, 1);
		Vec3 nearObjCoord, farObjCoord;
		/*int viewport[4];
		glGetIntegerv( GL_VIEWPORT, viewport ); 
		double mv[16], proj[16];
		for(int i = 0; i < 4; i++){
		      for(int j = 0; j < 4; j++){
			mv[(i*4)+j] = modelViewMatrix(i, j);
			proj[(i*4)+j] = projectionMatrix(i, j);
		      }
		    }
		    double a, b, c, d, e, f;*/
		    //_msUnProject(x, y, 0.0, mv, proj, viewport, &a, &b, &c);
		if(!unproject(nearWinCoord, projectionMatrix, modelViewMatrix, vp, nearObjCoord)){
			std::cerr << "Something is wrong, the omega of the unprojected winCoord is zero." << std::endl;
			assert(false);
		}
		    //_msUnProject(x, y, 1.0, mv, proj, viewport, &d, &e, &f);
		    if(!unproject(farWinCoord, projectionMatrix, modelViewMatrix, vp, farObjCoord)){
		      std::cerr << "Something is wrong, the omega of the unprojected winCoord is zero." << std::endl;
		      assert(false);
		    }
		    /*std::cerr << nearWinCoord << " " << nearObjCoord << " " << a << " " << b << " " << c << std::endl;
		    std::cerr << farWinCoord << " " << farObjCoord << " " << d << " " << e << " " << f << std::endl;*/
		    Vec3 direction1 = farObjCoord - nearObjCoord;
		    Point3 rayOrigin1(eyePosition);
		    Ray r1(rayOrigin1, direction1);
		    if( boundingSphere.intersectWith(r1) ){
		      //std::cerr << "Intersection" << std::endl;
			result = true;
		    }else{
		      //std::cerr << "No intersection" << std::endl;
		    }
		    /*
		     * End double unproject technique
		     ******/
		    
		    /******
		     * Ray in eye coordinates
		     */
		    int window_y = (vp.height( ) - y) - vp.height( )/2;
		    double norm_y = double(window_y)/double(vp.height( )/2);
		    int window_x = x - vp.width( )/2;
		    double norm_x = double(window_x)/double(vp.width( )/2);
		    double near_height = atan(degreesToRadians(25.0));
		    float _y = near_height * norm_y;
		    float _x = near_height * vp.aspect( ) * norm_x;
		    
		    Mat4 modelViewInverse = modelViewMatrix.inverse( );
		    Vec4 ray_origin(0.f, 0.f, 0.f, 1.f);
		    // near distance (3rd param) is from the perspective call,
		    // it should really come from a camera object.
		    Vec4 ray_direction(_x, _y, -1.f, 0.f);
		    ray_origin = modelViewInverse * ray_origin;
		    ray_direction = modelViewInverse * ray_direction;
		    
		    //std::cerr << ray_origin << std::endl;
		    //std::cerr << ray_direction << std::endl;
		    Point3 rayOrigin2 = Point3(ray_origin);
		    Vec3 direction2 = ray_direction.xyz( );
		    Ray r2(rayOrigin2, direction2);
		    //std::cerr << r2 << std::endl;
		    if( boundingSphere.intersectWith(r2) ){
		      //std::cerr << "Intersection again" << std::endl;
		    }else{
		      //std::cerr << "No intersection again" << std::endl;
		    }
		    
		   /*
		    * End ray in eye coordinates
		    ******/
		return result;
  }

	

	void drawFrustum(int x, int y, Vec3 center){
		GLViewPort vp;
		/*******
		* With the double unproject technique
		*/
		// origin at the lower left corner; flip the y
		int flipped_y = vp.height( ) - y - 1;
		Vec3 nearWinCoord(x, flipped_y, 0);
		Vec3 farWinCoord(x, flipped_y, 1);
		Vec3 nearObjCoord, farObjCoord;
		//_msUnProject(x, y, 0.0, mv, proj, viewport, &a, &b, &c);
		if(!unproject(nearWinCoord, projectionMatrix, modelViewMatrix, vp, nearObjCoord)){
			std::cerr << "Something is wrong, the omega of the unprojected winCoord is zero." << std::endl;
			assert(false);
		}
		//_msUnProject(x, y, 1.0, mv, proj, viewport, &d, &e, &f);
		if(!unproject(farWinCoord, projectionMatrix, modelViewMatrix, vp, farObjCoord)){
			std::cerr << "Something is wrong, the omega of the unprojected winCoord is zero." << std::endl;
			assert(false);
		}
		Vec3 direction1 = farObjCoord - nearObjCoord;
		Point3 rayOrigin1(eyePosition);
		Ray r1(rayOrigin1, direction1);
		/* End double unproject technique
		Ray in eye coordinates */
		int window_y = (vp.height( ) - y) - vp.height( )/2;
		double norm_y = double(window_y)/double(vp.height( )/2);
		int window_x = x - vp.width( )/2;
		double norm_x = double(window_x)/double(vp.width( )/2);
		double near_height = atan(degreesToRadians(25.0));
		float _y = near_height * norm_y;
		float _x = near_height * vp.aspect( ) * norm_x;
		Mat4 modelViewInverse = modelViewMatrix.inverse( );
		//Vec4 ray_origin(0.f, 0.f, 0.f, 1.f);
		Vec4 ray_origin(0.f, 0.f, 0.f, 1.f);
		// near distance (3rd param) is from the perspective call,
		// it should really come from a camera object.
		Vec4 ray_direction(_x, _y, -1.f, 0.f);
		//Vec4 ray_direction(1, 1, -1.f, 0.f);
		ray_origin = modelViewInverse * ray_origin;
		ray_direction = modelViewInverse * ray_direction;
		Vec4 p1 = modelViewInverse * Vec4(6,6,0,0);
		Vec4 p2 = modelViewInverse * Vec4(0,0,1,0);
		//std::cerr << ray_origin << std::endl;
		//std::cerr << ray_direction << std::endl;
		Point3 rayOrigin2 = Point3(ray_origin);
		Vec3 direction2 = ray_direction.xyz( );
		Ray r2(rayOrigin2, direction2);
		glLineWidth(5);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
		glVertex3f(p1[0], p1[1], p1[2]);
		glVertex3f(p2[0], p2[1], p2[2]);
		glEnd();
		//std::cerr << r2 << std::endl;
		/*
		* End ray in eye coordinates
		******/
	}	
  
  bool render( ){
	const Vec4 light0_position(14.0, 19.0, 14.0, 1.0);
	const Vec4 light1_position(-14.0, 14.0 ,-14.0 ,1);
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
	glColor3f(0.5f, 0.0f, 1.0f);	
	if(isKeyPressed('2')){
		myGraph.testPar();
		myGraph.translate(&myGraph.myObjs[3],-0.1);
	}
	if(isKeyPressed('3')){
		myGraph.testPar();
		myGraph.translate(&myGraph.myObjs[3],0.1);
	}
	if(isKeyPressed('4')){
		for(int p = 1; p < numObj; p++){
			if(abs(dot(myGraph.myObjs[p].BB.frontNorm(),normalize(Vec3(myGraph.myObjs[p].FL->center[0], myGraph.myObjs[p].FL->center[1], myGraph.myObjs[p].FL->center[2])-eyePosition)))==1){
				fprintf(stderr, "GOOD! %d\n", p);
				//awesome, every box is axis aligned
			}
			else{
				fprintf(stderr, "BAD! %d\n", p);//%f\n", dot(myBoxes[0].frontNorm(),normalize(eyePosition)));
				//no bueno, box is not axis aligned.
			}
		}	
	}
	if(myGraph.showBB>=0 && myGraph.boolBB == true){
		myGraph.myObjs[myGraph.showBB].BB.drawBB(); //render the bounding box of the model hit by pick()
		//myGraph.drawSphere(myGraph.myObjs[myGraph.showBB].FL->radius, 10, 10, myGraph.myObjs[myGraph.showBB].FL->center[0],myGraph.myObjs[myGraph.showBB].FL->center[1], myGraph.myObjs[myGraph.showBB].FL->center[2]);
		GLUquadricObj *quadObj;
		quadObj = gluNewQuadric();
		//assert(quadObj);
		
		//gluQuadricDrawStyle(quadObj, GLU_FILL);
  		//gluQuadricNormals(quadObj, GLU_SMOOTH);
  		
  		gluQuadricDrawStyle(quadObj, GLU_LINE);
  		//gluQuadricNormals(quadObj, GLU_SMOOTH);

		glTranslatef(myGraph.myObjs[myGraph.showBB].FL->center[0],myGraph.myObjs[myGraph.showBB].FL->center[1], myGraph.myObjs[myGraph.showBB].FL->center[2]);
  		gluSphere(quadObj, myGraph.myObjs[myGraph.showBB].FL->radius, 8, 8);
	}	
	
    glUniform4fv(uAmbient, 1, medium); 
    glUniform4fv(uDiffuse, 1, medium); 
    glUniform4fv(uSpecular, 1, one); 
    glUniform1fv(uShininess, 1, high); 

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

/////////////////////////////TRACKBALL STATE////////////////////////////
	//if (isKeyPressed(GLFW_KEY_LEFT_SHIFT)||isKeyPressed(GLFW_KEY_RIGHT_SHIFT))
	//{
		Vec2 oldPosition = (mousePosition);
		mousePosition = (mouseCurrentPosition( ));
		mousePosition[0] = ((mousePosition[0]*2)/500)-1;
		mousePosition[1] = ((mousePosition[1]*2)/500)-1;

		if(mouseButtonFlags( ) == GLFWApp::MOUSE_BUTTON_LEFT){	
			if(true)//abs(mousePosition[0])<1 && abs(mousePosition[1])<1)//only proceed if the cursor is still in the window
			{
				//Vec3 startVec = (centerPosition-Vec3(oldPosition[0], oldPosition[1], 0));
				//Vec3 endVec = (centerPosition-Vec3(mousePosition[0], mousePosition[1], 0));
				//Vec3 axisQ = normalize(cross(startVec, endVec));
				//float angleQ = dot(startVec, endVec);
				Vec3 right =  normalize(cross(centerPosition-eyePosition, upVector));

				if(oldPosition[0] != mousePosition[0] && oldPosition[1] != mousePosition[1])//only proceed if there was actual movement of the mouse
				{
						quatRotate(centerPosition, upVector[0],upVector[1],upVector[2],(mousePosition[0] - oldPosition[0]));
						upVector = normalize(cross(right, centerPosition-eyePosition));
						quatRotate(centerPosition, right[0],right[1],right[2],(mousePosition[1] - oldPosition[1]));
				}	
			}	
    		}
	//}
		
	
    if(isKeyPressed('Q')){
      end( );   
	}else if(isKeyPressed('H')){
		printf( "This is a help message. More to come soon." );
	   
	}else if(isKeyPressed('B')){
		myGraph.boolBB = !myGraph.boolBB;
	  
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
	if(mouseButtonFlags( ) == GLFWApp::MOUSE_BUTTON_LEFT){
		//printf("mouse left button\n");
		Vec2 mousePosition = mouseCurrentPosition( );
		//std::cout << "Mouse position: " << mousePosition << std::endl;
		for(int x=1; x<numObj; x++){
			if(pick(mousePosition[0], mousePosition[1], myGraph.myObjs[x].FL)){
				printf("Intersect with %s!\n", myGraph.myObjs[x].name.c_str());
				myGraph.showBB = x;
				break;
			}
			else{
				myGraph.showBB = -1;
			}
		}
	}
	myGraph.update(centerPosition, eyePosition, eyePosition);
	return !msglError( );
   }
};


int main(int argc, char* argv[]){
	//glfwInit(&argc,argv);
	//glfwInitDisplayMode(GLFW_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  PickingApp app(argc, argv);
  return app();
}


