//
// Project.cpp
// CG Project - The Cosmic Voyager
// The Orbiters
// Scene 1: Launch from Earth - Lian Talal Alanazi 446009851
// Scene 2: Orbiting Past Rotating Planets - Fatema Obaid Alruways 446000790
// Scene 3: Landing on a Distant Moon - Maha Safar Almuqati 446009760
// Scene 4: Return Back to Earth - Wareef Basim Almaghmasi 4460005374
//

#define _CRT_SECURE_NO_WARNINGS

#ifdef APPLE
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif
#include <stdlib.h>
#include <iostream>
#include <cmath>
using namespace std;

//////////////////////
// Global variables //
//////////////////////

GLint win_width = 600,
win_hight = 600;

int currentScene = 1;

// Scene 1 variables
float rocketX = 0.0f;
float rocketY = -0.5f;

// Scene 2 variables
float rocketX2 = 0.0f;
float rocketY2 = 0.0f;
float planetAngle = 0.0f;

// Scene 3 variables
float rocketX3 = 0.0f;
float rocketY3 = 0.8f;
float moonScale = 1.0f;

// Scene 4 variables
float rocketX4 = 0.0f;
float rocketY4 = 0.8f;
bool landed4 = false;

// textures for scene 2
GLuint texMercury, texMars, texVenus;

// texture for scene 4
GLuint texEarth;

char mercuryPath[] = "C:\\Users\\admin1446\\Downloads\\Mercury.bmp";
char marsPath[] = "C:\\Users\\admin1446\\Downloads\\Mars.bmp";
char venusPath[] = "C:\\Users\\admin1446\\Downloads\\Venus.bmp";
char earthPath[] = "C:\\Users\\admin1446\\Downloads\\Earth.bmp";

// shared stars
float stars[30][2] = {
    {-0.9f, 0.9f}, {-0.7f, 0.7f}, {-0.5f, 0.8f}, {-0.3f, 0.6f},
    {0.1f, 0.9f},  {0.3f, 0.7f},  {0.5f, 0.85f}, {0.7f, 0.6f},
    {0.9f, 0.8f},  {-0.8f, 0.5f}, {-0.6f, 0.3f}, {-0.1f, 0.4f},
    {0.2f, 0.5f},  {0.6f, 0.4f},  {0.8f, 0.3f},  {-0.4f, 0.2f},
    {0.4f, 0.2f},  {-0.9f, 0.1f}, {0.9f, 0.1f},  {0.0f, 0.6f},
    {-0.2f, 0.8f}, {0.3f, 0.1f},  {-0.7f, 0.1f}, {0.8f, 0.5f},
    {-0.5f, 0.4f}, {0.6f, 0.9f},  {-0.3f, 0.9f}, {0.1f, 0.4f},
    {-0.8f, 0.8f}, {0.5f, 0.5f}
};

////////////////
// Functions  //
////////////////

void drawCircle(float x, float y, float radius, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * 3.14159f * i / 60;
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();
}

void drawRect(float x, float y, float w, float h, float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x - w / 2, y - h / 2);
    glVertex2f(x + w / 2, y - h / 2);
    glVertex2f(x + w / 2, y + h / 2);
    glVertex2f(x - w / 2, y + h / 2);
    glEnd();
}

void drawText(float x, float y, const char* text)
{
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

void drawStars()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 30; i++)
        glVertex2f(stars[i][0], stars[i][1]);
    glEnd();
}

void drawSpaceBackground()
{
    drawRect(0.0f, 0.0f, 2.0f, 2.0f, 0.0f, 0.0f, 0.08f);
    drawStars();
}

