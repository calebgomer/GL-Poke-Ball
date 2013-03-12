/************************************
 * Erik Kremer and Caleb Gomer
 * Computer Graphics Project 2
 *
 * Pokemon Playground
 *
 * This project is hosted on github
 * https://github.com/calebgomer/GL-Poke-Ball
 ***********************************/

#include <cmath>
#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

//The PI Number
static const float MY_PI = 3.1415926536f;
int tick = 0;
int currentPokeBall = 0;

//Colors
GLfloat WHITE[] = {1, 1, 1};
GLfloat RED[] = {1, 0, 0};
GLfloat BLACK[] = {0, 0, 0};

//Materials

//Ruby
GLfloat ruby_ambient[] = {0.174500, 0.011750, 0.011750, 0.550000};
GLfloat ruby_diffuse[] = {0.614240, 0.041360, 0.041360, 0.550000};
GLfloat ruby_specular[] = {0.727811, 0.626959, 0.626959, 0.550000};
GLfloat ruby_shininess[] = {76.800003};

//Emerald
GLfloat emerald_ambient[] = {0.021500, 0.174500, 0.021500, 0.550000};
GLfloat emerald_diffuse[] = {0.075680, 0.614240, 0.075680, 0.550000};
GLfloat emerald_specular[] = {0.633000, 0.727811, 0.633000, 0.550000};
GLfloat emerald_shininess[] = {76.800003};

//Polished Silver
GLfloat p_silv_ambient[] = {0.231250, 0.231250, 0.231250, 1.000000};
GLfloat p_silv_diffuse[] = {0.277500, 0.277500, 0.277500, 1.000000};
GLfloat p_silv_specular[] = {0.773911, 0.773911, 0.773911, 1.000000};
GLfloat p_silv_shininess[] = {89.599998};

//Black rubber
GLfloat rubber_ambient[] = {0.000000, 0.000000, 0.000000, 1.000000};
GLfloat rubber_diffuse[] = {0.010000, 0.010000, 0.010000, 1.000000};
GLfloat rubber_specular[] = {0.500000, 0.500000, 0.500000, 1.000000};
GLfloat rubber_shininess[] = {32.000000};

void drawCircle(float radius, float thickness) {
  
  float x,y;
  glBegin(GL_QUADS);
  
  x = (float)radius * cos(359 * MY_PI/180.0f);
  y = (float)radius * sin(359 * MY_PI/180.0f);
  for(int j = 0; j < 360; j++) {
    glVertex3f(x, y, thickness/2);
    glVertex3f(x, y, -thickness/2);
    x = (float)radius * cos(j * MY_PI/180.0f);
    y = (float)radius * sin(j * MY_PI/180.0f);
    glVertex3f(x, y, -thickness/2);
    glVertex3f(x, y, thickness/2);
  }
  glEnd();
}


//half sphere by "Sumpfratte" found at
//http://www.opengl.org/discussion_boards/showthread.php/159402-half-sphere
//scalex - scaling of sphere around x-axis
//scaley - scaling of sphere around y-axis
//r - radius of sphere
void drawHalfSphere(int scaley, int scalex, GLfloat r) {
  int i, j;
  GLfloat v[400][3];
  
  for (i=0; i<scalex; ++i) {
    for (j=0; j<scaley; ++j) {
      v[i*scaley+j][0]=r*cos(j*2*MY_PI/scaley)*cos(i*MY_PI/(2*scalex));
      v[i*scaley+j][1]=r*sin(i*MY_PI/(2*scalex));
      v[i*scaley+j][2]=r*sin(j*2*MY_PI/scaley)*cos(i*MY_PI/(2*scalex));
    }
  }
  
  glBegin(GL_QUADS);
  for (i=0; i<scalex-1; ++i) {
    for (j=0; j<scaley; ++j) {
      glVertex3fv(v[i*scaley+j]);
      glVertex3fv(v[i*scaley+(j+1)%scaley]);
      glVertex3fv(v[(i+1)*scaley+(j+1)%scaley]);
      glVertex3fv(v[(i+1)*scaley+j]);
    }
  }
  glEnd();
}


