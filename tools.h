#pragma once
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

struct rtPos {
	GLfloat x1, y1, x2, y2;
};

struct vertex {
	GLfloat x = 0.0, y = 0.0, z = 0.0;
};

vertex randColor();
rtPos randSquarePos(GLfloat offset);
rtPos randRectPos(GLfloat offset);

void mPosToGL(GLuint width, GLuint height, int mx, int my, GLfloat& xGL, GLfloat& yGL);
bool isMouseIn(rtPos& pos, int mx, int my);

char* filetobuf(const char* file);
void make_vertexShaders(GLuint& vertexShader);
void make_fragmentShaders(GLuint& fragmentShader);
GLuint make_shaderProgram(const GLuint& vertexShader, const GLuint& fragmentShader);