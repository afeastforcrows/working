////////////////////////////////////////////////////////////////////////////////
//
// OpenTissue, A toolbox for physical based simulation and animation.
// Copyright (C) 2003 Department of Computer Science, University of Copenhagen
//
// This file is part of OpenTissue.
//
// OpenTissue is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// any later version.
//
// OpenTissue is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenTissue; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Please send remarks, questions and bug reports to OpenTissue@diku.dk,
// or write to:
//
//    Att: Kenny Erleben and Jon Sporring
//    Department of Computing Science, University of Copenhagen
//    Universitetsparken 1
//    DK-2100 Copenhagen
//    Denmark
//
////////////////////////////////////////////////////////////////////////////////
#if(_MSC_VER >= 1200)
# pragma warning(default: 56 61 62 191 263 264 265 287 289 296 347 529 686)
#endif

#include "gl_util.h"

#include <iostream>
#include <iomanip>

#include <cmath>
using namespace std;

#include <OpenTissue/trackball/trackball_generic.h>
#include <OpenTissue/trackball/trackball_chen.h>
#include <OpenTissue/trackball/trackball_bell.h>
#include <OpenTissue/trackball/trackball_shoemake.h>
#include <OpenTissue/trackball/trackball.h>

#include <OpenTissue/math/constants.h>

#define TWOSIDED 1

using namespace OpenTissue;

/*******************************************************************\
*                                                                   *
*                  G l o b a l   V a r i a b l e s                  *
*                                                                   *
\*******************************************************************/

int       Width  = 800;
int       Height = 800;

int Menu;

GLdouble  Xmin   = -1.0;
GLdouble  Xmax   =  1.0;
GLdouble  Ymin   = -1.0;
GLdouble  Ymax   =  1.0;
GLdouble  Zmin   = -1.0;
GLdouble  Zmax   =  1.0;


typedef double real_type;

real_type      Radius  = 0.8;
real_type      Xcenter = 0.0;
real_type      Ycenter = 0.0;
real_type      Zcenter = 0.0;

GenericTrackball<real_type> * TB = new Trackball<real_type>(Radius);


typedef GenericTrackball<real_type>::vector3_type     vector3_type;
typedef GenericTrackball<real_type>::matrix3x3_type    matrix3x3_type;
typedef GenericTrackball<real_type>::quaternion_type  quaternion_type;



/*******************************************************************\
*                                                                   *
*                   S t a t e   V a r i a b l e s                   *
*                                                                   *
\*******************************************************************/

short     active        = 0;     // TRUE if window is attached
int       keysDown      = 0;
int       CapsLock      = 0;

int       ShowTrackBall       = 1;
int       ShowCoordinateFrame = 1;

int       ShowRotationAxis    = 1;
int       ShowRotationPlane   = 1;

/*******************************************************************\
*                                                                   *
*              N o r m a l i z e  C o o r d i n a t e s             *
*                                                                   *
\*******************************************************************/

void NormalizeCoordinates(real_type &  x, real_type &  y)
{
  // std::cout << "(x, y) before = (" << x << ", " << y << ")" << std::endl;

  x = 2.0 * x / Width  - 1.0;
  if (x < -1.0) x = -1.0;
  if (x >  1.0) x =  1.0;

  y = -(2.0 * y / Height - 1.0);
  if (y < -1.0) y = -1.0;
  if (y >  1.0) y =  1.0;

  // std::cout << "(x, y) after  = (" << x << ", " << y << ")" << std::endl;
}


void DisplayCoordinateFrame(const real_type Xmin, const real_type Xmax,
                            const real_type Ymin, const real_type Ymax,
                            const real_type Zmin, const real_type Zmax)
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glEnable(GL_LIGHTING);
#if 0
  glBindTexture(GL_TEXTURE_2D, 0);
#endif
  glLineWidth(3.0);

  int    ArrowFacets = 8;
  double ArrowWidth  = 0.03;
  double ArrowHeight = 0.10;
  double AxisLength  = 100.0;

  GLfloat frontKa = 0.0;
  GLfloat frontKd = 1.0;
  GLfloat frontKs = 1.0;
  GLfloat frontN  = 128.0;
  GLfloat frontColor[] = { 1.0, 0.0, 0.0, 1.0 };

  GLfloat frontAmbient[]   = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat frontDiffuse[]   = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat frontSpecular[]  = { 1.0, 1.0, 1.0, 1.0 };
  //GLfloat frontShinyness[] = { frontN };

  // Generata the X-axis

  static GLuint Xaxis = 0;
  if (Xaxis == 0){
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    Xaxis = glGenLists(1);
    glNewList(Xaxis, GL_COMPILE);
    glBegin(GL_LINES);
    glVertex3d(Xmin, 0.0, 0.0);
    glVertex3d(Xmax, 0.0, 0.0);
    glEnd();
    AxisLength = (Xmax - Xmin) / 2.0;
    glTranslated((1.0 - ArrowHeight) * AxisLength, 0.0, 0.0);
    glRotated(90.0, 0.0, 1.0, 0.0);
    glScaled(AxisLength, AxisLength, AxisLength);
    glutSolidCone(ArrowWidth, ArrowHeight, ArrowFacets, 1);
    glEndList();
    glPopMatrix();
  }

  // Generate the Y-axis

  static GLuint Yaxis = 0;
  if (Yaxis == 0) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    Yaxis = glGenLists(1);
    glNewList(Yaxis, GL_COMPILE);
    AxisLength = (Ymax - Ymin) / 2.0;
    glBegin(GL_LINES);
    glVertex3d(0.0, Ymin, 0.0);
    glVertex3d(0.0, Ymax, 0.0);
    glEnd();
    glTranslated(0.0, (1.0 - ArrowHeight) * AxisLength, 0.0);
    glRotated(-90.0, 1.0, 0.0, 0.0);
    glScaled(AxisLength, AxisLength, AxisLength);
    glutSolidCone(ArrowWidth, ArrowHeight, ArrowFacets, 1);
    glEndList();
    glPopMatrix();
  }

  // Generate the Z-axis

  static GLuint Zaxis = 0;
  if (Zaxis == 0) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    Zaxis = glGenLists(1);
    glNewList(Zaxis, GL_COMPILE);
    AxisLength = (Zmax - Zmin) / 2.0;
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, Zmin);
    glVertex3d(0.0, 0.0, Zmax);
    glEnd();
    glTranslated(0.0, 0.0, (1.0 - ArrowHeight) * AxisLength);
    glScaled(AxisLength, AxisLength, AxisLength);
    glutSolidCone(ArrowWidth, ArrowHeight, ArrowFacets, 1);
    glEndList();
    glPopMatrix();
  }

  // Display the axes

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  frontColor[0] = 1.0;
  frontColor[1] = 0.0;
  frontColor[2] = 0.0;
  frontColor[3] = 1.0;

  for (unsigned int i = 0; i < 3; ++i) {
    frontAmbient[i]  = frontKa * frontColor[i];
    frontDiffuse[i]  = frontKd * frontColor[i];
    frontSpecular[i] = frontKs;
  }

  glMaterialfv(GL_FRONT, GL_AMBIENT,   frontAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   frontDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  frontSpecular);
  glMaterialf(GL_FRONT,  GL_SHININESS, frontN);

  glCallList(Xaxis);
  glPopMatrix();
  glPushMatrix();
  frontColor[0] = 0.0;
  frontColor[1] = 1.0;
  frontColor[2] = 0.0;
  frontColor[3] = 1.0;

  for (unsigned int i = 0; i < 3; ++i) {
    frontAmbient[i]  = frontKa * frontColor[i];
    frontDiffuse[i]  = frontKd * frontColor[i];
    frontSpecular[i] = frontKs;
  }

  glMaterialfv(GL_FRONT, GL_AMBIENT,   frontAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   frontDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  frontSpecular);
  glMaterialf(GL_FRONT,  GL_SHININESS, frontN);

  glCallList(Yaxis);
  glPopMatrix();
  glPushMatrix();
  frontColor[0] = 0.0;
  frontColor[1] = 0.0;
  frontColor[2] = 1.0;
  frontColor[3] = 1.0;

  for (unsigned int i = 0; i < 3; ++i) {
    frontAmbient[i]  = frontKa * frontColor[i];
    frontDiffuse[i]  = frontKd * frontColor[i];
    frontSpecular[i] = frontKs;
  }

  glMaterialfv(GL_FRONT, GL_AMBIENT,   frontAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   frontDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  frontSpecular);
  glMaterialf(GL_FRONT,  GL_SHININESS, frontN);

  glCallList(Zaxis);
  glPopMatrix();
  glPopAttrib();
}