GLuint LoadTexture(const char* filename, int width, int height)
{
    GLuint texture;
    FILE* file;

    file = fopen(filename, "rb");
    if (file == NULL)
    {
        cout << "Unable to open the image file: " << filename << endl;
        return 0;
    }

    // read pixel data offset (bytes 10-13)
    fseek(file, 10, SEEK_SET);
    int dataOffset = 0;
    fread(&dataOffset, 4, 1, file);

    // read bit depth (bytes 28-29)
    fseek(file, 28, SEEK_SET);
    short bitDepth = 0;
    fread(&bitDepth, 2, 1, file);

    // jump to pixel data
    fseek(file, dataOffset, SEEK_SET);

    int bytesPerPixel = bitDepth / 8;
    unsigned char* rawData = (unsigned char*)malloc(width * height * bytesPerPixel);
    fread(rawData, width * height * bytesPerPixel, 1, file);
    fclose(file);

    // convert to RGB regardless of whether source is 24 or 32 bit
    unsigned char* data = (unsigned char*)malloc(width * height * 3);
    for (int i = 0; i < width * height; ++i)
    {
        int srcIndex = i * bytesPerPixel;
        int dstIndex = i * 3;
        // BMP stores BGR(A), convert to RGB
        data[dstIndex + 0] = rawData[srcIndex + 2]; // R
        data[dstIndex + 1] = rawData[srcIndex + 1]; // G
        data[dstIndex + 2] = rawData[srcIndex + 0]; // B
    }
    free(rawData);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    free(data);

    if (glGetError() != GL_NO_ERROR)
        printf("GLError in LoadTexture()\n");

    return texture;
}

void drawTexturedCircle(float x, float y, float radius, GLuint tex)
{
    // draw solid base circle first to fill any gaps
    glColor3f(0.1f, 0.3f, 0.7f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * 3.14159f * i / 60;
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();

    // draw texture on top
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f, 0.5f);
    glVertex2f(x, y);
    for (int i = 0; i <= 60; i++)
    {
        float angle = 2.0f * 3.14159f * i / 60;
        float tx = 0.5f + 0.5f * cos(angle);
        float ty = 0.5f + 0.5f * sin(angle);
        glTexCoord2f(tx, ty);
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// draw rocket pointing up
void drawRocket(float x, float y)
{
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(x - 0.06f, y);
    glVertex2f(x + 0.06f, y);
    glVertex2f(x + 0.06f, y + 0.25f);
    glVertex2f(x - 0.06f, y + 0.25f);
    glEnd();

    glColor3f(0.9f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.06f, y + 0.25f);
    glVertex2f(x + 0.06f, y + 0.25f);
    glVertex2f(x, y + 0.38f);
    glEnd();

    glColor3f(0.8f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.06f, y);
    glVertex2f(x - 0.06f, y + 0.08f);
    glVertex2f(x - 0.14f, y);
    glEnd();

    glColor3f(0.8f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 0.06f, y);
    glVertex2f(x + 0.06f, y + 0.08f);
    glVertex2f(x + 0.14f, y);
    glEnd();

    drawCircle(x, y + 0.18f, 0.03f, 0.3f, 0.6f, 1.0f);

    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.04f, y);
    glVertex2f(x + 0.04f, y);
    glVertex2f(x, y - 0.1f);
    glEnd();

    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.02f, y);
    glVertex2f(x + 0.02f, y);
    glVertex2f(x, y - 0.06f);
    glEnd();
}

// draw rocket pointing down for landing
void drawRocketLanding(float x, float y)
{
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(x - 0.06f, y);
    glVertex2f(x + 0.06f, y);
    glVertex2f(x + 0.06f, y - 0.25f);
    glVertex2f(x - 0.06f, y - 0.25f);
    glEnd();

    glColor3f(0.9f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.06f, y - 0.25f);
    glVertex2f(x + 0.06f, y - 0.25f);
    glVertex2f(x, y - 0.38f);
    glEnd();

    glColor3f(0.8f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.06f, y);
    glVertex2f(x - 0.06f, y - 0.08f);
    glVertex2f(x - 0.14f, y);
    glEnd();

    glColor3f(0.8f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 0.06f, y);
    glVertex2f(x + 0.06f, y - 0.08f);
    glVertex2f(x + 0.14f, y);
    glEnd();

    drawCircle(x, y - 0.18f, 0.03f, 0.3f, 0.6f, 1.0f);

    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.04f, y);
    glVertex2f(x + 0.04f, y);
    glVertex2f(x, y + 0.1f);
    glEnd();

    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.02f, y);
    glVertex2f(x + 0.02f, y);
    glVertex2f(x, y + 0.06f);
    glEnd();
}

