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

const GLfloat PI = 3.141592f;

unsigned int indices[12] = {
	0, 3, 4,
	1, 0, 5,
	2, 1, 6,
	3, 2, 7
};

unsigned int indicesR[12] = {
	0, 4, 3,
	1, 5, 0,
	2, 6, 1,
	3, 7, 2
};

class SpecialRt {
	friend class Renderer;

	// 극좌표 사용
	Vertex center;
	GLfloat radInner, radOuter;
	GLfloat angle = 45.0f, angleOffset = 5.0f;

	std::vector<ColoredVertex> vertices;

public:
	SpecialRt(const Vertex& center, const GLfloat& In, const GLfloat& Out) : center(center), radInner(In), radOuter(Out) {
		for (int j = 0; j < 2; j++) {
			for (int i = 0; i < 4; i++) {
				vertices.push_back({
					center.x + (j == 0 ? radInner * cos((i * 90.0f + angle) * PI / 180.0f) : radOuter * cos((i * 90.0f) * PI / 180.0f)),
					center.y + (j == 0 ? radInner * sin((i * 90.0f + angle) * PI / 180.0f) : radOuter * sin((i * 90.0f) * PI / 180.0f)),
					0.0f,
					{1.0f, 0.5f, 0.0f} });
			}
		}
	}

};

class Renderer {
	std::vector<GLuint> VAOs, VBOs, EBOs;

public:
	// VAO, VBO 초기화 및 데이터 연결
	void begin(std::vector<SpecialRt>& shapeList, const int max) {

		VAOs.resize(max);
		VBOs.resize(max);
		EBOs.resize(max);

		for (int i = 0; i < shapeList.size(); i++) {
			glGenVertexArrays(1, &VAOs[i]);
			glGenBuffers(1, &VBOs[i]);
			glGenBuffers(1, &EBOs[i]);

			glBindVertexArray(VAOs[i]);
			glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
			glBufferData(GL_ARRAY_BUFFER, shapeList[i].vertices.size() * sizeof(ColoredVertex), shapeList[i].vertices.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
		}
	}

	void draw() {
		for (int i = 0; i < VAOs.size(); i++) {
			glBindVertexArray(VAOs[i]);
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		}
	}

};

Vertex bgColor = { 0.1f, 0.1f, 0.1f };
Renderer renderer;
std::vector<SpecialRt> shapeList;
bool pause = false;
GLfloat Inner = 0.1f, Outer = 0.14f;

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
	int randMax = rand() % 5 + 3; // 3 ~ 7
	for (int i = 0; i < randMax; i++) {
		shapeList.push_back({ {rand() / static_cast<float>(RAND_MAX) * 1.6f - 0.8f,
								rand() / static_cast<float>(RAND_MAX) * 1.6f - 0.8f, 0.0f},
								Inner, Outer });
	}
	renderer.begin(shapeList, randMax);

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