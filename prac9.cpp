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
void makeTriangle(std::vector<Vertex>& triangle, GLfloat x, GLfloat y);

//--- 필요한 변수 선언
GLint winWidth = 800, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

GLuint VAO[4], VBO[4]; // VAO[도형 타입], VBO[정점, 색상]
std::vector<Vertex> triangles[4];	// 사분면 별 삼각형
int totalShapes = 0, selectedQuadrant = 0, selectedIndex = 0;	// 도형 개수

void updateVABO(int targetVBO) {
	glBindVertexArray(VAO[targetVBO]); // i번째 VAO를 바인드하기
	glBindBuffer(GL_ARRAY_BUFFER, VBO[targetVBO]);

	glBufferData(GL_ARRAY_BUFFER, triangles[targetVBO].size() * sizeof(Vertex), triangles->data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(Vertex), (void*)(3 * sizeof(Vertex)));
	glEnableVertexAttribArray(1);
}

void InitBuffer(GLuint VAO[], GLuint VBO[])
{

	for (int i = 0; i < 4; i++) {
		glGenVertexArrays(1, &VAO[i]); // i번째 VAO 를 지정하고 할당하기
		glGenBuffers(1, &VBO[i]); // 2개의 i번째 VBO를 지정하고 할당하기

		updateVABO(i); // i번째 VBO 업데이트하기
		glBindVertexArray(0); // VAO 바인드 해제하기
	}
}

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
	make_vertexShaders(vertexShader); //--- 버텍스 세이더 만들기
	make_fragmentShaders(fragmentShader); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram(vertexShader, fragmentShader);

	InitBuffer(VAO, VBO);

	//--- 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMainLoop();
}

//--- 출력 콜백 함수
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	Vertex bgColor = { 1.0f, 1.0f, 1.0f };

	glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	/*for (auto& vao : VAO) {
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}*/

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
	case 'a':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'b':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'c':
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
			GLfloat xGL, yGL, randX, randY;
			mPosToGL(winWidth, winHeight, mx, my, xGL, yGL);
			if (xGL > 0.0f) {
				if (yGL > 0.0f) {	// 1사분면
					triangles[0].clear();

					// 랜덤 중심점 좌표
					randX = rand() / static_cast<float>(RAND_MAX) * 0.6f + 0.2f;	// 0.2 ~ 0.8, 최대 폭은 0.4임
					randY = rand() / static_cast<float>(RAND_MAX) * 0.6f + 0.2f;	// 0.2 ~ 0.8, 최대 높이는 0.4임
					makeTriangle(triangles[0], randX, randY);
				}
				else {				// 4사분면
					triangles[3].clear();

					randX = rand() / static_cast<float>(RAND_MAX) * 0.6f + 0.2f;
					randY = rand() / static_cast<float>(RAND_MAX) * -0.6f - 0.2f;	// -0.2 ~ -0.8
					makeTriangle(triangles[0], randX, randY);
				}
			}
			else {
				if (yGL > 0.0f) {	// 2사분면
					triangles[1].clear();

					randX = rand() / static_cast<float>(RAND_MAX) * -0.6f - 0.2f;	// -0.2 ~ -0.8
					randY = rand() / static_cast<float>(RAND_MAX) * 0.6f + 0.2f;
					makeTriangle(triangles[1], randX, randY);
				}
				else {				// 3사분면
					triangles[2].clear();

					randX = rand() / static_cast<float>(RAND_MAX) * -0.6f - 0.2f;
					randY = rand() / static_cast<float>(RAND_MAX) * -0.6f - 0.2f;
					makeTriangle(triangles[2], randX, randY);
				}
			}
		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN) {
			GLfloat xGL, yGL;
			mPosToGL(winWidth, winHeight, mx, my, xGL, yGL);
			if (xGL > 0.0f) {
				if (yGL > 0.0f) {	// 1사분면
					if (triangles[0].size() < 4) {
						if (xGL < 0.2f) xGL = 0.2f;
						if (yGL < 0.2f) yGL = 0.2f;
						makeTriangle(triangles[0], xGL, yGL);
					}
				}
				else {				// 4사분면
					if (triangles[3].size() < 4) {
						if (xGL < 0.2f) xGL = 0.2f;
						if (yGL > -0.2f) yGL = -0.2f;
						makeTriangle(triangles[3], xGL, yGL);
					}
				}
			}
			else {
				if (yGL > 0.0f) {	// 2사분면
					if (triangles[1].size() < 4) {
						if (xGL > -0.2f) xGL = -0.2f;
						if (yGL < 0.2f) yGL = 0.2f;
						makeTriangle(triangles[1], xGL, yGL);
					}
				}
				else {				// 3사분면
					if (triangles[2].size() < 4) {
						if (xGL > -0.2f) xGL = -0.2f;
						if (yGL > -0.2f) yGL = -0.2f;
						makeTriangle(triangles[2], xGL, yGL);
					}
				}
			}
		}
		break;
	}
}

void makeTriangle(std::vector<Vertex>& triangle, GLfloat x, GLfloat y) {
	GLfloat offset = rand() / static_cast<float>(RAND_MAX) * 0.2f;
	Vertex newTriangle[4] = {

	};
}