#if 1
void DisplayArrow(const vector3_type & Tail, const vector3_type& Head, const vector3_type& Color)
{
  // Right now it only works for Tail = {0, 0, 0}

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glEnable(GL_LIGHTING);

  glLineWidth(3.0);

#if 1
  vector3_type Arrow;
  real_type ArrowLength = 0;
  for (int i = 0; i < 3; ++i) {
    Arrow[i] = Head[i] - Tail[i];
    ArrowLength += Arrow[i] * Arrow[i];
  }
  ArrowLength = sqrt(ArrowLength);
  if (ArrowLength != 0) {
    for (int i = 0; i < 3; ++i) Arrow[i] /= ArrowLength;
  }
#endif

  int    ArrowFacets = 8;
  double ArrowWidth  = 0.03;
  double ArrowHeight = 0.10;

  GLfloat frontKa = 0.0;
  GLfloat frontKd = 1.0;
  GLfloat frontKs = 1.0;
  GLfloat frontN  = 128.0;
  GLfloat frontColor[] = { 1.0, 0.0, 0.0, 1.0 };
  for (int i = 0; i < 3; ++i) frontColor[i] = Color[i];
  frontColor[3] = 1.0;

  GLfloat frontAmbient[]   = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat frontDiffuse[]   = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat frontSpecular[]  = { 1.0, 1.0, 1.0, 1.0 };
  //GLfloat frontShinyness[] = { frontN };

  // Generate an arrow in the z-direction

  static GLuint Zarrow = 0;
  if (Zarrow == 0){
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    Zarrow = glGenLists(1);
    glNewList(Zarrow, GL_COMPILE);
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, ArrowLength);
    glEnd();

    glTranslated(0.0, 0.0, (1.0 - ArrowHeight) * ArrowLength);
    //glTranslated(TB->radius() - ArrowHeight, 0.0, 0.0);

    //glRotated(90.0, 0.0, 1.0, 0.0);
    //glScaled(ArrowLength, ArrowLength, ArrowLength);
    glutSolidCone(ArrowWidth, ArrowHeight, ArrowFacets, 1);
    glEndList();
    glPopMatrix();
  }
  // Display the Arrow

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
#if 0
  frontColor[0] = 1.0;
  frontColor[1] = 1.0;
  frontColor[2] = 0.0;
  frontColor[3] = 1.0;
#endif
  for (unsigned int i = 0; i < 3; ++i) {
    frontAmbient[i]  = frontKa * frontColor[i];
    frontDiffuse[i]  = frontKd * frontColor[i];
    frontSpecular[i] = frontKs;
  }

  glMaterialfv(GL_FRONT, GL_AMBIENT,   frontAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   frontDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  frontSpecular);
  glMaterialf(GL_FRONT,  GL_SHININESS, frontN);

#if 0
  // Rotate the arrow into position
  // Use quaternions to make it easy

  quaternion_type Qstart = {0.0, 0.0, 0.0, 1.0};
  quaternion_type Qstop  = {0.0, 0.0, 0.0, 0.0};

  // Make Head a unit quaternion

  real_type lengthHead = 0;
  for (int i = 1; i < 4; ++i) lengthHead += Head[i];
  lengthHead = sqrt(lengthHead);
  if (lengthHead != 0) {
    for (int i = 1; i < 4; ++i) {
      //Qstop[i]  = Arrow[i];
      Qstop[i] = Head[i] / lengthHead;
    }
  }

#if 0
  std::cout << "Qstart:" << std::endl;
  std::cout << Qstart << std::endl;
  std::cout << "Qstop:" << std::endl;
  std::cout << Qstop << std::endl;
#endif

  quaternion_type Qrotation;
  // Multiply Qstop and Qstart
  // (s1 * s2 - v1 * v2, s1 * v2 + s2 * v1  v1 x v2)

  real_type dotprod = 0;
  for (int i = 1; i < 4; ++i) dotprod += Qstop[i] * Qstart[i];

  // Make the cross product Qstop x Qstart

  real_type Xcrossprod = Qstop[2]  * Qstart[3] - Qstart[2] * Qstop[3];
  real_type Ycrossprod = Qstart[1] * Qstop[3]  - Qstop[1]  * Qstart[3];
  real_type Zcrossprod = Qstop[1]  * Qstart[2] - Qstart[1] * Qstop[2];
#if 0
  std::cout << "Cross Product" << std::endl;
  std::cout << "(" << Xcrossprod << ", " << Ycrossprod << ", " << Zcrossprod << ")" << std::endl;
#endif

  Qrotation[0] = Qstop[0] * Qstart[0] - dotprod;
  Qrotation[1] = Qstop[0] * Qstart[1] + Qstart[0] * Qstop[1] + Xcrossprod;
  Qrotation[2] = Qstop[0] * Qstart[2] + Qstart[0] * Qstop[2] + Ycrossprod;
  Qrotation[3] = Qstop[0] * Qstart[3] + Qstart[0] * Qstop[3] + Zcrossprod;
  // Negate Qrotation

  for (int i = 0; i < 4; ++i) Qrotation[i] = -Qrotation[i];

#if 0
  // Take the square root of Qrotation

  real_type lengthV = 0;
  for (int i = 1; i < 4; ++i) lengthV += Qrotation[i] * Qrotation[i];
  lengthV = sqrt(lengthV);

  vector3_type N = {0.0, 0.0, 0.0};
  if (lengthV != 0.0) {
    for (int i = 0; i < 3; ++i) N[i] = Qrotation[i+1] / lengthV;
  }

  Qrotation[0] = sqrt((1.0 + Qrotation[0]) / 2.0);
  real_type factor  = sqrt((1.0 - Qrotation[0]) / 2.0);
  Qrotation[1] = N[0] * factor;
  Qrotation[2] = N[2] * factor;
  Qrotation[3] = N[3] * factor;
#endif

  // Convert Qrotation into a Matrix

  real_type W = Qrotation[0];
  real_type X = Qrotation[1];
  real_type Y = Qrotation[2];
  real_type Z = Qrotation[3];

#if 0
  std::cout << "Qrotation" << std::endl;
  std::cout << Qrotation   << std::endl;
