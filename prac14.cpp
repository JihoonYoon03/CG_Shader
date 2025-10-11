//--- 필요한 헤더파일 선언
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
GLvoid Mouse(int button, int state, int x, int y);
GLvoid MouseMotion(int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);

//--- 필요한 변수 선언
GLint winWidth = 600, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

class ShapeObject {
	friend class Renderer;

	Vertex center;



public:

};

class Renderer {
	std::vector<GLuint> VAOs, VBOs;

public:
	Renderer() {
		VAOs.resize(15);
		VBOs.resize(15);
	}

	// VAO, VBO 초기화 및 데이터 연결
	void begin() {
		
	}

};

Vertex bgColor = { 0.1f, 0.1f, 0.1f };
Renderer renderer;
bool pause = false;

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

	// 도형 데이터 초기화
	// renderer.begin(shapeList);


	//--- 세이더 읽어와서 세이더 프로그램 만들기: 사용자 정의함수 호출
	make_vertexShaders(vertexShader, "vertex.glsl"); //--- 버텍스 세이더 만들기
	make_fragmentShaders(fragmentShader, "fragment.glsl"); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram(vertexShader, fragmentShader);

	//--- 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(1000 / 60, Timer, 0);
	glutMainLoop();
}

//--- 출력 콜백 함수
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	// renderer.draw(shapeList);

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
	case 'c':
		// 화면 가운데 중심 시계방향 회전
		break;
	case 't':
		// 화면 가운데 중심 반시계방향 회전
		break;
	case 's':
		// 애니메이션 일시정지
		break;
	case 'q':
		glutLeaveMainLoop();
		return;
	}
}

GLvoid Timer(int value) {
	if (pause) return;
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, Timer, 0);
}