// draw astronaut - light blue outfit, light grey face
void drawAstronaut(float x, float y)
{
    // body - light blue, bigger
    drawRect(x, y, 0.13f, 0.18f, 0.5f, 0.8f, 1.0f);
    // helmet - light blue
    drawCircle(x, y + 0.15f, 0.08f, 0.5f, 0.8f, 1.0f);
    // face visor - light grey
    drawCircle(x, y + 0.15f, 0.05f, 0.75f, 0.75f, 0.75f);
    // left arm - light blue
    drawRect(x - 0.11f, y + 0.01f, 0.05f, 0.10f, 0.5f, 0.8f, 1.0f);
    // right arm - light blue
    drawRect(x + 0.11f, y + 0.01f, 0.05f, 0.10f, 0.5f, 0.8f, 1.0f);
    // left leg - light blue
    drawRect(x - 0.04f, y - 0.15f, 0.05f, 0.10f, 0.5f, 0.8f, 1.0f);
    // right leg - light blue
    drawRect(x + 0.04f, y - 0.15f, 0.05f, 0.10f, 0.5f, 0.8f, 1.0f);
}

// ============================================
// SCENE 1: Launch from Earth
// ============================================
void drawBackground()
{
    glColor3f(0.02f, 0.02f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 20; i++)
        glVertex2f(stars[i][0], stars[i][1]);
    glEnd();
}

void drawGround()
{
    glColor3f(0.1f, 0.4f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, -0.6f);
    glVertex2f(-1.0f, -0.6f);
    glEnd();

    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-0.15f, -0.6f);
    glVertex2f(0.15f, -0.6f);
    glVertex2f(0.15f, -0.55f);
    glVertex2f(-0.15f, -0.55f);
    glEnd();

    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.15f, -0.55f); glVertex2f(-0.2f, -0.6f);
    glVertex2f(0.15f, -0.55f);  glVertex2f(0.2f, -0.6f);
    glEnd();
}

void drawScene1()
{
    drawBackground();
    drawGround();
    drawRocket(rocketX, rocketY);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(-0.95f, -0.92f, "Arrow Keys: Move Rocket | N: Next Scene");
}

// ============================================
// SCENE 2: Orbiting Past Planets - with textures
// ============================================
void drawScene2()
{
    drawSpaceBackground();

    // planet 1 - Mercury
    glPushMatrix();
    glTranslatef(-0.5f, 0.5f, 0.0f);
    glRotatef(planetAngle, 0.0f, 0.0f, 1.0f);
    drawTexturedCircle(0.0f, 0.0f, 0.2f, texMercury);
    glPopMatrix();

    // planet 2 - Venus
    glPushMatrix();
    glTranslatef(0.5f, 0.0f, 0.0f);
    glRotatef(-planetAngle, 0.0f, 0.0f, 1.0f);
    drawTexturedCircle(0.0f, 0.0f, 0.25f, texVenus);
    glPopMatrix();

    // planet 3 - Mars
    glPushMatrix();
    glTranslatef(0.0f, -0.55f, 0.0f);
    glRotatef(planetAngle * 0.5f, 0.0f, 0.0f, 1.0f);
    drawTexturedCircle(0.0f, 0.0f, 0.18f, texMars);
    glPopMatrix();

    drawRocket(rocketX2, rocketY2);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(-0.95f, -0.92f, "UP: Rocket & Planets Spin | N: Next Scene");
}

