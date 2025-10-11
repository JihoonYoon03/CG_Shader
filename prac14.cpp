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
	GLfloat radInner, radOuter, breathAmount = 0.0f;
	GLfloat angle = 0.0f, angleOffset = 2.5f;

	std::vector<ColoredVertex> vertices;

	bool isIn = false, breath = true;
public:
	SpecialRt(const Vertex& center, const GLfloat& In, const GLfloat& Out, bool isIn) : center(center), radInner(In), radOuter(Out), isIn(isIn) {
		for (int j = 0; j < 2; j++) {
			for (int i = 0; i < 4; i++) {
				GLfloat radius = j == 0 ? radInner : radOuter;
				GLfloat currAngle = (j == 0) ? (i * 90.0f + 45.0f) : (i * 90.0f);

				vertices.push_back({
					center.x + radius * cos(currAngle * PI / 180.0f),
					center.y + radius * sin(currAngle * PI / 180.0f),
					0.0f,
					{1.0f, 0.5f, 0.0f} });
			}
		}
	}

	void spin(Vertex base, bool clockwise) {
		angle += clockwise ? -angleOffset : angleOffset;
		GLfloat radian = (clockwise ? -angleOffset : angleOffset) * PI / 180.0f;

		for (int i = 0; i < 8; i++) {
			GLfloat distX = vertices[i].pos.x - base.x;
			GLfloat distY = vertices[i].pos.y - base.y;

			vertices[i].pos.x = base.x + (distX * cos(radian) - distY * sin(radian));
			vertices[i].pos.y = base.y + (distX * sin(radian) + distY * cos(radian));
		}

		center.x = base.x + (center.x - base.x) * cos(radian) - (center.y - base.y) * sin(radian);
		center.y = base.y + (center.x - base.x) * sin(radian) + (center.y - base.y) * cos(radian);
	}

	void breathing() {
		for (int i = 0; i < 8; i++) {
			GLfloat radius = (i < 4) ? radInner : radOuter;
			breathAmount += breath ? 0.0001f : -0.0001f;
			radius += breathAmount;

			if ((breathAmount >= 0.05f && breath) || (breathAmount <= -0.01f && !breath)) {
				breath = !breath;
			}

			GLfloat currAngle = (i < 4) ? (i * 90.0f + 45.0f + angle) : (i * 90.0f + angle);

			vertices[i].pos.x = center.x + radius * cos(currAngle * PI / 180.0f);
			vertices[i].pos.y = center.y + radius * sin(currAngle * PI / 180.0f);
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
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * sizeof(unsigned int), shapeList[i].isIn ? indicesR : indices, GL_DYNAMIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
		}
	}

	void updateVBO(SpecialRt& shape, int index) {
		glBindVertexArray(VAOs[index]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[index]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, shape.vertices.size() * sizeof(ColoredVertex), shape.vertices.data());
	}

	void draw() {
		for (int i = 0; i < VAOs.size(); i++) {
			glBindVertexArray(VAOs[i]);
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		}
	}

};

Vertex bgColor = { 0.1f, 0.1f, 0.1f };
Vertex spinBase = { 0.0f, 0.0f, 0.0f };
Renderer renderer;
std::vector<SpecialRt> shapeList;
bool pause = false, clockwise = true;
GLfloat Inner = 0.1f, Outer = 0.14f, reverse = 0.02f;;

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
	srand((unsigned int)time(NULL));
	int randMax = rand() % 5 + 3; // 3 ~ 7
	for (int i = 0; i < randMax; i++) {
		if (rand() % 2 == 0) {
			shapeList.push_back({ {rand() / static_cast<float>(RAND_MAX) * 1.6f - 0.8f,
									rand() / static_cast<float>(RAND_MAX) * 1.6f - 0.8f, 0.0f},
									Inner, Outer, false });
		}
		else {
			shapeList.push_back({ {rand() / static_cast<float>(RAND_MAX) * 1.6f - 0.8f,
									rand() / static_cast<float>(RAND_MAX) * 1.6f - 0.8f, 0.0f},
									Inner, reverse, true });
		}
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
	glutMouseFunc(Mouse);
	glutTimerFunc(1000 / 60, Timer, clockwise);
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
		if (!clockwise || pause) {
			pause = false;
			clockwise = true;
			glutTimerFunc(1000 / 60, Timer, clockwise);
		}
		break;
	case 't':
		// 화면 가운데 중심 반시계방향 회전
		if (clockwise || pause) {
			pause = false;
			clockwise = false;
			glutTimerFunc(1000 / 60, Timer, clockwise);
		}
		break;
	case 's':
		// 애니메이션 일시정지
		pause = !pause;
		if (!pause) glutTimerFunc(1000 / 60, Timer, clockwise);
		break;
	case 'q':
		glutLeaveMainLoop();
		return;
	}
}

GLvoid Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		GLfloat xGL, yGL;
		mPosToGL(winWidth, winHeight, x, y, xGL, yGL);
		spinBase = { xGL, yGL, 0.0f };
	}
}

GLvoid Timer(int value) {
	if (pause || value != clockwise) return;
	for (auto& s : shapeList) {
		s.spin(spinBase, clockwise);
		s.breathing();
		renderer.updateVBO(s, &s - &shapeList[0]);
	}
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, Timer, clockwise);
}