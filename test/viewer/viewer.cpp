/***
 * CS426: homework 4
 * Subdivision interactive editor.
 *
 * Original written by Robert Osada, March 1999.
 * Updated by Renato Werneck, January 2003.
 ***/
#include <iostream>
#include <ctype.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <tr1/memory>

/**
 * local variables
 **/
// window info
static int       										winWidth, winHeight;
static float     										aspect, zoom=.8f;
static std::tr1::shared_ptr<ArcBall> arcBall;
static HMatrix   										arcBallMatrix;

// meshes
static MeshPtr mesh; // our mesh

// state
enum { DISPLAY_VERTICES, DISPLAY_EDGES, DISPLAY_FACES, DISPLAY_FACES_SMOOTH, DISPLAY_OFF };
static int ctrMeshDisplay=DISPLAY_EDGES, subMeshDisplay=DISPLAY_FACES;

enum { DRAG_NORMAL, DRAG_ORTHOGONAL, DRAG_NEIGHBORHOOD, DRAG_MULTIPLE };
static int dragMode=DRAG_ORTHOGONAL;

// gl
static Vector3f center(0,0,0), _right(1,0,0), up(0,1,0);
static float    distanceToEye=3;
static Vector3f eye=Cross(_right,up)*distanceToEye + center;

static Vector3f bbCenter; // bounding box center and scale
static float    bbScale;


/**
 * function declarations
 **/
// init
static void InitGlut (int argc, char **argv);
static void InitGL   ();
static void InitMesh (int argc, char **argv);

// glut
void Display  (void);
void Reshape  (int width, int height);
void Mouse    (int button, int state, int x, int y);
void Motion   (int x, int y);
void Keyboard (unsigned char c, int x, int y);
void Idle     (void);

// helper
static void     ProjectionMatrix ();
static void     ModelMatrix      ();
static Vector3f MultVector       (HMatrix &m, const Vector3f v);

// rotate mode
static void Rotate_Mouse  (int button, int state, int xInt, int yInt);
static void Rotate_Motion (int x, int y);

// menus
static void CreateMenus        ();
static void MainMenu           (int value);
static void DragModeMenu       (int value);
static void CtrMeshDisplayMenu (int value);
static void SubMeshDisplayMenu (int value);
static void SubdivisionMenu    (int value);
static void ZoomMenu           (int value);

static void WriteMesh (MeshPtr m, char *name);


/**
 * main
 **/
int main(int argc, char **argv)
{
  // initialize arcball
  arcBall = new ArcBall();
  arcBall->value(arcBallMatrix);

  InitMesh(argc, argv);
  InitGlut(argc, argv);
  InitGL  ();

  // start
  glutMainLoop();

  return 0;
}


/**
 * init
 **/
void InitMesh (int argc, char **argv)
{
  if(argc <= 1) {
      cerr << "Syntax: subdivision [input]" << endl; exit(-1);
  }

  char *inputName = argv[1];
  FILE *input = fopen(inputName, "rt");
  if(!input) {
    cerr << "cannot open " << inputName << endl; exit(-1);
  }

  // create control mesh
  mesh = new Mesh(input);
  fclose(input);

  // calculate bounding box transformations
  Vector3f min, max;
  mesh->getBBox(min, max);

  bbCenter = (max + min) * .5;

  bbScale = Max(max.x-bbCenter.x, bbCenter.x-min.x);
  bbScale = Max(max.y-bbCenter.y, bbCenter.y-min.y, bbScale);
  bbScale = Max(max.z-bbCenter.z, bbCenter.z-min.z, bbScale);
  if(bbScale)
    bbScale = 1. / bbScale;
}

void InitGlut (int argc, char **argv)
{
  // initialize glut
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE); // double buffered

  // create window
  glutCreateWindow( "Subdivision" );

  // setup callback functions
  glutDisplayFunc  (Display );
  glutReshapeFunc  (Reshape );
  glutMouseFunc    (Mouse   );
  glutMotionFunc   (Motion  );
  glutKeyboardFunc (Keyboard);

  // menus
  CreateMenus();
}

void InitGL ()
{
  // material and lighting
  float
  mat_specular  []={ .3f, .3f, .3f, .3f },
  mat_shininess []={ 100.f },
  light0        []={ .25f,-.25f,-1,0 };

  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
  glLightfv(GL_LIGHT0, GL_POSITION, light0);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);

  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glDisable(GL_CULL_FACE);
}


