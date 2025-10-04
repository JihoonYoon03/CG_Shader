#pragma once
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <string>

struct rtPos {
	GLfloat x1, y1, x2, y2;
};

struct Vertex {
	GLfloat x = 0.0, y = 0.0, z = 0.0;
};

struct ColoredVertex {
	GLfloat x = 0.0, y = 0.0, z = 0.0;
	GLfloat r = 0.0, g = 0.0, b = 0.0;
};

Vertex randColor();
rtPos randSquarePos(GLfloat offset);
rtPos randRectPos(GLfloat offset);

void mPosToGL(GLuint width, GLuint height, int mx, int my, GLfloat& xGL, GLfloat& yGL);
bool isMouseIn(rtPos& pos, GLuint width, GLuint height, int mx, int my);
bool checkCollide(Vertex& center, GLuint width, GLuint height, GLfloat distCap, int mx, int my);

char* filetobuf(const char* file);
void make_vertexShaders(GLuint& vertexShader, const std::string& shaderName);
void make_fragmentShaders(GLuint& fragmentShader, const std::string& shaderName);
GLuint make_shaderProgram(const GLuint& vertexShader, const GLuint& fragmentShader);