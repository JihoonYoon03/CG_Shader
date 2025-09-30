//--- 필요한 헤더파일 선언
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <iostream>
#include <stdlib.h>
#include <stdio.h>	
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include "tools.h"

//--- 아래 5개 함수는 사용자 정의 함수 임
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int mx, int my);
GLvoid Timer(int value);
void makeTriangle(GLfloat x, GLfloat y);

//--- 필요한 변수 선언
GLint winWidth = 800, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

class Renderer;

class Triangle {
public:
	enum Direction { STOP, BOUNCE, ZIGZAG, SPIRAL_RT, SPIRAL };

private:
	friend class Renderer;
	ColoredVertex vertex[3];
	Vertex center;

	GLfloat dx = 0.0f, dy = 0.0f, speed = 0.01f, degree = 0.0f, radius = 0.0f, clockwise = 0.0f;
	GLfloat offset = 0.1f;
	Direction direction = STOP;
	bool radIncrease = true;

public:
	Triangle(Vertex center) : center(center) {
		Vertex color = { rand() / static_cast<float>(RAND_MAX), rand() / static_cast<float>(RAND_MAX), rand() / static_cast<float>(RAND_MAX) };
		vertex[0] = { center.x, center.y + offset, 0.0f, color.x, color.y, color.z };
		vertex[1] = { center.x - offset, center.y - offset, 0.0f, color.x, color.y, color.z };
		vertex[2] = { center.x + offset, center.y - offset, 0.0f, color.x, color.y, color.z };

		GLfloat allignX = 0, allignY = 0;
		if (vertex[1].x < -1.0f) allignX = -1.0f - vertex[1].x;
		else if (vertex[2].x > 1.0f) allignX = 1.0f - vertex[2].x;

		if (vertex[0].y > 1.0f) allignY = 1.0f - vertex[0].y;
		else if (vertex[1].y < -1.0f) allignY = -1.0f - vertex[1].y;

		relocate(allignX, allignY);
	}

	void switchMove(Direction dirInput) {
		relocate(dx * speed, direction == ZIGZAG ? 0 : dy * speed);
		if (direction == ZIGZAG) rotate(dx * 90);
		else if (direction == SPIRAL) {
			rotate(-degree);
			degree = 0;
		}
		switch (dirInput) {
		case STOP:
			dx = 0;	dy = 0;
			direction = STOP;
			break;
		case BOUNCE:
			dx = rand() / static_cast<float>(RAND_MAX) * 2 - 1.0f;
			dy = rand() / static_cast<float>(RAND_MAX) * 2 - 1.0f;
			direction = BOUNCE;
			break;
		case ZIGZAG:
			// 지그재그 시 회전해서 삼각형 방향 표시 필요
			dx = rand() % 2 == 0 ? -1.0f : 1.0f;
			dy = center.y < 0 ? 1.0f : -1.0f;
			rotate(-dx * 90);
			direction = ZIGZAG;
			break;
		case SPIRAL_RT:
			direction = SPIRAL_RT;
			break;
		case SPIRAL:
			center.x = 0; center.y = 0;
			clockwise = rand() % 2 == 0 ? -1.0f : 1.0f;
			setPosToCenter();
			direction = SPIRAL;
			break;
		}
	}

	void updatePos() {

		// 벽 충돌 체킹
		switch (direction) {
		case STOP:
			break;
		case BOUNCE:
			relocate(dx * speed, dy * speed);
			if (vertex[0].y > 1.0f || vertex[1].y < -1.0f)
				dy = -dy; 
			if (vertex[1].x < -1.0f || vertex[2].x > 1.0f)
				dx = -dx;

			break;
		case ZIGZAG:
			relocate(dx * speed, 0);
			if (vertex[0].x < -1.0f || vertex[0].x > 1.0f) {
				dx = -dx;
				relocate(0, abs(vertex[1].y - vertex[2].y) * dy);
				if (vertex[1].y > 1.0f || vertex[2].y > 1.0f ||
					vertex[1].y < -1.0f || vertex[2].y < -1.0f) {
					dy = -dy;
					relocate(0, abs(vertex[1].y - vertex[2].y) * dy);
				}
				rotate(180);
			}

			break;
		case SPIRAL_RT:
			break;
		case SPIRAL:
			if (radius < 1.0f && radIncrease)
				radius += 0.001f;
			else if (radius > -1.0f && !radIncrease)
				radius -= 0.001f;

			rotate(clockwise);
			degree += clockwise;
			/*GLfloat rx = radius * cos(degree * 3.141592f / 180.0f);
			GLfloat ry = radius * sin(degree * 3.141592f / 180.0f);

			relocate(radius * clockwise, radius);*/
			break;
		}
	}

