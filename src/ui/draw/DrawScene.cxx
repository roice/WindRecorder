/*
 * Scene Drawing
 *          using OpenGL GLUT
 *
 *
 * Author: Roice (LUO Bing)
 * Date: 
 *       2017-04-16 modify for GSRAO project
 */

#include <FL/gl.h>

#include "ui/draw/draw_arena.h" // arena visualization
#include "ui/draw/materials.h" // create material lists

GLfloat localAmb[4] = { 0.7, 0.7, 0.7, 1.0 };
GLfloat ambient0[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat diffuse0[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat specular0[4] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat ambient1[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat diffuse1[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat specular1[4] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat position0[4] = { 2.0, 100.5, 1.5, 1.0 };
GLfloat position1[4] = { -2.0, 100.5, 1.0, 0.0 };

/*--------------------------------------------------------------------------
 * draw everything (environment + quad rotor)
 *--------------------------------------------------------------------------
 */
void DrawScene(void)
{
    /* GL stuff */
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, localAmb);
  	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
  	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
  	glEnable(GL_LIGHT0);
  	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
  	glLightfv(GL_LIGHT0, GL_POSITION, position0);
  	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
  	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
  	glEnable(GL_LIGHT1);
  	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
  	glLightfv(GL_LIGHT1, GL_POSITION, position1);
  	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
  	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  	glLoadIdentity();
    glEnable(GL_LIGHTING); // default enable for the rest routines

    /* draw arena */
    draw_arena();

    /* draw anemometer results */
    //draw_anemometer_results();
}

void DrawScene_init(void) // call before DrawScene
{
    create_materials();
}
/* End of DrawScene.cxx */