#endif

  Transform transformation;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      transformation[i][j] = 0;
    }
  }
  for (int i = 0; i < 4; ++i) transformation[i][i] = 1;

  if (sqrt(X*X + Y*Y + Z*Z) != 0) {
    transformation[0][0] = 1 - 2 * Y * Y - 2 * Z * Z;
    transformation[0][1] = 2 * X * Y - 2 * W * Z;
    transformation[0][2] = 2 * X * Z + 2 * W * Y;
    transformation[0][3] = 0;

    transformation[1][0] = 2 * X * Y + 2 * W * Z;
    transformation[1][1] = 1 - 2 * X * X - 2 * Z * Z;
    transformation[1][2] = 2 * Y * Z - 2 * W * X;
    transformation[1][3] = 0;

    transformation[2][0] = 2 * X * Z - 2 * W * Y;
    transformation[2][1] = 2 * Y * Z + 2 * W * X;
    transformation[2][2] = 1 - 2 * X * X - 2 * Y * Y;
    transformation[2][3] = 0;

    transformation[3][0] = 0;
    transformation[3][1] = 0;
    transformation[3][2] = 0;
    transformation[3][3] = 1;
  }
  else {
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        transformation[i][j] = 0;
      }
    }
    for (int i = 0; i < 4; ++i) transformation[i][i] = 1;
  }
#if 0
  std::cout << "Transformation:" << std::endl;
  std::cout << transformation << std::endl;
#endif

  // Convert the Matrix into the format used by OpenGL

  glTransform glmatrix;
  int glindex = 0;
  for (unsigned int col = 0; col < 4; ++col)
  {
    for (unsigned int row = 0; row < 4; ++row)
    {
      glmatrix[glindex++] = transformation[row][col];
    }
  }

  // Apply the rotation
#if 1
  glLoadMatrixd(glmatrix);
#endif
#endif

#if 1
  // Rotate the Zaxis into the Arrow vector

  real_type DotProduct = Arrow[2];
  real_type Angle      = 180.0 * acos(DotProduct) / OT_M_PI;

  vector3_type CrossProduct;
  CrossProduct[0] = - Arrow[1];
  CrossProduct[1] =   Arrow[0];
  CrossProduct[2] =   0;

  real_type LengthAxis = CrossProduct[0] * CrossProduct[0] +
    CrossProduct[1] * CrossProduct[1] +
    CrossProduct[2] * CrossProduct[2];

  vector3_type Axis = vector3_type(0, 0, 0);
  if (LengthAxis != 0) {
    for (int i = 0; i < 3; ++i) Axis[i] = CrossProduct[i] / LengthAxis;
  }

  glRotated(Angle, Axis[0], Axis[1], Axis[2]);
  glCallList(Zarrow);

#else
  glBegin(GL_LINES);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(0.0, 0.0, ArrowLength);
  glEnd();

  glTranslated(0.0, 0.0, (1.0 - ArrowHeight) * ArrowLength);
  //glTranslated(TB->radius() - ArrowHeight, 0.0, 0.0);

  //glRotated(90.0, 0.0, 1.0, 0.0);
  //glScaled(ArrowLength, ArrowLength, ArrowLength);
  glutSolidCone(ArrowWidth, ArrowHeight, ArrowFacets, 1);
#endif

  glPopMatrix();
  glPopAttrib();
}
#endif


#if 1
void DisplayCircularArch(const real_type Radius,
                         const vector3_type& Start,
                         const vector3_type& Stop)
{
  // Start and Stop are unit vectors

  real_type lengthStart = 0;
  real_type lengthStop  = 0;
  for (int i = 0; i < 3; ++i) {
    lengthStart += Start[i] * Start[i];
    lengthStop  += Stop[i]  * Stop[i];
  }
  lengthStart = sqrt(lengthStart);
  lengthStop  = sqrt(lengthStop);

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_LIGHTING);
  glLineWidth(3.0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  vector3_type Axis;
  Axis[0] = Start[1] * Stop[2]  - Stop[1]  * Start[2];
  Axis[1] = Stop[0]  * Start[2] - Start[0] * Stop[2];
  Axis[2] = Start[0] * Stop[1]  - Stop[0]  * Start[1];
  real_type lengthAxis = 0;
  for (int i = 0; i < 3; ++i) lengthAxis += Axis[i] * Axis[i];
  lengthAxis = sqrt(lengthAxis);
  if (lengthAxis != 0) {
    for (int i = 0; i < 3; ++i) Axis[i] /= lengthAxis;
  }
  real_type sinOmega = lengthAxis;
  real_type cosOmega = 0;
  for (int i = 0; i < 3; ++i) {
    cosOmega += Start[i] * Stop[i];
  }
  real_type Omega = atan2(sinOmega, cosOmega);
  if (lengthAxis != 0) {
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    real_type DeltaTheta = Omega / 20.0;
    real_type Alpha = 0;
    real_type Beta  = 0;
    vector3_type P;
    glVertex3d(Start[0] * Radius, Start[1] * Radius, Start[2] * Radius);
    for (real_type Theta = 0; Theta <= Omega; Theta += DeltaTheta) {
      Alpha = sin(Omega - Theta) / sinOmega;
      Beta  = sin(Theta) / sinOmega;
      for (int i = 0; i < 3; ++i) {
        P[i] = Alpha * Start[i] + Beta * Stop[i];
      }

      glVertex3d(P[0] * Radius, P[1] * Radius, P[2] * Radius);
    }
    glVertex3d(Stop[0] * Radius, Stop[1] * Radius, Stop[2] * Radius);
    glEnd();
  }
  glPopMatrix();
  glPopAttrib();
}
#endif


