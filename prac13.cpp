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
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);

//--- 필요한 변수 선언
GLint winWidth = 600, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

GLfloat shapeSizeOffset = 0.2f;

Vertex ColorTable[4] = {
	{1.0f, 0.0f, 0.0f},	// Red
	{0.0f, 1.0f, 0.5f},	// Green
	{0.0f, 0.0f, 1.0f},	// Blue
	{1.0f, 0.2f, 0.5f}	// Magenta
};

unsigned int indices[9] = {
	0, 1, 2,	// 중앙 삼각형
	0, 2, 3,	// 좌상단 삼각형
	1, 4, 2		// 우상단 삼각형
};

class ShapeObject {
public:
	enum Shape { LINE = 0, TRIANGLE, RECTANGLE, PENTAGON };

private:
	friend class Renderer;

	Vertex center;
	GLfloat size;
	ColoredVertex vertices[5];	// 0: 좌하단, 1: 우하단, 2: 상단, 3: 좌상단, 4: 우상단
	enum VertexName { LB = 0, RB, T, LT, RT };;

	Shape currentShape;
	bool isLine = false;
	unsigned int sleepFrame = 0;

	
public:
	ShapeObject(int shape, Vertex& center, GLfloat& size) : center(center), currentShape(static_cast<Shape>(shape)), size(size) {

		// 기본 정점 초기화. 오각형에서 시작하고, 아래 switch문에서 점점 접어가기
		vertices[LB] = { center.x - size * 0.65f, center.y - size, 0.0f, ColorTable[shape] };
		vertices[RB] = { center.x + size * 0.65f, center.y - size, 0.0f, ColorTable[shape] };
		vertices[T] = { center.x, center.y + size * 1.25f, 0.0f, ColorTable[shape] };
		vertices[LT] = { center.x - size, center.y + size * 0.4f, 0.0f, ColorTable[shape] };
		vertices[RT] = { center.x + size, center.y + size * 0.4f, 0.0f, ColorTable[shape] };

		// 도형 별 조정. break 없이 fallthrough
		if (shape != PENTAGON) {
			// RECTANGLE: 중앙 점 아래로 접기, 나머지 점 좌표 수정
			vertices[LB].pos.x = center.x - size;
			vertices[RB].pos.x = center.x + size;
			vertices[T] = { center.x, center.y + size, 0.0f, ColorTable[shape] };
			vertices[LT].pos.y = center.y + size;
			vertices[RT].pos.y = center.y + size;

			if (shape != RECTANGLE) {
				// TRIANGLE: 좌상단, 우상단 중앙으로 접기
				vertices[LT] = vertices[RT] = vertices[T];

				if (shape != TRIANGLE) {
					// LINE: 우하단 우상단으로, 나머지는 센터로 접기
					vertices[RB].pos.y += size * 2;
					vertices[T].pos.y -= size;
					vertices[LT] = vertices[RT] = vertices[T];
					isLine = true;
				}
			}
		}
	}

};

class Renderer {
	std::vector<GLuint> VAOs, VBOs, EBOs;
	bool onDisplay[5];

public:
	Renderer() {
		

	}

	// VAO, VBO, EBO 초기화 및 데이터 연결
	void begin(std::vector<ShapeObject>& shapeList) {
		for (int i = 0; i < shapeList.size(); i++) {
			glGenVertexArrays(1, &VAOs[i]);
			glGenBuffers(1, &VBOs[i]);
			glGenBuffers(1, &EBOs[i]);

			glBindVertexArray(VAOs[i]);
			glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
			glBufferData(GL_ARRAY_BUFFER, 5 * sizeof(ColoredVertex), shapeList[i].vertices, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 9 * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void*)(sizeof(Vertex)));
			glEnableVertexAttribArray(1);
			onDisplay[i] = true;
		}
	}

	void updatePos(std::vector<ShapeObject>& shapeList) {
		for (int i = 0; i < shapeList.size(); i++) {
			glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 5 * sizeof(ColoredVertex), shapeList[i].vertices);
		}
	}

	void draw(std::vector<ShapeObject>& shapeList) {
		for (int i = 0; i < VAOs.size(); i++) {
			if (onDisplay[i] == false) continue;

			glBindVertexArray(VAOs[i]);
			glDrawElements(shapeList[i].isLine ? GL_LINE_STRIP : GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
		}
	}
};

Vertex bgColor = { 0.1f, 0.1f, 0.1f };
Renderer renderer;
std::vector<ShapeObject> shapeList;

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

	
	//--- 세이더 읽어와서 세이더 프로그램 만들기: 사용자 정의함수 호출
	make_vertexShaders(vertexShader, "vertex.glsl"); //--- 버텍스 세이더 만들기
	make_fragmentShaders(fragmentShader, "fragment.glsl"); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram(vertexShader, fragmentShader);

	//--- 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
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


	glutSwapBuffers(); // 화면에 출력하기
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:
			break;
		}
	}
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'c':
		// 리셋하고 다시 그리기
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
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, Timer, 0); // 60 FPS
}