/**
 * glut
 **/
void Display (void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear buffer

  // model matrix
  ModelMatrix();

  // draw mesh
  glDisable(GL_LIGHTING);
  switch(ctrMeshDisplay)
  {
    case DISPLAY_VERTICES:
      LoadColor(Color::GREEN);
      glPointSize(7);
      mesh->drawVertices();
      break;

    case DISPLAY_EDGES:
      LoadColor(Color::GREEN);
      glPointSize(7);
      mesh->drawVertices();

      LoadColor(Color::DARK_GREEN);
      mesh->drawEdges();
      break;

    case DISPLAY_FACES:
      glEnable(GL_LIGHTING);
      glShadeModel(GL_FLAT);
      mesh->drawFaces();
      break;
  }

  // draw arcball
  glDisable(GL_LIGHTING);
  glLoadIdentity();
  arcBall->draw(1);

  // swap buffers and implicit flush
  glutSwapBuffers();
}

void Reshape (int width, int height)
{
  winWidth = width;
  winHeight = height;
  aspect = (float)width / Max(height, 1);

  glViewport(0, 0, width, height); // set viewport
  ProjectionMatrix();              // set projection matrix
}

void Mouse (int button, int state, int xInt, int yInt)
{
  /** WORK HERE **/
  Rotate_Mouse(button, state, xInt, yInt);
}

void Motion (int x, int y)
{
  /** WORK HERE **/
  Rotate_Motion(x, y);
}

void Keyboard (unsigned char c, int x, int y)
{
  // pass off to menu routines
  switch( toupper(c) )
  {
    case '+':
    case '=':
      ZoomMenu(1);
      break;

    case '-':
    case '_':
      ZoomMenu(2);
      break;

    case 'S':
      SubdivisionMenu(1);
      break;

    case 'D':
      SubdivisionMenu(2);
      break;

    case 'R':
      SubdivisionMenu(3);
      break;

    case 'Q':
      MainMenu(3);
      break;
  }
}


/**
 * helper functions
 **/
/**
 * loades the projection matrix
 **/
void ProjectionMatrix ()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // perspective transform
  gluPerspective(60.f, aspect, .1, 100);

  // viewing transform
  gluLookAt(
    eye.x,    eye.y,    eye.z,
    center.x, center.y, center.z,
    up.x,     up.y,     up.z );
}

/**
 * multiply a vector by a 4x4 matrix (homogeneous coordinates)
 **/
static Vector3f MultVector (HMatrix &m, const Vector3f v)
{
  // homogeneous coordinates
  float a[4]={ v.x, v.y, v.z, 1 }, b[4]={0,0,0,0};

  for(int i=0; i<4; i++)
  for(int j=0; j<4; j++)
    b[i] += m[i][j] * a[j];

  // normalize homogeneous coordinates
  assert(b[3]);
  return Vector3f(b[0], b[1], b[2]) / b[3];
}


/**
 * loads the model matrix
 * scales to the bounding box of the model
 **/
void ModelMatrix ()
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // arcball transformation
  glMultMatrixf((float*)arcBallMatrix);

  // zoom
  glScalef(zoom, zoom, zoom);

  // bounding box transformation
  glScalef(bbScale, bbScale, bbScale);
  glTranslatef(-bbCenter.x, -bbCenter.y, -bbCenter.z);
}


/**
 * rotate mode
 **/
void Rotate_Mouse (int button, int state, int xInt, int yInt)
{
  Quat v = ArcBall::arcBallCoord(xInt, yInt, winWidth, winHeight);

  switch(state)
  {
    case GLUT_UP:
      arcBall->mouse(v);              // sets new mouse coordinates
      arcBall->endDrag();             // indictes that dragging should end
      arcBall->update();              // alters the internal state of the arcball
      arcBall->value(arcBallMatrix);  // extracts the current matrix transform
      break;

    case GLUT_DOWN:
      arcBall->mouse(v);      // sets new mouse coordinates
      arcBall->beginDrag();   // indictes that dragging should begin
      arcBall->update();      // alters the internal state of the arcball
      break;
  }

  glutPostRedisplay();
}

void Rotate_Motion (int x, int y)
{
  Quat v = ArcBall::arcBallCoord(x, y, winWidth, winHeight);

  arcBall->mouse(v);                    // set the mouse position
  arcBall->update();                // alters the internal state of the arcball
  arcBall->value(arcBallMatrix);  // reads the matrix from the arcball
  glutPostRedisplay();
}