void DisplayRotationPlane(const real_type Radius,
                          const vector3_type& Start,
                          const vector3_type& Stop)
{
  GLfloat frontKa = 0.6;
  GLfloat frontKd = 0.0;
  GLfloat frontKs = 0.0;
  GLfloat frontN  = 128.0;
  GLfloat frontColor[] = { 1.0, 1.0, 1.0, 0.5 };

  GLfloat frontAmbient[]   = { 1.0, 1.0, 1.0, 0.3 };
  GLfloat frontDiffuse[]   = { 1.0, 1.0, 1.0, 0.3 };
  GLfloat frontSpecular[]  = { 1.0, 1.0, 1.0, 0.3 };

  GLfloat backKa = 0.6;
  GLfloat backKd = 0.0;
  GLfloat backKs = 0.0;
  GLfloat backN  = 128.0;
  GLfloat backColor[] = { 1.0, 1.0, 1.0, 0.5 };

  GLfloat backAmbient[]   = { 1.0, 1.0, 1.0, 0.3 };
  GLfloat backDiffuse[]   = { 1.0, 1.0, 1.0, 0.3 };
  GLfloat backSpecular[]  = { 1.0, 1.0, 1.0, 0.3 };

  // Start and Stop are unit vectors

  real_type lengthStart = 0;
  real_type lengthStop  = 0;
  for (int i = 0; i < 3; ++i) {
    lengthStart += Start[i] * Start[i];
    lengthStop  += Stop[i]  * Stop[i];
  }
  lengthStart = sqrt(lengthStart);
  lengthStop  = sqrt(lengthStop);

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_LIGHTING);
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  for (unsigned int i = 0; i < 3; ++i) {
    frontAmbient[i]  = frontKa * frontColor[i];
    frontDiffuse[i]  = frontKd * frontColor[i];
    frontSpecular[i] = frontKs;

    backAmbient[i]  = backKa * backColor[i];
    backDiffuse[i]  = backKd * backColor[i];
    backSpecular[i] = backKs;
  }

  glMaterialfv(GL_FRONT, GL_AMBIENT,   frontAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   frontDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  frontSpecular);
  glMaterialf(GL_FRONT,  GL_SHININESS, frontN);

  glMaterialfv(GL_BACK, GL_AMBIENT,   backAmbient);
  glMaterialfv(GL_BACK, GL_DIFFUSE,   backDiffuse);
  glMaterialfv(GL_BACK, GL_SPECULAR,  backSpecular);
  glMaterialf(GL_BACK,  GL_SHININESS, backN);

  // Transparency

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  vector3_type Axis;
  Axis[0] = Start[1] * Stop[2]  - Stop[1]  * Start[2];
  Axis[1] = Stop[0]  * Start[2] - Start[0] * Stop[2];
  Axis[2] = Start[0] * Stop[1]  - Stop[0]  * Start[1];
  real_type lengthAxis = 0;
  for (int i = 0; i < 3; ++i) lengthAxis += Axis[i] * Axis[i];
  lengthAxis = sqrt(lengthAxis);
  if (lengthAxis != 0) {
    for (int i = 0; i < 3; ++i) Axis[i] /= lengthAxis;
  }
  real_type sinOmega = lengthAxis;
  real_type cosOmega = 0;
  for (int i = 0; i < 3; ++i) {
    cosOmega += Start[i] * Stop[i];
  }
  real_type Omega = atan2(sinOmega, cosOmega);
  if (lengthAxis != 0) {
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_TRIANGLES);
    real_type DeltaTheta = Omega / 20.0;
    real_type Alpha = 0;
    real_type Beta  = 0;
    vector3_type Origin = vector3_type(0.0, 0.0, 0.0);
    vector3_type P;
    vector3_type Pold;
    Pold[0] = Start[0];
    Pold[1] = Start[1];
    Pold[2] = Start[2];
    for (real_type Theta = 0; Theta <= Omega; Theta += DeltaTheta) {
      Alpha = sin(Omega - Theta) / sinOmega;
      Beta  = sin(Theta) / sinOmega;
      for (int i = 0; i < 3; ++i) {
        P[i] = Alpha * Start[i] + Beta * Stop[i];
      }

      glNormal3d(Axis[0], Axis[1], Axis[2]);
      glVertex3d(Origin[0],
        Origin[1],
        Origin[2]);
      glNormal3d(Axis[0], Axis[1], Axis[2]);
      glVertex3d(Pold[0] * Radius,
        Pold[1] * Radius,
        Pold[2] * Radius);
      glNormal3d(Axis[0], Axis[1], Axis[2]);
      glVertex3d(P[0] * Radius,
        P[1] * Radius,
        P[2] * Radius);

      for (int i = 0; i < 3; ++i) Pold[i] = P[i];
    }
    glNormal3d(Axis[0], Axis[1], Axis[2]);
    glVertex3d(Origin[0],
      Origin[1],
      Origin[2]);
    glNormal3d(Axis[0], Axis[1], Axis[2]);
    glVertex3d(Pold[0] * Radius,
      Pold[1] * Radius,
      Pold[2] * Radius);
    glNormal3d(Axis[0], Axis[1], Axis[2]);
    glVertex3d(Stop[0] * Radius,
      Stop[1] * Radius,
      Stop[2] * Radius);
    glEnd();
  }
  glPopMatrix();
  glPopAttrib();
}


void DisplayRotationAxis(const real_type Radius)
{
  GLfloat frontKa = 0.0;
  GLfloat frontKd = 1.0;
  GLfloat frontKs = 1.0;
  GLfloat frontN  = 128.0;
  GLfloat frontColor[] = { 1.0, 0.0, 0.0, 1.0 };

  GLfloat frontAmbient[]   = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat frontDiffuse[]   = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat frontSpecular[]  = { 1.0, 1.0, 1.0, 1.0 };
  //GLfloat frontShinyness[] = { frontN };

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glLineWidth(3.0);

  frontColor[0] = 1.0;
  frontColor[1] = 1.0;
  frontColor[2] = 0.0;
  frontColor[3] = 1.0;

  for (unsigned int i = 0; i < 3; ++i) {
    frontAmbient[i]  = frontKa * frontColor[i];
    frontDiffuse[i]  = frontKd * frontColor[i];
    frontSpecular[i] = frontKs;
  }

  glMaterialfv(GL_FRONT, GL_AMBIENT,   frontAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   frontDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  frontSpecular);
  glMaterialf(GL_FRONT,  GL_SHININESS, frontN);

  vector3_type Origin = vector3_type(0.0, 0.0, 0.0);
  const vector3_type& Pa = TB->anchor();

#if 0
  real_type lengthPa = 0;
  for (int i = 0; i < 3; ++i) {
    lengthPa += Pa[i] * Pa[i];
  }
  lengthPa = sqrt(lengthPa);
  std::cout << "|Pa| = " << lengthPa << std::endl;
#endif

  glBegin(GL_LINES);
  glVertex3d(Origin[0], Origin[1], Origin[2]);
  glVertex3d(Pa[0] * Radius, Pa[1] * Radius, Pa[2] * Radius);
  glEnd();

  vector3_type PaColor = vector3_type(1.0, 1.0, 0.0);
  vector3_type PaScaled;
  for (int i = 0; i < 3; ++i) PaScaled[i] = Pa[i] * Radius;
  DisplayArrow(Origin, PaScaled, PaColor);

  frontColor[0] = 0.0;
  frontColor[1] = 1.0;
  frontColor[2] = 1.0;
  frontColor[3] = 1.0;

  for (unsigned int i = 0; i < 3; ++i) {
    frontAmbient[i]  = frontKa * frontColor[i];
    frontDiffuse[i]  = frontKd * frontColor[i];
    frontSpecular[i] = frontKs;
  }

  glMaterialfv(GL_FRONT, GL_AMBIENT,   frontAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   frontDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  frontSpecular);
  glMaterialf(GL_FRONT,  GL_SHININESS, frontN);

  const vector3_type& Pc = TB->current();
  glBegin(GL_LINES);
  glVertex3d(Origin[0], Origin[1], Origin[2]);
  glVertex3d(Pc[0] * Radius, Pc[1] * Radius, Pc[2] * Radius);
  glEnd();

  vector3_type PcColor = vector3_type(0.0, 1.0, 1.0);
  vector3_type PcScaled;
  for (int i = 0; i < 3; ++i) PcScaled[i] = Pc[i] * Radius;
  DisplayArrow(Origin, PcScaled, PcColor);

#if 0
  vector3_type Yaxis = {0.0, 1.0, 0.0};
  DisplayArrow(Origin, Yaxis, PcColor);
#endif

  frontColor[0] = 1.0;
  frontColor[1] = 1.0;
  frontColor[2] = 1.0;
  frontColor[3] = 1.0;

  for (unsigned int i = 0; i < 3; ++i) {
    frontAmbient[i]  = frontKa * frontColor[i];
    frontDiffuse[i]  = frontKd * frontColor[i];
    frontSpecular[i] = frontKs;
  }

  glMaterialfv(GL_FRONT, GL_AMBIENT,   frontAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   frontDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  frontSpecular);
  glMaterialf(GL_FRONT,  GL_SHININESS, frontN);

#if 0
  const vector3_type& Axis = TB->Axis();
#else
  vector3_type Axis;
  Axis[0] = Pa[1] * Pc[2] - Pc[1] * Pa[2];
  Axis[1] = Pc[0] * Pa[2] - Pa[0] * Pc[2];
  Axis[2] = Pa[0] * Pc[1] - Pc[0] * Pa[1];
  real_type lengthAxis = 0;
  for (int i = 0; i < 3; ++i) lengthAxis += Axis[i] * Axis[i];
  if (lengthAxis != 0) {
    for (int i = 0; i < 3; ++i) Axis[i] /= lengthAxis;
  }
#endif
#if 0
  glBegin(GL_LINES);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(Axis[0], Axis[1], Axis[2]);
  glEnd();
#else
  vector3_type AxisColor = vector3_type(1.0, 1.0, 1.0);
  DisplayArrow(Origin, Axis, AxisColor);
#endif
  //DisplayCircularArch(Radius, Pa, Pc);
  glPopMatrix();
  glPopAttrib();
}