	void relocate(GLfloat x, GLfloat y) { // x, y만큼 이동
		center.x += x; center.y += y;
		for (int i = 0; i < 3; i++) {
			vertex[i].x += x;
			vertex[i].y += y;
		}
	}

	void setPosToCenter() {
		vertex[0].x = center.x;
		vertex[0].y = center.y + offset;
		vertex[1].x = center.x - offset;
		vertex[1].y = center.y - offset;
		vertex[2].x = center.x + offset;
		vertex[2].y = center.y - offset;
	}

	void rotate(GLfloat degree) {
		GLfloat rad = degree * 3.141592f / 180.0f;
		for (int i = 0; i < 3; i++) {
			GLfloat x = vertex[i].x - center.x;
			GLfloat y = vertex[i].y - center.y;
			vertex[i].x = x * cos(rad) - y * sin(rad) + center.x;
			vertex[i].y = x * sin(rad) + y * cos(rad) + center.y;
		}
	}

	Direction state() { return direction; }
};

class Renderer {
	std::vector<ColoredVertex> triangleData;

	GLuint VAO = 0, VBO = 0;

public:
	void refreshVABO() {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, triangleData.size() * sizeof(ColoredVertex), triangleData.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
	}

	void begin() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		refreshVABO();
	}

	void addTriangleData(std::vector<Triangle>& target) {
		triangleData.clear();
		for (auto& tri : target) {
			for (int i = 0; i < 3; i++) {
				triangleData.push_back(tri.vertex[i]);
			}
		}
		refreshVABO();
	}

	void draw() {
		if (!triangleData.empty()) {
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, triangleData.size());
		}
	}
};

std::vector<Triangle> triangles;
Renderer renderer;

//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Example1");

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	//--- 렌더러 초기화
	renderer.begin();

	//--- 세이더 읽어와서 세이더 프로그램 만들기: 사용자 정의함수 호출
	make_vertexShaders(vertexShader); //--- 버텍스 세이더 만들기
	make_fragmentShaders(fragmentShader); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram(vertexShader, fragmentShader);

	//--- 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutTimerFunc(1000 / 60, Timer, 0);
	glutMainLoop();
}

//--- 출력 콜백 함수
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	Vertex bgColor = { 0.1f, 0.1f, 0.1f };

	glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	renderer.draw();

	glutSwapBuffers(); // 화면에 출력하기
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '0':
		for (auto& tri : triangles) {
			if (tri.state() != Triangle::STOP) tri.switchMove(Triangle::STOP);
		}
		break;
	case '1':
		for (auto& tri : triangles) {
			if (tri.state() != Triangle::BOUNCE) tri.switchMove(Triangle::BOUNCE);
		}
		break;
	case '2':
		for (auto& tri : triangles) {
			if (tri.state() != Triangle::ZIGZAG) tri.switchMove(Triangle::ZIGZAG);
		}
		break;
	case '3':
		break;
	case '4':
		for (auto& tri : triangles) {
			if (tri.state() != Triangle::SPIRAL) tri.switchMove(Triangle::SPIRAL);
		}
		break;
	case 'c':
		triangles.clear();
		glutPostRedisplay();
		break;
	case 'q':
		glutLeaveMainLoop();
		return;
	}
}

GLvoid Mouse(int button, int state, int mx, int my)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN && triangles.size() < 10) {
			GLfloat xGL, yGL;
			mPosToGL(winWidth, winHeight, mx, my, xGL, yGL);

			if		(xGL > 0.9f) xGL = 0.9f;
			else if (xGL < -0.9f) xGL = -0.9f;
			if		(yGL > 0.8f) yGL = 0.8f;
			else if (yGL < -0.8f) yGL = -0.8f;

			makeTriangle(xGL, yGL);
			glutPostRedisplay();
		}
		break;
	}
}

void makeTriangle(GLfloat x, GLfloat y) {
	Vertex center = { x , y, 0.0f };

	triangles.push_back(Vertex(center));
	renderer.addTriangleData(triangles);
}

GLvoid Timer(int value) {
	for (auto& tri : triangles) {
		tri.updatePos();
	}
	renderer.addTriangleData(triangles);
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, Timer, 0); // 60 FPS
}