class Camera {
  double theta;
  double y;
  double dTheta;
  double dy;
public:
  Camera(): theta(0), y(3), dTheta(0.04), dy(0.2) {}
  double getX() {return 10 * cos(theta);}
  double getY() {return y;}
  double getZ() {return 10 * sin(theta);}
  void moveRight() {theta += dTheta;}
  void moveLeft() {theta -= dTheta;}
  void moveUp() {y += dy;}
  void moveDown() {if (y > dy) y -= dy;}
};

class Environment {
  int groundList;
  int width;
  int depth;
  bool light1;
  bool light2;
public:
  Environment(int width, int depth): width(width), depth(depth), light1(true), light2(true) {}
  double centerx() {return width / 2;}
  double centerz() {return depth / 2;}
  void toggleLight1() {light1 = !light1;}
  void toggleLight2() {light2 = !light2;}
  void create() {
    tick = 0;
    groundList = glGenLists(1);
    glNewList(groundList, GL_COMPILE);
    
    GLfloat light1Position[] = {width, 10, centerz(), 1};
    glLightfv(GL_LIGHT0, GL_POSITION, light1Position);
    
    GLfloat light2Position[] = {0, 0, 0, 1};
    glLightfv(GL_LIGHT1, GL_POSITION, light2Position);
    
    glBegin(GL_QUADS);
    glNormal3d(0, 1, 0);
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, emerald_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, emerald_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, emerald_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, emerald_shininess);
    
    glVertex3d(0, 0, 0);
    glVertex3d(width, 0, 0);
    glVertex3d(width, 0, depth);
    glVertex3d(0, 0, depth);
    
    glEnd();
    glEndList();
  }
  void draw() {
    if (light1)
      glEnable(GL_LIGHT0);
    else
      glDisable(GL_LIGHT0);
    if (light2)
      glEnable(GL_LIGHT1);
    else
      glDisable(GL_LIGHT1);
    glCallList(groundList);
  }
};

class SwingSet{
	double x, y, z;
	int swingDirection;
	GLUquadric* qobj;
  double rotx;
  double roty;
  double rotz, rotSwing, rotTire;
  double speed, a;
  int direction;
  bool moving;
public:
	SwingSet(double x, double y, double z):
	x(x), y(y), z(z), swingDirection(-1),
  rotx(0), roty(0), rotz(0), rotSwing(90), rotTire(0), speed(1), direction(-1), moving(true) {}
  
  void create() {
    qobj = gluNewQuadric();
    gluQuadricNormals(qobj, GLU_SMOOTH);
  }
  
  void moveBack() {z-=speed;};
  void moveForward() {z+=speed;};
  void moveLeft() {x-=speed;};
  void moveRight() {x+=speed;};
  void moveDown(){y-=speed;};
  void moveUp() {y+=speed;};
  
  void rotateXPos() {rotx+=speed*5;};
  void rotateXNeg() {rotx-=speed*5;};
  void rotateYPos() {roty+=speed*5;};
  void rotateYNeg() {roty-=speed*5;};
  void rotateZPos() {rotz+=speed*5;};
  void rotateZNeg() {rotz-=speed*5;};
  
  void rotateSPos() {rotSwing+= speed*5;};
  void rotateSNeg() {rotSwing-= speed*5;};
  void rotateTirePos(){rotTire+= speed*5;};
  void rotateTireNeg(){rotTire-= speed*5;};
  
  void speedUp() {speed*=1.1;};
  void slowDown() {speed*=0.9;};
  
  void toggleMoving() {moving=!moving;};
  void autoMove() {moving=true;};
  void manualMove() {moving=false;};
  
  
  void update() {
    if (moving) {
      rotSwing += direction *speed;
      if (rotSwing > 140) {
        rotSwing = 140; direction = -1;
      } else if (rotSwing < 50) {
        rotSwing = 50; direction = 1;
      }
    }
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotx, 1, 0, 0);
    glRotatef(roty, 0, 1, 0);
    glRotatef(rotz, 0, 0, 1);
    