void DisplaySphereRim(const float Radius, const unsigned int M)
{
  static GLuint sphereRim = 0;
  if (sphereRim == 0) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    sphereRim = glGenLists(1);
    glNewList(sphereRim, GL_COMPILE);
    double  R          = 1.0;
    double  TWO_PI     = 2.0 * OT_M_PI;
    double  Theta      = 0.0;
    double  DeltaTheta = TWO_PI / double(M);
    double  ThetaStop  = TWO_PI;

    GLfloat P[3];

    glBegin(GL_LINE_LOOP);
    for (Theta = 0.0; Theta < ThetaStop; Theta += DeltaTheta) {
      P[0] = R * cos(Theta);
      P[1] = R * sin(Theta);
      P[2] = 0.0;

      glVertex3f(P[0], P[1], P[2]);
    }
    glEnd();
    glEndList();
    glPopMatrix();
  }

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);
  glLineWidth(1.0);
  glColor3d(0.0, 0.0, 0.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  if (Radius != 1.0) {
    glScalef(Radius, Radius, Radius);
  }
  glCallList(sphereRim);
  glPopMatrix();
  glPopAttrib();
}


void DisplayWireSphere(const float Radius, const unsigned int M, const unsigned int N)
{
  static GLuint wireSphere = 0;
  if (wireSphere == 0) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    wireSphere = glGenLists(1);
    glNewList(wireSphere, GL_COMPILE);
    double  R          = 1.0;
    double  TWO_PI     = 2.0 * OT_M_PI;
    double  Theta      = 0.0;
    double  DeltaTheta = TWO_PI / double(M);
    double  ThetaStop  = TWO_PI;
    double  Phi        = 0.0;
    double  DeltaPhi   = OT_M_PI   / double(N);
    double  PhiStop    = OT_M_PI - DeltaPhi - 0.001;

    GLfloat P1[3], P2[3], P3[3], P4[3];

    for (Theta = 0.0; Theta < ThetaStop; Theta += DeltaTheta) {
      for (Phi = DeltaPhi; Phi < PhiStop; Phi += DeltaPhi) {
        P1[0] = R * sin(Phi) * sin(Theta);
        P1[1] = R * cos(Phi);
        P1[2] = R * sin(Phi) * cos(Theta);

        P2[0] = R * sin(Phi + DeltaPhi) * sin(Theta);
        P2[1] = R * cos(Phi + DeltaPhi);
        P2[2] = R * sin(Phi + DeltaPhi) * cos(Theta);

        P3[0] = R * sin(Phi + DeltaPhi) * sin(Theta + DeltaTheta);
        P3[1] = R * cos(Phi + DeltaPhi);
        P3[2] = R * sin(Phi + DeltaPhi) * cos(Theta + DeltaTheta);

        P4[0] = R * sin(Phi) * sin(Theta + DeltaTheta);
        P4[1] = R * cos(Phi);
        P4[2] = R * sin(Phi) * cos(Theta + DeltaTheta);

        glBegin(GL_POLYGON);
        glNormal3f(P1[0], P1[1], P1[2]);
        glVertex3f(P1[0], P1[1], P1[2]);
        glNormal3f(P2[0], P2[1], P2[2]);
        glVertex3f(P2[0], P2[1], P2[2]);
        glNormal3f(P3[0], P3[1], P3[2]);
        glVertex3f(P3[0], P3[1], P3[2]);
        glNormal3f(P4[0], P4[1], P4[2]);
        glVertex3f(P4[0], P4[1], P4[2]);
        glEnd();
      }
    }
#if 0
    // This is just a white quadrilateral
    Theta = 0.0 - DeltaTheta / 2.0;
    Phi   = OT_M_PI / 2.0 - DeltaPhi / 2.0;

    P1[0] = R * sin(Phi) * sin(Theta);
    P1[1] = R * cos(Phi);
    P1[2] = R * sin(Phi) * cos(Theta);

    P2[0] = R * sin(Phi + DeltaPhi) * sin(Theta);
    P2[1] = R * cos(Phi + DeltaPhi);
    P2[2] = R * sin(Phi + DeltaPhi) * cos(Theta);

    P3[0] = R * sin(Phi + DeltaPhi) * sin(Theta + DeltaTheta);
    P3[1] = R * cos(Phi + DeltaPhi);
    P3[2] = R * sin(Phi + DeltaPhi) * cos(Theta + DeltaTheta);

    P4[0] = R * sin(Phi) * sin(Theta + DeltaTheta);
    P4[1] = R * cos(Phi);
    P4[2] = R * sin(Phi) * cos(Theta + DeltaTheta);

    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3d(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glNormal3f(P1[0], P1[1], P1[2]);
    glVertex3f(P1[0], P1[1], P1[2]);
    glNormal3f(P2[0], P2[1], P2[2]);
    glVertex3f(P2[0], P2[1], P2[2]);
    glNormal3f(P3[0], P3[1], P3[2]);
    glVertex3f(P3[0], P3[1], P3[2]);
    glNormal3f(P4[0], P4[1], P4[2]);
    glVertex3f(P4[0], P4[1], P4[2]);
    glEnd();
#endif
    glEndList();
    glPopMatrix();
  }

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_LIGHTING);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glLineWidth(1.0);
  glColor3d(0.0, 0.0, 0.0);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  if (Radius != 1.0) {
    glScalef(Radius, Radius, Radius);
  }
  glCallList(wireSphere);
  glPopMatrix();
  glPopAttrib();
}


