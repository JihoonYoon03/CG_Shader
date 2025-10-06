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
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);

//--- 필요한 변수 선언
GLint winWidth = 800, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

GLfloat shapeSizeOffset = 0.1f;

Vertex ColorTable[5] = {
	{1.0f, 0.0f, 0.0f},	// Red
	{0.0f, 1.0f, 0.0f},	// Green
	{0.0f, 0.0f, 1.0f},	// Blue
	{1.0f, 0.0f, 1.0f}	// Magenta
};

unsigned int indices[9] = {
	0, 1, 2,	// 중앙 삼각형
	0, 2, 3,	// 좌상단 삼각형
	1, 2, 4		// 우상단 삼각형
};

class TransformShape {
public:
	enum Shape { LINE = 0, TRIANGLE, RECTANGLE, PENTAGON };

private:
	friend class Renderer;

	ColoredVertex vertices[5];	// 0: 좌하단, 1: 우하단, 2: 상단, 3: 좌상단, 4: 우상단
	enum VertexName { LB = 0, RB, T, LT, RT };;
	
	Shape currentShape;

public:
	TransformShape(int shape, Vertex& center, GLfloat& size) : currentShape(static_cast<Shape>(shape)) {
		switch (shape) {
		case LINE:
			// LINE: 좌하단 우하단으로 선 구성, 나머지 점은 정중앙에 대기
			vertices[LB] = { center.x - size, center.y - size, 0.0f, ColorTable[shape] };
			vertices[RB] = { center.x + size, center.y + size, 0.0f, ColorTable[shape]};
			vertices[T] = { center.x, center.y, 0.0f, ColorTable[shape] };
			vertices[LT] = vertices[RT] = vertices[T];
			break;

		case TRIANGLE:
			// TRIANGLE: 좌하단 우하단 상단으로 삼각형 구성, 나머지 점은 상단에 대기
			vertices[LB] = { center.x - size, center.y - size, 0.0f, ColorTable[shape] };
			vertices[RB] = { center.x + size, center.y - size, 0.0f, ColorTable[shape] };
			vertices[T] = { center.x, center.y + size, 0.0f, ColorTable[shape] };
			vertices[LT] = vertices[RT] = vertices[T];
			break;
		}
	}

};

class Renderer {
	std::vector<GLuint> VAOs, VBOs, EBOs;
	bool onDisplay[5], isLine[5];

public:
	Renderer() {
		// 5번째 인덱스는 중앙 큰 도형용
		VAOs.resize(5);
		VBOs.resize(5);
		EBOs.resize(5);
	}

	// VAO, VBO, EBO 초기화 및 데이터 연결
	void begin(std::vector<TransformShape>& shapeList) {
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
			isLine[i] = shapeList[i].currentShape == TransformShape::Shape::LINE ? true : false;
		}
		onDisplay[4] = false;	// 중앙 도형은 끄기
	}

	void draw() {
		for (int i = 0; i < VAOs.size(); i++) {
			if (onDisplay[i] == false) continue;

			glBindVertexArray(VAOs[i]);
			glDrawElements(isLine[i] ? GL_LINE_STRIP : GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
		}
	}

};

Vertex bgColor = { 0.1f, 0.1f, 0.1f };
Renderer renderer;
std::vector<TransformShape> shapeList;

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

	for (int i = 0; i < 4; i++) {
		Vertex center = { (i % 2 == 0 ? -0.5f : 0.5f), (i < 2 ? 0.5f : -0.5f), 0.0f };
		TransformShape newShape(i, center, shapeSizeOffset);
		shapeList.push_back(newShape);
	}
	Vertex center = { 0.0f, 0.0f, 0.0f };
	TransformShape newShape(0, center, shapeSizeOffset);
	shapeList.push_back(newShape);

	renderer.begin(shapeList);

	//--- 세이더 읽어와서 세이더 프로그램 만들기: 사용자 정의함수 호출
	make_vertexShaders(vertexShader, "vertex.glsl"); //--- 버텍스 세이더 만들기
	make_fragmentShaders(fragmentShader, "fragment.glsl"); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram(vertexShader, fragmentShader);

	//--- 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
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
	case 'l':
		// 선 -> 삼각형
		break;
	case 't':
		// 삼각형 -> 사각형
		break;
	case 'r':
		// 사각형 -> 오각형
		break;
	case 'p':
		// 오각형 -> 선
		break;
	case 'a':
		// 4개 도형 다시 그리기
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