    glPushMatrix();
    //swingsetframe
    GLfloat ambient[] = {0.250000, 0.250000, 0.250000, 1.000000};
    GLfloat diffuse[] = {0.400000, 0.400000, 0.400000, 1.000000};
    GLfloat specular[] = {0.774597, 0.774597, 0.774597, 1.000000};
    GLfloat shininess[] = {76.800003};
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
    
    glTranslatef(0.1,0,1);
    
    glPushMatrix();
    glRotatef(-60,1,0,0);
    gluCylinder(qobj, .1,.1, 5, 100, 100);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0,0,5);
    glRotatef(-120,1,0,0);
    gluCylinder(qobj, .1,.1, 5, 100, 100);
    glPopMatrix();
    
    glTranslatef(6,0,0);
    
    glPushMatrix();
    glRotatef(-60,1,0,0);
    gluCylinder(qobj, .1,.1, 5, 100, 100);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0,0,5);
    glRotatef(-120,1,0,0);
    gluCylinder(qobj, .1,.1, 5, 100, 100);
    glPopMatrix();
    glPopMatrix();
    
    //top bar
    glPushMatrix();
    glTranslatef(0.1,4.33,3.5);
    glRotatef(90,0,1,0);
    gluCylinder(qobj, .1,.1, 6, 100, 100);
    glPopMatrix();
    
    //swing rope
    GLfloat ambient2[] = {.2, .2, .2, 1};
    GLfloat diffuse2[] = {.1, .1, .1, 1};
    GLfloat specular2[] = {156.0/256.0,111.0/256.0,12.0/256.0, 1};
    GLfloat shininess2[] = {6.8};
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient2);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse2);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular2);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess2);
    
    glPushMatrix();
    glTranslatef(3,4.33,3.5);
    glRotatef(rotSwing,1,0,0);
    gluCylinder(qobj, .1,.1, 2, 100, 100);
    
    //tire
    GLfloat ambient3[] = {0.020000, 0.020000, 0.020000, 1.000000};
    GLfloat diffuse3[] = {0.010000, 0.010000, 0.010000, 1.000000};
    GLfloat specular3[] = {0.400000, 0.400000, 0.400000, 1.000000};
    GLfloat shininess3[] = {10.000000};
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient3);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse3);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular3);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess3);
    
    glTranslatef(0,0,3);
    glRotatef(90,0,1,0);
    glRotatef(rotTire, 1,0,0);
    glutSolidTorus(.25,1,16,16);
    glPopMatrix();
    
    glPopMatrix();
  }
};

class PokeBall {
  double radius;
  double maximumHeight;
  double x;
  double y;
  double z;
  double a;
  double rotx;
  double roty;
  double rotz;
  double speed;
  int direction;
  bool moving;
public:
  PokeBall(double r, double h, double x, double z):
  radius(r), maximumHeight(h),
  x(x), y(h), z(z), a(0),
  rotx(0), roty(0), rotz(0),
  speed(1), direction(-1), moving(true) {}
  
  void moveBack() {z-=speed;};
  void moveForward() {z+=speed;};
  void moveLeft() {x-=speed;};
  void moveRight() {x+=speed;};
  void moveDown(){y-=speed;};
  void moveUp() {y+=speed;};
  
  void rotateXPos() {rotx+=speed*5;};
  void rotateXNeg() {rotx-=speed*5;};
  void rotateYPos() {roty+=speed*5;};
  void rotateYNeg() {roty-=speed*5;};
  void rotateZPos() {rotz+=speed*5;};
  void rotateZNeg() {rotz-=speed*5;};
  
  void speedUp() {speed*=1.1;};
  void slowDown() {speed*=0.9;};
  
  void toggleMoving() {moving=!moving;};
  void autoMove() {moving=true;};
  void manualMove() {moving=false;};
  
