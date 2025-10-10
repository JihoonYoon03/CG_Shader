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
	Vertex pos;
	Vertex color;
};

Vertex randColor();
rtPos randSquarePos(GLfloat offset);
rtPos randRectPos(GLfloat offset);

void mPosToGL(GLuint winWidth, GLuint winHeight, int mx, int my, GLfloat& xGL, GLfloat& yGL);
bool isMouseIn(rtPos& pos, GLuint winWidth, GLuint winHeight, int mx, int my);
bool CircleCollider(Vertex& center, GLfloat distCap, GLfloat xGL, GLfloat yGL);
bool LineCollider(Vertex& p1, Vertex& p2, GLfloat distCap, GLfloat xGL, GLfloat yGL);
bool RectCollider(Vertex& p1, Vertex& p2, GLfloat xGL, GLfloat yGL);

char* filetobuf(const char* file);
void make_vertexShaders(GLuint& vertexShader, const std::string& shaderName);
void make_fragmentShaders(GLuint& fragmentShader, const std::string& shaderName);
GLuint make_shaderProgram(const GLuint& vertexShader, const GLuint& fragmentShader);