void DisplaySolidSphere(const float Radius, const unsigned int M, const unsigned int N)
{
  static const GLfloat frontKa = 0.0;
  static const GLfloat frontKd = 0.7;
  static const GLfloat frontKs = 1.0;
  static const GLfloat frontN  = 128.0;
  static const GLfloat frontColor[] = { 0.0, 1.0, 0.0, 1.0 };

#ifdef TWOSIDED
  static const GLfloat backKa = 0.0;
  static const GLfloat backKd = 0.7;
  static const GLfloat backKs = 1.0;
  static const GLfloat backN  = 128.0;
  static const GLfloat backColor[] = { 1.0, 0.0, 0.0, 1.0 };
#endif

  static GLfloat frontAmbient[]   = { 1.0, 1.0, 1.0, 1.0 };
  static GLfloat frontDiffuse[]   = { 1.0, 1.0, 1.0, 1.0 };
  static GLfloat frontSpecular[]  = { 1.0, 1.0, 1.0, 1.0 };
  //static GLfloat frontShinyness[] = { frontN };

#ifdef TWOSIDED
  static GLfloat backAmbient[]    = { 1.0, 1.0, 1.0, 1.0 };
  static GLfloat backDiffuse[]    = { 1.0, 1.0, 1.0, 1.0 };
  static GLfloat backSpecular[]   = { 1.0, 1.0, 1.0, 1.0 };
  //static GLfloat backShinyness[]  = { backN };
#endif

  static GLuint solidSphere = 0;
  if (solidSphere == 0) {
    solidSphere = glGenLists(1);
    glNewList(solidSphere, GL_COMPILE);
    double  R          = 1.0;
    double  TWO_PI     = 2.0 * OT_M_PI;
    double  Theta      = 0.0;
    double  DeltaTheta = TWO_PI / double(M);
    double  ThetaStop  = TWO_PI;
    ThetaStop  /= (3.0 / 2.0);
    double  Phi        = 0.0;
    double  DeltaPhi   = OT_M_PI / double(N);
    double  PhiStop    = OT_M_PI - DeltaPhi;

    GLfloat P1[3], P2[3], P3[3], P4[3];

#ifndef DONTUSETRIANGLEFAN
    glBegin(GL_TRIANGLE_FAN);
    P1[0] = 0.0;
    P1[1] = R;
    P1[2] = 0.0;

    glNormal3f(P1[0], P1[1], P1[2]);
    glVertex3f(P1[0], P1[1], P1[2]);

    Phi = DeltaPhi;
    for (Theta = 0.0; Theta <= ThetaStop; Theta += DeltaTheta) {
      P1[0] = R * sin(Phi) * sin(Theta);
      P1[1] = R * cos(Phi);
      P1[2] = R * sin(Phi) * cos(Theta);

      glNormal3f(P1[0], P1[1], P1[2]);
      glVertex3f(P1[0], P1[1], P1[2]);
    }
    glEnd();
#endif

    for (Theta = 0.0; Theta < ThetaStop; Theta += DeltaTheta) {
      for (Phi = DeltaPhi; Phi < PhiStop; Phi += DeltaPhi) {
        P1[0] = R * sin(Phi) * sin(Theta);
        P1[1] = R * cos(Phi);
        P1[2] = R * sin(Phi) * cos(Theta);

        P2[0] = R * sin(Phi + DeltaPhi) * sin(Theta);
        P2[1] = R * cos(Phi + DeltaPhi);
        P2[2] = R * sin(Phi + DeltaPhi) * cos(Theta);

        P3[0] = R * sin(Phi + DeltaPhi) * sin(Theta + DeltaTheta);
        P3[1] = R * cos(Phi + DeltaPhi);
        P3[2] = R * sin(Phi + DeltaPhi) * cos(Theta + DeltaTheta);

        P4[0] = R * sin(Phi) * sin(Theta + DeltaTheta);
        P4[1] = R * cos(Phi);
        P4[2] = R * sin(Phi) * cos(Theta + DeltaTheta);

        glBegin(GL_POLYGON);
        glNormal3f(P1[0], P1[1], P1[2]);
        glVertex3f(P1[0], P1[1], P1[2]);
        glNormal3f(P2[0], P2[1], P2[2]);
        glVertex3f(P2[0], P2[1], P2[2]);
        glNormal3f(P3[0], P3[1], P3[2]);
        glVertex3f(P3[0], P3[1], P3[2]);
        glNormal3f(P4[0], P4[1], P4[2]);
        glVertex3f(P4[0], P4[1], P4[2]);
        glEnd();
#if 0
        // Display the normals
        float s = 0.25;
        glBegin(GL_LINES);
        glVertex3f(P1[0], P1[1], P1[2]);
        glVertex3f((1 + s) * P1[0],
          (1 + s) * P1[1],
          (1 + s) * P1[2]);

        glVertex3f(P2[0], P2[1], P2[2]);
        glVertex3f((1 + s) * P2[0],
          (1 + s) * P2[1],
          (1 + s) * P2[2]);

        glVertex3f(P3[0], P3[1], P3[2]);
        glVertex3f((1 + s) * P3[0],
          (1 + s) * P3[1],
          (1 + s) * P3[2]);

        glVertex3f(P3[0], P3[1], P3[2]);
        glVertex3f((1 + s) * P3[0],
          (1 + s) * P3[1],
          (1 + s) * P3[2]);
        glEnd();
#endif
      }
    }

#ifndef DONTUSETRIANGLEFAN
    glBegin(GL_TRIANGLE_FAN);
    P1[0] =   0.0;
    P1[1] = - R;
    P1[2] =   0.0;

    glNormal3f(P1[0], P1[1], P1[2]);
    glVertex3f(P1[0], P1[1], P1[2]);

    Phi = OT_M_PI - DeltaPhi;
    for (Theta = ThetaStop; Theta >= 0.0; Theta -= DeltaTheta) {
      P1[0] = R * sin(Phi) * sin(Theta);
      P1[1] = R * cos(Phi);
      P1[2] = R * sin(Phi) * cos(Theta);

      glNormal3f(P1[0], P1[1], P1[2]);
      glVertex3f(P1[0], P1[1], P1[2]);
    }
    glEnd();
#endif
    glEndList();

    for (unsigned int i = 0; i < 3; ++i) {
      frontAmbient[i]  = frontKa * frontColor[i];
      frontDiffuse[i]  = frontKd * frontColor[i];
      frontSpecular[i] = frontKs;

#ifdef TWOSIDED
      backAmbient[i]   = backKa * backColor[i];
      backDiffuse[i]   = backKd * backColor[i];
      backSpecular[i]  = backKs;
#endif
    }
#if 0
    cout << "frontAmbient  == ("
      << frontAmbient[0] << ", " << frontAmbient[1] << ", "
      << frontAmbient[2] << ", " << frontAmbient[3]
      << ")\tKa == " << frontKa << endl;
      cout << "frontDiffuse  == ("
        << frontDiffuse[0] << ", " << frontDiffuse[1] << ", "
        << frontDiffuse[2] << ", " << frontDiffuse[3]
        << ")\tKd == " << frontKd << endl;
        cout << "frontSpecular == ("
          << frontSpecular[0] << ", " << frontSpecular[1] << ", "
          << frontSpecular[2] << ", " << frontSpecular[3]
          << ")\tKs == " << frontKs << "\tN == " << frontN << endl << endl;
#endif

#ifdef TWOSIDED
#if 0
          cout << "backAmbient  == ("
            << backAmbient[0] << ", " << backAmbient[1] << ", "
            << backAmbient[2] << ", " << backAmbient[3]
            << ")\tKa == " << backKa << endl;
            cout << "backDiffuse  == ("
              << backDiffuse[0] << ", " << backDiffuse[1] << ", "
              << backDiffuse[2] << ", " << backDiffuse[3]
              << ")\tKd == " << backKd << endl;
              cout << "backSpecular == ("
                << backSpecular[0] << ", " << backSpecular[1] << ", "
                << backSpecular[2] << ", " << backSpecular[3]
                << ")\tKs == " << backKs << "\tN == " << backN << endl << endl;
#endif
#endif
  }

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glMaterialfv(GL_FRONT, GL_AMBIENT,   frontAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   frontDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  frontSpecular);
#if 1
  glMaterialf(GL_FRONT,  GL_SHININESS, frontN);
#else
  glMaterialfv(GL_FRONT, GL_SHININESS, frontShinyness);
#endif

#ifdef TWOSIDED
  glMaterialfv(GL_BACK, GL_AMBIENT,    backAmbient);
  glMaterialfv(GL_BACK, GL_DIFFUSE,    backDiffuse);
  glMaterialfv(GL_BACK, GL_SPECULAR,   backSpecular);
#if 1
  glMaterialf(GL_BACK,  GL_SHININESS,  backN);
#else
  glMaterialfv(GL_BACK, GL_SHININESS,  backShinyness);
#endif
#endif

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  if (Radius != 1.0) {
    glScalef(Radius, Radius, Radius);
  }
  glCallList(solidSphere);
  glPopMatrix();
  glPopAttrib();
}