// ============================================
// SCENE 3: Landing on the Moon
// ============================================
void drawScene3()
{
    drawSpaceBackground();

    glPushMatrix();
    glTranslatef(0.0f, -1.2f, 0.0f);
    glScalef(moonScale, moonScale, 1.0f);

    // moon base - light grey
    drawCircle(0.0f, 0.0f, 0.9f, 0.82f, 0.82f, 0.82f);

    // large craters
    drawCircle(-0.3f, 0.4f, 0.12f, 0.70f, 0.70f, 0.70f);
    drawCircle(0.4f, 0.2f, 0.10f, 0.70f, 0.70f, 0.70f);
    drawCircle(-0.1f, -0.2f, 0.14f, 0.70f, 0.70f, 0.70f);
    drawCircle(0.3f, -0.4f, 0.09f, 0.70f, 0.70f, 0.70f);
    drawCircle(-0.5f, -0.1f, 0.08f, 0.70f, 0.70f, 0.70f);

    // small craters
    drawCircle(0.1f, 0.5f, 0.05f, 0.68f, 0.68f, 0.68f);
    drawCircle(-0.55f, 0.3f, 0.06f, 0.68f, 0.68f, 0.68f);
    drawCircle(0.55f, -0.2f, 0.05f, 0.68f, 0.68f, 0.68f);
    drawCircle(-0.2f, 0.6f, 0.04f, 0.68f, 0.68f, 0.68f);
    drawCircle(0.45f, 0.5f, 0.04f, 0.68f, 0.68f, 0.68f);

    // crater inner shadows
    drawCircle(-0.3f, 0.4f, 0.08f, 0.60f, 0.60f, 0.60f);
    drawCircle(0.4f, 0.2f, 0.06f, 0.60f, 0.60f, 0.60f);
    drawCircle(-0.1f, -0.2f, 0.09f, 0.60f, 0.60f, 0.60f);
    drawCircle(0.3f, -0.4f, 0.05f, 0.60f, 0.60f, 0.60f);
    drawCircle(-0.5f, -0.1f, 0.04f, 0.60f, 0.60f, 0.60f);
    glPopMatrix();

    drawRocketLanding(rocketX3, rocketY3);

    if (rocketY3 <= -0.1f)
        drawAstronaut(rocketX3 + 0.3f, -0.22f);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(-0.95f, -0.92f, "DOWN: Land Rocket & Moon Grows | N: Next Scene");
}

// ============================================
// SCENE 4: Return Back to Earth
// ============================================
void drawScene4()
{
    // one solid dark blue background
    drawRect(0.0f, 0.0f, 2.0f, 2.0f, 0.05f, 0.1f, 0.3f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 15; i++)
        if (stars[i][1] > 0.3f)
            glVertex2f(stars[i][0], stars[i][1]);
    glEnd();

    // earth with texture
    drawTexturedCircle(0.0f, -1.1f, 0.85f, texEarth);

    drawRocketLanding(rocketX4, rocketY4);

    if (rocketY4 <= 0.0f)
    {
        landed4 = true;
        drawRect(0.0f, 0.3f, 0.9f, 0.18f, 0.0f, 0.3f, 0.0f);
        glColor3f(1.0f, 1.0f, 0.3f);
        drawText(-0.28f, 0.34f, "Welcome Home!");
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(-0.3f, 0.22f, "The Voyage is Complete!");
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    if (!landed4)
        drawText(-0.95f, -0.92f, "Arrow Keys: Move Rocket | Land on Earth!");
    else
        drawText(-0.6f, -0.92f, "Press R to restart | Q to quit");
}

////////////////
//  Display   //
////////////////
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch (currentScene)
    {
    case 1:
        drawScene1();
        glColor3f(1.0f, 1.0f, 0.5f);
        drawText(-0.4f, 0.9f, "Scene 1: Launch!");
        break;
    case 2:
        drawScene2();
        glColor3f(1.0f, 1.0f, 0.5f);
        drawText(-0.45f, 0.9f, "Scene 2: The Planets");
        break;
    case 3:
        drawScene3();
        glColor3f(1.0f, 1.0f, 0.5f);
        drawText(-0.45f, 0.9f, "Scene 3: Moon Landing");
        break;
    case 4:
        drawScene4();
        glColor3f(1.0f, 1.0f, 0.5f);
        drawText(-0.45f, 0.9f, "Scene 4: Return to Earth");
        break;
    }

    glutSwapBuffers();
}

