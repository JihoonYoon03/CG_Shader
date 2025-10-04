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

class Spiral {
	Vertex center, end, currentVertex;
	GLfloat angle = 0.0f, radius = 0.0f;	// 타이머 호출 시 마다 증가, Renderer에서 해당 값으로 위치 계산해 VBO 저장
	int clockwise = 0; // 1: 시계, -1: 반시계
	bool expand = true;

public:
	Spiral(Vertex center) : center(center) {
		currentVertex = center;
		clockwise = (rand() % 2) ? 1 : -1;
		end = center;
		end.x += 0.225f * clockwise;
	}

	// 지속적으로 타이머를 통해 호출됨
	void Spin() {

		// 만약 900도 이상 돌았다면, 방향 전환 후 기준점은 end로 잡는다.
		if (angle >= 900 || angle <= -900) {
			clockwise = -clockwise;
			expand = false;
		}

		angle += 0.5f * clockwise;
		radius += 0.0005f * clockwise;

		// 회전에 따른 현재 정점위치 계산. 이 값은 Renderer의 VBO에 저장
		currentVertex.x = expand ? center.x : end.x + radius * cos(angle * 3.141592f / 180.0f);
		currentVertex.y = expand ? center.y : end.y + radius * sin(angle * 3.141592f / 180.0f);
	}
};

class Renderer {
	

	GLuint VAO = 0, VBO = 0;

public:
	void refreshVABO() {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// glBufferData(GL_ARRAY_BUFFER, triangleData.size() * sizeof(ColoredVertex), triangleData.data(), GL_STATIC_DRAW);

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

	void draw() {
	}
};

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
	case 'p':
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case 'l':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
		// 개수만큼 생성
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
		if (state == GLUT_DOWN) {

		}
		break;
	}
}

GLvoid Timer(int value) {

	glutPostRedisplay();
	glutTimerFunc(1000 / 60, Timer, 0); // 60 FPS
}