/*******************************************************************\
*                                                                   *
*                C a l l B a c k   F u n c t i o n s                *
*                                                                   *
\*******************************************************************/

/*******************************************************************\
*                                                                   *
*                E n t r y C a l l B a c k ( i n t )                *
*                                                                   *
\*******************************************************************/

void EntryCallBack(int State)
{
  // std::cout << "-->EntryCallBack" << std::endl;

#if 1
  if (State == GLUT_ENTERED) {
    active = 1;
    std::cout << "Entering Window" << std::endl;
  }
  if (State == GLUT_LEFT) {
    active = 0;
    std::cout << "Leaving Window" << std::endl;
  }
#endif

  // std::cout << "<--EntryCallBack" << std::endl;
}


/*******************************************************************\
*                                                                   *
*  K e y b o a r d C a l l B a c k ( c h a r ,   i n t ,   i n t )  *
*                                                                   *
\*******************************************************************/

void KeyboardCallBack(unsigned char Key, int /*Xmouse*/, int /*Ymouse*/)
{
  switch (Key) {
      case 'a':    // show axis
      case 'A':
        ShowRotationAxis = ShowRotationAxis ? 0 : 1;
        break;
      case 'b':    // show trackball
      case 'B':
        ShowTrackBall = ShowTrackBall ? 0 : 1;
        break;
      case 'f':    // show coordinatesystem
      case 'F':
        ShowCoordinateFrame = ShowCoordinateFrame ? 0 : 1;
        break;
      case 'p':
      case 'P':
        ShowRotationPlane = ShowRotationPlane ? 0 : 1;
        break;
      case 'r':    // reset the trackball
      case 'R':
        TB->reset();
        break;
      case 'q':    // quit the program
      case 'Q':
        exit(0);
        break;
  }
  glutPostRedisplay();
}


/*******************************************************************\
*                                                                   *
* M o u s e C a l l B a c k ( i n t ,   i n t ,   i n t ,   i n t ) *
*                                                                   *
\*******************************************************************/

void MouseCallBack(int Button, int State, int Xmouse, int Ymouse)
{
  //std::cout << "-->MouseCallBack" << std::endl;

  real_type Xnorm = Xmouse;
  real_type Ynorm = Ymouse;
  NormalizeCoordinates(Xnorm, Ynorm);

  if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) {
    TB->begin_drag(Xnorm, Ynorm);
  }
  if (Button == GLUT_LEFT_BUTTON && State == GLUT_UP) {
    TB->end_drag(Xnorm, Ynorm);
  }
  glutPostRedisplay();

  //std::cout << "<--MouseCallBack" << std::endl;
}


/*******************************************************************\
*                                                                   *
*          M o t i o n C a l l B a c k ( i n t ,   i n t )          *
*                                                                   *
\*******************************************************************/

void MotionCallBack(int Xmouse, int Ymouse)
{
  //std::cout << "-->MotionCallBack" << std::endl;

  real_type Xnorm = Xmouse;
  real_type Ynorm = Ymouse;
  NormalizeCoordinates(Xnorm, Ynorm);

  TB->drag(Xnorm, Ynorm);

  glutPostRedisplay();

  //std::cout << "<--MotionCallBack" << std::endl;
}


/*******************************************************************\
*                                                                   *
*   P a s s i v e M o t i o n C a l l B a c k ( i n t ,   i n t )   *
*                                                                   *
\*******************************************************************/

void PassiveMotionCallBack(int Xmouse, int Ymouse)
{
  //std::cout << "-->PassiveMotionCallBack" << std::endl;

  real_type Xnorm = Xmouse;
  real_type Ynorm = Ymouse;
  NormalizeCoordinates(Xnorm, Ynorm);

  //std::cout << "<--PassiveMotionCallBack" << std::endl;
}


/*******************************************************************\
*                                                                   *
*         R e s h a p e C a l l B a c k ( i n t ,   i n t )         *
*                                                                   *
\*******************************************************************/

void ReshapeCallBack(int /*width*/, int /*height*/)
{
  //std::cout << "-->ReshapeCallBack" << std::endl;

#if 0
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  //glOrtho(Xmin, Xmax, Ymin, Ymax, Zmin, Zmax);
  glOrtho(Xmin, Xmax, Ymin, Ymax, 0, 1);

  int WindowSide = width > height ? height : width; // Keep Aspect Ratio
  Width = Height = WindowSide;

  glViewport(0, 0, WindowSide, WindowSide);
  glutPostRedisplay();
#endif

  //std::cout << "<--ReshapeCallBack" << std::endl;
}


/*******************************************************************\
*                                                                   *
*                    I d l e C a l l B a c k ( )                    *
*                                                                   *
\*******************************************************************/

void IdleCallBack()
{
  //cout << "-->IdleCallBack" << endl;
  // do some computations here when we are idle
  //cout << "<--IdleCallBack" << endl;
}


/*******************************************************************\
*                                                                   *
*                 D i s p l a y C a l l B a c k ( )                 *
*                                                                   *
\*******************************************************************/

void DisplayCallBack()
{
  //std::cout << "-->DisplayCallBack" << std::endl;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  {
    glLoadIdentity();
    glLoadMatrixd(TB->get_gl_current_rotation());


    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    {
      glLoadIdentity();
      glOrtho(Xmin, Xmax, Ymin, Ymax, Zmin, Zmax);
      glDepthRange(Zmax, Zmin);
      glClearDepth(Zmin);
      glDepthFunc(GL_GREATER);
      glEnable(GL_DEPTH_TEST);

#if 1
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      {
        glLoadIdentity();
        if (ShowTrackBall) {
          DisplaySphereRim(TB->radius(), 100);
        }
      }
      glPopMatrix();
#endif
      if (ShowTrackBall) {
        DisplayWireSphere(TB->radius(), 40, 20);
      }

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      {
        if (ShowCoordinateFrame) {
          DisplayCoordinateFrame(-0.95, 0.95, -0.95, 0.95, -0.95, 0.95);
        }
      }
      glPopMatrix();

#if 1
      if (ShowRotationAxis) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        {
          DisplayRotationAxis(TB->radius());
          GLint OrgDepthFunc;
          glGetIntegerv(GL_DEPTH_FUNC, &OrgDepthFunc);
          glDepthFunc(GL_ALWAYS);
          DisplayCircularArch(Radius, TB->anchor(), TB->current());
          glDepthFunc(OrgDepthFunc);
        }
        glPopMatrix();
      }
#endif

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      {
#if 0
        // Check the depth buffer to see if it is ok

        glDisable(GL_LIGHTING);
        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_QUADS);
        glVertex3f(-0.8, -0.8, -0.5);
        glVertex3f( 0.8, -0.8, -0.5);
        glVertex3f( 0.8,  0.8, -0.5);
        glVertex3f(-0.8,  0.8, -0.5);
        glEnd();
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_QUADS);
        glVertex3f(-0.5, -0.5, 0.5);
        glVertex3f( 0.5, -0.5, 0.5);
        glVertex3f( 0.5,  0.5, 0.5);
        glVertex3f(-0.5,  0.5, 0.5);
        glEnd();
        glEnable(GL_LIGHTING);
