//--- 필요한 헤더파일 선언
#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <iostream>
#include <stdlib.h>
#include <stdio.h>	
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <array>

#include "tools.h"

//--- 아래 5개 함수는 사용자 정의 함수 임
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int mx, int my);

//--- 필요한 변수 선언
GLint winWidth = 800, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

GLuint VAO[4], VBO[4], EBO; // VAO[도형 타입], VBO[정점, 색상] 선언

int currentShape = -1, totalShapes = 0;					// 도형 개수
Vertex vertexList[20];		// 점 배열 (정점 + 색상, 10개)
Vertex lineList[40];		// 선 배열 ((정점 + 색상) * 2, 40개)
std::vector<std::array<std::array<Vertex, 2>, 3>> triangleList;	// 삼각형 배열
std::vector<std::array<std::array<Vertex, 2>, 4>> rectList;		// 사각형 배열
unsigned int vertexListSize = 0, lineListSize = 0, triangleListSize = 0, rectListSize = 0;
unsigned int rectIndice[6] = { 0,1,2, 0,2,3 }; // 사각형 인덱스 배열


void updateVBO(int targetVBO) {

	glBindVertexArray(VAO[targetVBO]); // i번째 VAO를 바인드하기
	glBindBuffer(GL_ARRAY_BUFFER, VBO[targetVBO]);

	switch (targetVBO) {
	case 0: // 점
		glBufferData(GL_ARRAY_BUFFER, vertexListSize * 2 * sizeof(Vertex), vertexList, GL_DYNAMIC_DRAW);
		break;
	case 1:	// 선
		glBufferData(GL_ARRAY_BUFFER, lineListSize * 4 * sizeof(Vertex), lineList, GL_DYNAMIC_DRAW);
		break;
	case 2: // 삼각형
		glBufferData(GL_ARRAY_BUFFER, triangleList.size() * sizeof(std::array<std::array<Vertex, 2>, 3>), triangleList.data(), GL_DYNAMIC_DRAW);
		break;
	case 3: // 사각형
		glBufferData(GL_ARRAY_BUFFER, rectList.size() * sizeof(std::array<std::array<Vertex, 2>, 4>), rectList.data(), GL_DYNAMIC_DRAW);

		std::vector<unsigned int> indices;
		for (int i = 0; i < rectList.size(); i++) {
			unsigned int base = i * 4;
			indices.insert(indices.end(), {
				base + 0, base + 1, base + 2,  // 첫 번째 삼각형
				base + 0, base + 2, base + 3   // 두 번째 삼각형
				});
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

		break;
	}
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vertex), (void*)(sizeof(Vertex)));
	glEnableVertexAttribArray(1);
}

void InitBuffer(GLuint VAO[], GLuint VBO[])
{

	for (int i = 0; i < 4; i++) {
		glGenVertexArrays(1, &VAO[i]); // i번째 VAO 를 지정하고 할당하기
		glGenBuffers(1, &VBO[i]); // 2개의 i번째 VBO를 지정하고 할당하기

		if (i == 3) glGenBuffers(1, &EBO);
		updateVBO(i); // i번째 VBO 업데이트하기
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

	glBindVertexArray(VAO[0]); // VAO 바인드하기
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 0, vertexListSize);	// 점 그리기

	glBindVertexArray(VAO[1]);
	glLineWidth(3.0);
	glDrawArrays(GL_LINES, 0, lineListSize * 2);

	glBindVertexArray(VAO[2]);
	glDrawArrays(GL_TRIANGLES, 0, triangleList.size() * 3);
	 
	glBindVertexArray(VAO[3]); // VAO 바인드하기
	glDrawElements(GL_TRIANGLES, rectList.size() * 6, GL_UNSIGNED_INT, 0);
	
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
		currentShape = 0;
		std::cout << "point selected" << std::endl;
		break;
	case 'l':
		currentShape = 1;
		std::cout << "line selected" << std::endl;
		break;
	case 't':
		currentShape = 2;
		std::cout << "triangle selected" << std::endl;
		break;
	case 'r':
		currentShape = 3;
		std::cout << "rectangle selected" << std::endl;
		break;
	}
}

GLvoid Mouse(int button, int state, int mx, int my)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN && totalShapes < 10) {
			Vertex pos;
			mPosToGL(winWidth, winHeight, mx, my, pos.x, pos.y);
			switch (currentShape)
			{
			case -1:
				std::cout << "shape didn't selected" << std::endl;
				break;
			case 0: // 점
			{
				vertexList[vertexListSize * 2] = pos;
				vertexList[vertexListSize * 2 + 1] = randColor();
				vertexListSize++;
				totalShapes++;
				updateVBO(0);
			}
				break;
			case 1: // 선
			{
				pos.x -= 0.05f;
				pos.y += 0.05f;

				Vertex pos2 = { pos.x + 0.1f, pos.y - 0.1f, 0.0f };

				lineList[lineListSize * 4] = pos;
				lineList[lineListSize * 4 + 1] = randColor();
				lineList[lineListSize * 4 + 2] = pos2;
				lineList[lineListSize * 4 + 3] = randColor();
				lineListSize++;
				totalShapes++;
				updateVBO(1);
			}
				break;
			case 2: // 삼각형
			{
				pos.y += 0.05f;

				Vertex pos2 = { pos.x - 0.05f, pos.y - 0.1f, 0.0f };
				Vertex pos3 = { pos.x + 0.05f, pos.y - 0.1f, 0.0f };

				std::array<Vertex, 2> point1Data = { pos, randColor() };
				std::array<Vertex, 2> point2Data = { pos2, randColor() };
				std::array<Vertex, 2> point3Data = { pos3, randColor() };

				std::array<std::array<Vertex, 2>, 3> triangleData = { point1Data , point2Data, point3Data };
				triangleList.push_back(triangleData);
				totalShapes++;
				updateVBO(2);
			}
				break;
			case 3: // 사각형
			{
				pos.x += 0.05f;
				pos.y += 0.05f;

				Vertex pos2 = { pos.x - 0.1f, pos.y, 0.0f };
				Vertex pos3 = { pos.x - 0.1f, pos.y - 0.1f, 0.0f };
				Vertex pos4 = { pos.x, pos.y - 0.1f, 0.0f };

				std::array<Vertex, 2> point1Data = { pos, randColor() };
				std::array<Vertex, 2> point2Data = { pos2, randColor() };
				std::array<Vertex, 2> point3Data = { pos3, randColor() };
				std::array<Vertex, 2> point4Data = { pos4, randColor() };

				std::array<std::array<Vertex, 2>, 4> rectData = { point1Data , point2Data, point3Data, point4Data };
				rectList.push_back(rectData);
				totalShapes++;
				updateVBO(3);
			}
				break;
			}
			glutPostRedisplay();
		}
		break;
	}
}