/**
 * menu
 **/
void CreateMenus ()
{
  int dragModeMenu = glutCreateMenu(DragModeMenu);
  glutAddMenuEntry("Normal",       DRAG_NORMAL);
  glutAddMenuEntry("Orthogonal",   DRAG_ORTHOGONAL);
  glutAddMenuEntry("Neighborhood", DRAG_NEIGHBORHOOD);
  glutAddMenuEntry("Multiple",     DRAG_MULTIPLE);

  int ctrMeshDisplayMenu = glutCreateMenu(CtrMeshDisplayMenu);
  glutAddMenuEntry("Vertices", DISPLAY_VERTICES);
  glutAddMenuEntry("Edges",    DISPLAY_EDGES);
  glutAddMenuEntry("Faces",    DISPLAY_FACES);
  glutAddMenuEntry("Off",      DISPLAY_OFF);

  int subMeshDisplayMenu = glutCreateMenu(SubMeshDisplayMenu);
  glutAddMenuEntry("Vertices",     DISPLAY_VERTICES);
  glutAddMenuEntry("Edges",        DISPLAY_EDGES);
  glutAddMenuEntry("Faces",        DISPLAY_FACES);
  glutAddMenuEntry("Smooth Faces", DISPLAY_FACES_SMOOTH);
  glutAddMenuEntry("Off",          DISPLAY_OFF);

  int subdivisionMenu = glutCreateMenu(SubdivisionMenu);
  glutAddMenuEntry("&Subdivide",    1);
  glutAddMenuEntry("Un&divide",     2);
  glutAddMenuEntry("&Reset",        3);
  glutAddMenuEntry("Finer control", 4);

  int zoomMenu = glutCreateMenu(ZoomMenu);
  glutAddMenuEntry("&+", 1);
  glutAddMenuEntry("&-", 2);

  int mainMenu;
  mainMenu = glutCreateMenu(MainMenu);
  glutAddSubMenu  ("Drag Mode",        dragModeMenu);
  glutAddSubMenu  ("Control Mesh",     ctrMeshDisplayMenu);
  glutAddSubMenu  ("Subdivided Mesh",  subMeshDisplayMenu);
  glutAddSubMenu  ("Subdivision",      subdivisionMenu);
  glutAddSubMenu  ("Zoom",             zoomMenu);

  glutAddMenuEntry("Write Control Mesh",    1);
  glutAddMenuEntry("Write Subdivided Mesh", 2);
  glutAddMenuEntry("&Quit",                 3);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void MainMenu (int value)
{
  switch(value)
  {
    case 1:
      // write mesh
      WriteMesh(mesh, "control.ray");
      break;

    case 2:
      // write subdivided mesh
      /** WORK HERE **/
      cerr << "write subdivision not implemented" << endl;
      break;

    case 3:
      // quit
      exit(0);
      break;
  }
}

void DragModeMenu (int value)
{
  // set the drag mode
  dragMode = value;
}

void CtrMeshDisplayMenu (int value)
{
  ctrMeshDisplay = value;
  glutPostRedisplay();
}

void SubMeshDisplayMenu (int value)
{
  subMeshDisplay = value;
  glutPostRedisplay();
}

void SubdivisionMenu (int value)
{
  switch(value)
  {
    case 1:
      // subdivide
      /** WORK HERE **/
      cerr << "subdivide not implemented" << endl;
      break;

    case 2:
      // undivide
      /** WORK HERE **/
      cerr << "undivide not implemented" << endl;
      break;

    case 3:
      // reset
      /** WORK HERE **/
      cerr << "reset not implemented" << endl;
      break;

    case 4:
      // finer control
      /** WORK HERE **/
      cerr << "finer control not implemented" << endl;
      break;
  }
}

void ZoomMenu (int value)
{
  switch(value)
  {
    case 1:
      zoom /= .9f;
      glutPostRedisplay();
      break;

    case 2:
      zoom *= .9f;
      glutPostRedisplay();
      break;
  }
}


/**
 * Write out a mesh to a file.
 **/
void WriteMesh (MeshPtr m, char *name)
{
  assert(m);
  assert(name);

  FILE *file = fopen(name, "wt");
  if(!file) {
    cerr << "cannot open " << name << endl; return;
  }

  if(m->write(file))
    cout << "mesh written to " << name << endl;
  else
    cout << "cannot write to " << name << endl;

  fclose(file);
}