  void update() {
    if (moving) {
      y += direction * 0.1;
      a += 0.5;
      rotx += 0.5;
      if (y > maximumHeight) {
        y = maximumHeight; direction = -1;
      } else if (y < radius) {
        y = radius; direction = 1;
      }
    }
    
    glPushMatrix();
    
    glTranslatef(x, y, z);
    glRotatef(rotx, 1, 0, 0);
    glRotatef(roty, 0, 1, 0);
    glRotatef(rotz, 0, 0, 1);
    
    //top red part
    glMaterialfv(GL_FRONT, GL_AMBIENT, ruby_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ruby_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ruby_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, ruby_shininess);
    glColor3ub(255, 0, 0);
    
    glPushMatrix();
    float angle = abs(((int(a*2)%90)-45));
    float a2rad = MY_PI/180.0;
    float move_up = (radius*sin(angle*a2rad))/sin(90*a2rad);
    float move_back = move_up * 0.7;//(move_up/sin((90-angle)*a2rad))*sin(angle*a2rad);
    glTranslatef(0, move_up, -move_back);
    glRotatef(-angle, 1, 0, 0);
    
    drawHalfSphere(20, 20, radius);
    glPopMatrix();
    
    //bottom white part
    glMaterialfv(GL_FRONT, GL_AMBIENT, p_silv_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, p_silv_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, p_silv_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, p_silv_shininess);
    glTranslatef(0, 0.0, 0);
    glRotatef(180, 1, 0, 0);
    glColor3ub(255, 255, 255);
    
    drawHalfSphere(20, 20, radius);
    glRotatef(-180, 1, 0, 0);
    
    //black dot
    glMaterialfv(GL_FRONT, GL_AMBIENT, rubber_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, rubber_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, rubber_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, rubber_shininess);
    glTranslatef(0, 0.0, radius
                 );
    glRotatef(90, 1, 0, 0);
    glColor3ub(0, 0, 0);
    drawHalfSphere(20, 20, radius/5.0);
    glRotatef(-90, 1, 0, 0);
    
    //black ring
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, BLACK);
    glTranslatef(0, 0, -radius);
    glRotatef(90, 1, 0, 0);
    glColor3ub(0, 0, 0);
    drawCircle(radius + 0.01,radius/10.0);
    
    glPopMatrix();
  }
};

Environment area(8, 50);
Camera camera;
PokeBall balls[] = {
  PokeBall(1, 10, 6, 1),
  PokeBall(1.5, 10, 3, 4),
  PokeBall(0.4, 10, 1, 7)
};
SwingSet playground(1,0,-1);
GLUquadric* qobj;

void init() {
  qobj = gluNewQuadric();
  gluQuadricNormals(qobj, GLU_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glLightfv(GL_LIGHT0, GL_AMBIENT, WHITE);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, WHITE);
  glLightfv(GL_LIGHT0, GL_SPECULAR, WHITE);
  
  glLightfv(GL_LIGHT1, GL_AMBIENT, RED);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, RED);
  glLightfv(GL_LIGHT1, GL_SPECULAR, RED);
  
  glMaterialfv(GL_FRONT, GL_SPECULAR, WHITE);
  glMaterialf(GL_FRONT, GL_SHININESS, 30);
  
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  area.create();
  playground.create();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt(4, 2, 15,
            4, 4, -5,
            0, 1, 0);
  area.draw();
  for (int i = 0; i < sizeof balls / sizeof(PokeBall); i++) {
    balls[i].update();
  }
  
  playground.update();
  glFlush();
  glutSwapBuffers();
}

void reshape(GLint w, GLint h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, GLfloat(w) / GLfloat(h), 1.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
}

void timer(int v) {
  tick++;
  glutPostRedisplay();
  glutTimerFunc(1000/60, timer, v);
}

void special(int key, int, int) {
  switch (key) {
    case GLUT_KEY_LEFT: camera.moveLeft(); break;
    case GLUT_KEY_RIGHT: camera.moveRight(); break;
    case GLUT_KEY_UP: camera.moveUp(); break;
    case GLUT_KEY_DOWN: camera.moveDown(); break;
  }
  glutPostRedisplay();
}