////////////////
// Update     //
////////////////
void update(int value)
{
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

////////////////
// Keyboard   //
////////////////
void key(unsigned char keyPressed, int x, int y)
{
    switch (keyPressed)
    {
    case 'n': case 'N':
        if (currentScene < 4) currentScene++;
        break;

    case 'r': case 'R':
        currentScene = 1;
        rocketX = 0.0f;   rocketY = -0.5f;
        rocketX2 = 0.0f;  rocketY2 = 0.0f;
        rocketX3 = 0.0f;  rocketY3 = 0.8f;
        rocketX4 = 0.0f;  rocketY4 = 0.8f;
        planetAngle = 0.0f;
        moonScale = 1.0f;
        landed4 = false;
        break;

    case 'q': case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void specialKey(int key, int x, int y)
{
    switch (currentScene)
    {
    case 1:
        if (key == GLUT_KEY_UP)    rocketY += 0.05f;
        if (key == GLUT_KEY_DOWN)  rocketY -= 0.05f;
        if (key == GLUT_KEY_LEFT)  rocketX -= 0.05f;
        if (key == GLUT_KEY_RIGHT) rocketX += 0.05f;
        break;

    case 2:
        if (key == GLUT_KEY_UP)
        {
            rocketY2 += 0.05f;
            planetAngle += 10.0f;
            if (planetAngle > 360.0f) planetAngle = 0.0f;
        }
        if (key == GLUT_KEY_DOWN)  rocketY2 -= 0.05f;
        if (key == GLUT_KEY_LEFT)  rocketX2 -= 0.05f;
        if (key == GLUT_KEY_RIGHT) rocketX2 += 0.05f;
        break;

    case 3:
        if (key == GLUT_KEY_UP)    rocketY3 += 0.05f;
        if (key == GLUT_KEY_DOWN)
        {
            rocketY3 -= 0.05f;
            moonScale += 0.02f;
            if (moonScale > 2.0f) moonScale = 2.0f;
        }
        if (key == GLUT_KEY_LEFT)  rocketX3 -= 0.05f;
        if (key == GLUT_KEY_RIGHT) rocketX3 += 0.05f;
        break;

    case 4:
        if (!landed4)
        {
            if (key == GLUT_KEY_UP)    rocketY4 += 0.05f;
            if (key == GLUT_KEY_DOWN)  rocketY4 -= 0.05f;
            if (key == GLUT_KEY_LEFT)  rocketX4 -= 0.05f;
            if (key == GLUT_KEY_RIGHT) rocketX4 += 0.05f;
        }
        break;
    }
    glutPostRedisplay();
}

void reshapeFunc(GLint new_width, GLint new_hight)
{
    glViewport(0, 0, new_width, new_hight);
    win_width = new_width;
    win_hight = new_hight;
}

void init()
{
    glClearColor(0.0f, 0.0f, 0.08f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);

    // load planet textures for scene 2
    texMercury = LoadTexture(mercuryPath, 2048, 1024);
    texMars = LoadTexture(marsPath, 2048, 800);
    texVenus = LoadTexture(venusPath, 2048, 1024);

    // load earth texture for scene 4
    texEarth = LoadTexture(earthPath, 2048, 1024);

    cout << "================================" << endl;
    cout << "   The Cosmic Voyager           " << endl;
    cout << "   The Orbiters                 " << endl;
    cout << "================================" << endl;
    cout << " Arrow Keys - Move Rocket       " << endl;
    cout << " Scene 2 UP - Planets Spin      " << endl;
    cout << " Scene 3 DOWN - Moon Grows      " << endl;
    cout << " N         - Next Scene         " << endl;
    cout << " R         - Restart            " << endl;
    cout << " Q / ESC   - Quit               " << endl;
    cout << "================================" << endl;
}

////////////////
//    Main    //
////////////////
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(200, 100);
    glutCreateWindow("The Cosmic Voyager - The Orbiters");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshapeFunc);
    glutKeyboardFunc(key);
    glutSpecialFunc(specialKey);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}