#endif

#if 0
        glPushMatrix();
        {
          glRotated(180.0, 1.0, 0.0, 0.0);
          glutSolidTeapot(-0.5);
        }
        glPopMatrix();
#endif

        //glutSolidSphere(0.7, 40, 20);
        DisplaySolidSphere(0.5, 200, 100);;
      }
      glPopMatrix();

      if (ShowRotationPlane) {
        glPushMatrix();
        {
          DisplayRotationPlane(TB->radius(), TB->anchor(), TB->current());
        }
        glPopMatrix();
      }

      glMatrixMode(GL_PROJECTION);
    }
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
  }
  glPopMatrix();

  glFinish();
  glutSwapBuffers();

  // check for errors

  GLenum ErrorCode;
  while ((ErrorCode = glGetError()) != GL_NO_ERROR) {
    std::cerr << "DisplayCallBack: "
      << (const char*) gluErrorString(ErrorCode) << std::endl << std::flush;
  }

  //std::cout << "<--DisplayCallBack" << std::endl;
}


/*******************************************************************\
*                                                                   *
*   I n i t i a l i z e G r a p h i c s ( i n t ,   c h a r * * )   *
*                                                                   *
\*******************************************************************/

void InitializeGraphics(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(Width, Height);
  glutInitWindowPosition(50, 50);
  glutCreateWindow("Kaiip Trackball");
  glClearColor(0.3, 0.3, 0.3, 0.0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(Xmin, Xmax, Ymin, Ymax, Zmin, Zmax);
  glViewport(0, 0, Width, Height);
  //int WindowSide = Width > Height ? Height : Width;
  //Width = Height = WindowSide;
  //glViewport(0, 0, WindowSide, WindowSide);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDepthRange(Zmax, Zmin);
  glClearDepth(Zmin);
  glDepthFunc(GL_GREATER);
  //glDepthFunc(GL_GEQUAL);
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_NORMALIZE);
  glFrontFace(GL_CCW);

  // Set up Blending

  glEnable(GL_BLEND);
  //glBlendFunc(GL_ONE, GL_ZERO);

  // Set up Antialiasing

  //glEnable(GL_LINE_SMOOTH);
  //glEnable(GL_POLYGON_SMOOTH);

#if 1
  // Set up Lights

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  // light_position is NOT a default value
  // 4th component == 1 means at finite position,
  //               == 0 means at infinity

  // Shading Model

  glShadeModel(GL_SMOOTH);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  //glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

  // light_position is NOT a default value
  // 4th component == 1 means at finite position,
  //               == 0 means at infinity
  GLfloat LightPosition[] = { 1.0, 1.0, 1.0, 1.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

  GLfloat AmbientLight[]  = { 1.0, 1.0, 1.0, 1.0 };
  glLightfv(GL_LIGHT0, GL_AMBIENT,  AmbientLight);

  GLfloat DiffuseLight[]  = { 1.0, 1.0, 1.0, 1.0 };
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  DiffuseLight);

  GLfloat SpecularLight[] = { 1.0, 1.0, 1.0, 1.0 };
  glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
#endif

  // CallBack functions

  //glutEntryFunc(EntryCallBack);
  glutKeyboardFunc(KeyboardCallBack);
  glutMouseFunc(MouseCallBack);
  glutMotionFunc(MotionCallBack);
  //glutPassiveMotionFunc(PassiveMotionCallBack);
  glutReshapeFunc(ReshapeCallBack);
  //glutIdleFunc(IdleCallBack);
  glutDisplayFunc(DisplayCallBack);
}


void RefreshMenu()
{
  glutChangeToMenuEntry(1, "   Chen", 1);
  glutChangeToMenuEntry(2, "   Shoemake", 2);
  glutChangeToMenuEntry(3, "   Bell", 3);
  glutChangeToMenuEntry(4, "   Kaiip", 4);
  glutChangeToMenuEntry(5, "   Unstable Shoemake", 5);
  glutChangeToMenuEntry(6, "   Unstable Bell", 6);
  glutChangeToMenuEntry(7, "   Unstable Kaiip", 7);
}


void SelectMenuItem(int value)
{
  switch (value) {
    case 1:
      delete TB;
      TB = new Chen<real_type>(Radius);
      RefreshMenu();
      glutSetWindowTitle("Chen Trackball");
      glutChangeToMenuEntry(1, "-> Chen", 1);
      break;
    case 2:
      delete TB;
      TB = new Shoemake<real_type>(Radius);
      RefreshMenu();
      glutSetWindowTitle("Shoemake Trackball");
      glutChangeToMenuEntry(2, "-> Shoemake", 2);
      break;
    case 3:
      delete TB;
      TB = new Bell<real_type>(Radius);
      RefreshMenu();
      glutSetWindowTitle("Bell Trackball");
      glutChangeToMenuEntry(3, "-> Bell", 3);
      break;
    case 4:
      delete TB;
      TB = new Trackball<real_type>(Radius);
      RefreshMenu();
      glutSetWindowTitle("Kaiip Trackball");
      glutChangeToMenuEntry(4, "-> Kaiip", 4);
      break;
    case 5:
      delete TB;
      //TB = new UShoemake(Radius);
      RefreshMenu();
      glutSetWindowTitle("Unstable Shoemake Trackball");
      glutChangeToMenuEntry(5, "-> Unstable Shoemake", 5);
      break;
    case 6:
      delete TB;
      //TB = new UBell(Radius);
      RefreshMenu();
      glutSetWindowTitle("Unstable Bell Trackball");
      glutChangeToMenuEntry(6, "-> Unstable Bell", 6);
      break;
    case 7:
      delete TB;
      //TB = new UTrackball(Radius);
      RefreshMenu();
      glutSetWindowTitle("Unstable Kaiip Trackball");
      glutChangeToMenuEntry(7, "-> Unstable Kaiip", 7);
      break;
  }
}


void createMenu()
{
  Menu = glutCreateMenu(SelectMenuItem);
  glutAddMenuEntry("   Chen", 1);
  glutAddMenuEntry("   Shoemake", 2);
  glutAddMenuEntry("   Bell", 3);
  glutAddMenuEntry("   Kaiip", 4);
  glutAddMenuEntry("   Unstable Shoemake", 5);
  glutAddMenuEntry("   Unstable Bell", 6);
  glutAddMenuEntry("   Unstable Kaiip", 7);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}


/*******************************************************************\
*                                                                   *
*                 m a i n ( i n t ,   c h a r * * )                 *
*                                                                   *
\*******************************************************************/
int main(int argc, char** argv)
{
  InitializeGraphics(argc, argv);
  createMenu();
  glutChangeToMenuEntry(4, "-> Kaiip", 4);
  glutMainLoop();
  return 0;
};