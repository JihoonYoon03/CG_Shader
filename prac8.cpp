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
GLvoid Mouse(int button, int state, int mx, int my);

//--- 필요한 변수 선언
GLint width, height;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

GLuint VAO[3], VBO[3][2]; // VAO[도형 타입], VBO[도형 타입][정점, 색상] 선언

int currentShape = -1, totalShapes = 0;					// 도형 개수
std::vector<vertex[2]> vertexList;			// 점 배열
std::vector<vertex[3][2]> triangleList;	// 삼각형 배열
std::vector<vertex[4][2]> rectList;		// 사각형 배열
int rectIndice[6] = { 0,1,2, 0,2,3 }; // 사각형 인덱스 배열


void updateVBO(int targetVBO) {

	glBindBuffer(GL_ARRAY_BUFFER, VBO[targetVBO][0]);

	switch (targetVBO) {
	case 0: // 점
		glBufferData(GL_ARRAY_BUFFER, vertexList.size() * sizeof(vertex), vertexList.data(), GL_STATIC_DRAW);
		break;
	case 1: // 삼각형
		glBufferData(GL_ARRAY_BUFFER, triangleList.size() * sizeof(vertex), triangleList.data(), GL_STATIC_DRAW);
		break;
	case 2: // 사각형
		glBufferData(GL_ARRAY_BUFFER, rectList.size() * sizeof(vertex), rectList.data(), GL_STATIC_DRAW);
		break;
	}
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, VBO[targetVBO][1]);

	switch (targetVBO) {
	case 0: // 점
		glBufferData(GL_ARRAY_BUFFER, vertexList.size() * sizeof(vertex), vertexList.data(), GL_DYNAMIC_DRAW);
		break;
	case 1: // 삼각형
		glBufferData(GL_ARRAY_BUFFER, triangleList.size() * sizeof(vertex), triangleList.data(), GL_DYNAMIC_DRAW);
		break;
	case 2: // 사각형
		glBufferData(GL_ARRAY_BUFFER, rectList.size() * sizeof(vertex), rectList.data(), GL_DYNAMIC_DRAW);
		break;
	}
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
}

void InitBuffer(GLuint* VAO, GLuint* VBO[])
{
	for (int i = 0; i < 3; i++) {
		glGenVertexArrays(i, &VAO[i]); // i번째 VAO 를 지정하고 할당하기
		glBindVertexArray(VAO[i]); // i번째 VAO를 바인드하기
		glGenBuffers(2, VBO[i]); //2개의 i번째 VBO를 지정하고 할당하기

		updateVBO(i); // i번째 VBO 업데이트하기
	}
}

//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	width = 500;
	height = 500;

	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Example1");

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	//--- 세이더 읽어와서 세이더 프로그램 만들기: 사용자 정의함수 호출
	make_vertexShaders(vertexShader); //--- 버텍스 세이더 만들기
	make_fragmentShaders(fragmentShader); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram(vertexShader, fragmentShader);

	//--- 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMainLoop();
}

//--- 출력 콜백 함수
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = 0.0;
	bColor = 1.0; //--- 배경색을 파랑색으로 설정
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glPointSize(5.0);
	glDrawArrays(GL_LINES, 0, 4); //--- 렌더링하기: 0번 인덱스에서 1개의 버텍스를 사용하여 점 그리기
	glutSwapBuffers(); // 화면에 출력하기
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int mx, int my)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN && totalShapes < 10) {
			switch (currentShape)
			{
			case -1:
				std::cout << "shape didn't selected" << std::endl;
				break;
			case 0: // 점

				break;
			case 1: // 삼각형
				break;
			case 2: // 사각형
				break;
			}
		}
		break;
	}
}