void keyboardFunc (unsigned char key, int x, int y) {
  switch (key) {
    case '1':
      area.toggleLight1();
      break;
    case '2':
      area.toggleLight2();
      break;
      
      //pokeball maneuvering
    case 'w':
      balls[currentPokeBall].manualMove();
      balls[currentPokeBall].moveUp();
      break;
    case 'a':
      balls[currentPokeBall].manualMove();
      balls[currentPokeBall].moveLeft();
      break;
    case 's':
      balls[currentPokeBall].manualMove();
      balls[currentPokeBall].moveDown();
      break;
    case 'd':
      balls[currentPokeBall].manualMove();
      balls[currentPokeBall].moveRight();
      break;
    case 'q':
      balls[currentPokeBall].manualMove();
      balls[currentPokeBall].moveBack();
      break;
    case 'e':
      balls[currentPokeBall].manualMove();
      balls[currentPokeBall].moveForward();
      break;
    case 'z':
      balls[currentPokeBall].manualMove();
      balls[currentPokeBall].rotateYNeg();
      break;
    case 'x':
      balls[currentPokeBall].manualMove();
      balls[currentPokeBall].rotateYPos();
      break;
    case 'c':
      balls[currentPokeBall].manualMove();
      balls[currentPokeBall].rotateXNeg();
      break;
    case 'v':
      balls[currentPokeBall].manualMove();
      balls[currentPokeBall].rotateXPos();
      break;
    case 'g':
      balls[currentPokeBall].manualMove();
      balls[currentPokeBall].rotateZNeg();
      break;
    case 'b':
      balls[currentPokeBall].manualMove();
      balls[currentPokeBall].rotateZPos();
      break;
    case 'r':
      balls[currentPokeBall].speedUp();
      break;
    case 'f':
      balls[currentPokeBall].slowDown();
      break;
    case ' ':
      balls[currentPokeBall].toggleMoving();
      break;
      //toggle which ball to move
    case '`':
      currentPokeBall++;
      if (currentPokeBall >= sizeof balls / sizeof(PokeBall))
        currentPokeBall = 0;
      break;
      
      //swingset maneuvering
    case 'u':
      playground.manualMove();
      playground.moveUp();
      break;
    case 'h':
      playground.manualMove();
      playground.moveLeft();
      break;
    case 'j':
      playground.manualMove();
      playground.moveDown();
      break;
    case 'k':
      playground.manualMove();
      playground.moveRight();
      break;
    case 'y':
      playground.manualMove();
      playground.moveBack();
      break;
    case 'i':
      playground.manualMove();
      playground.moveForward();
      break;
    case 'n':
      playground.manualMove();
      playground.rotateYNeg();
      break;
    case 'm':
      playground.manualMove();
      playground.rotateYPos();
      break;
    case ',':
      playground.manualMove();
      playground.rotateXNeg();
      break;
    case '.':
      playground.manualMove();
      playground.rotateXPos();
      break;
    case ';':
      playground.manualMove();
      playground.rotateZNeg();
      break;
    case '/':
      playground.manualMove();
      playground.rotateZPos();
      break;
    case 'o':
      playground.speedUp();
      break;
    case 'l':
      playground.slowDown();
      break;
    case '7':
      playground.toggleMoving();
      break;
    case '-':
      playground.rotateSPos();
      break;
    case '=':
      playground.rotateSNeg();
      break;
    case '9':
      playground.rotateTirePos();
      break;
    case '0':
      playground.rotateTireNeg();
      break;
  }
}

int main(int argc, char** argv) {
  
  //set up glut
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowPosition(80, 80);
  glutInitWindowSize(800, 600);
  glutCreateWindow("Poke-Balls");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special);
  glutKeyboardFunc(keyboardFunc);
  glutTimerFunc(100, timer, 0);
  
  //do some init
  init();
  
  //loop it up
  glutMainLoop();
}