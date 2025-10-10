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

//--- 필요한 변수 선언
GLint winWidth = 600, winHeight = 600;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

GLfloat shapeSizeOffset = 0.1f;

Vertex ColorTable[5] = {
	{1.0f, 1.0f, 0.0f},	// Yellow
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
	enum Shape { DOT = 0, LINE, TRIANGLE, RECTANGLE, PENTAGON };

private:
	friend class Renderer;

	Vertex center;
	GLfloat size;
	std::vector<ColoredVertex> vertices;	// 0: 좌하단, 1: 우하단, 2: 상단, 3: 좌상단, 4: 우상단

	GLfloat dx = 0.0f, dy = 0.0f;
	Shape currentShape;
	unsigned int sleepFrame = 0;


public:
	ShapeObject(Shape shape, Vertex& center, GLfloat& size) : center(center), currentShape(shape), size(size) {
		switch (shape) {
		case DOT:
			// 중앙
			vertices.push_back({ center.x, center.y, 0.0f, ColorTable[shape] });
			break;
		case LINE:
			// 좌하, 우상
			vertices.push_back({ center.x - size, center.y - size, 0.0f, ColorTable[shape] });
			vertices.push_back({ center.x + size, center.y + size, 0.0f, ColorTable[shape] });
			break;
		case TRIANGLE:
			// 좌하, 우하, 중앙
			vertices.push_back({ center.x - size, center.y - size, 0.0f, ColorTable[shape] });
			vertices.push_back({ center.x + size, center.y - size, 0.0f, ColorTable[shape] });
			vertices.push_back({ center.x, center.y + size, 0.0f, ColorTable[shape] });
			break;
		case RECTANGLE:
			// 좌상, 좌하, 우하, 우상
			vertices.push_back({ center.x - size, center.y + size, 0.0f, ColorTable[shape] });
			vertices.push_back({ center.x - size, center.y - size, 0.0f, ColorTable[shape] });
			vertices.push_back({ center.x + size, center.y - size, 0.0f, ColorTable[shape] });
			vertices.push_back({ center.x + size, center.y + size, 0.0f, ColorTable[shape] });
			break;
		case PENTAGON:
			// 좌하, 우하, 중앙, 좌상, 우상
			vertices.push_back({ center.x - size * 0.65f, center.y - size, 0.0f, ColorTable[shape] });
			vertices.push_back({ center.x + size * 0.65f, center.y - size, 0.0f, ColorTable[shape] });
			vertices.push_back({ center.x, center.y + size * 1.25f, 0.0f, ColorTable[shape] });
			vertices.push_back({ center.x - size, center.y + size * 0.4f, 0.0f, ColorTable[shape] });
			vertices.push_back({ center.x + size, center.y + size * 0.4f, 0.0f, ColorTable[shape] });
			break;
		}
	}

	// 클릭 여부 체크
	bool clickCheck(GLfloat xGL, GLfloat yGL) {
		switch (currentShape) {
		case DOT:
			return CircleCollider(center, 0.05f, xGL, yGL);
		case LINE:
			return LineCollider(vertices[0].pos, vertices[1].pos, size * 0.25f, xGL, yGL);
		case TRIANGLE:
			return CircleCollider(center, size * 1.25f, xGL, yGL);
		case RECTANGLE:
			return RectCollider(vertices[0].pos, vertices[2].pos, xGL, yGL);
		case PENTAGON:
			return CircleCollider(center, size * 1.25f, xGL, yGL);
		}
	}

	void dragTo(GLfloat xGL, GLfloat yGL) {
		dx = xGL - center.x;
		dy = yGL - center.y;
		center.x = xGL;
		center.y = yGL;
		for (auto& v : vertices) {
			v.pos.x += dx;
			v.pos.y += dy;
		}
	}

	void absorb(ShapeObject& other) {
		// 점 개수 기준 값
		int sumShape = ((currentShape + 1) + (other.currentShape + 1));
		if (sumShape > 5) sumShape = 1;

		// 인덱스화 (-1)
		currentShape = static_cast<Shape>(sumShape - 1);

		center = { (center.x + other.center.x) * 0.5f, (center.y + other.center.y) * 0.5f, 0.0f };

		vertices.clear();
		switch (currentShape) {
		case DOT:
			// 중앙
			vertices.push_back({ center.x, center.y, 0.0f, ColorTable[currentShape] });
			break;
		case LINE:
			// 좌하, 우상
			vertices.push_back({ center.x - size, center.y - size, 0.0f, ColorTable[currentShape] });
			vertices.push_back({ center.x + size, center.y + size, 0.0f, ColorTable[currentShape] });
			break;
		case TRIANGLE:
			// 좌하, 우하, 중앙
			vertices.push_back({ center.x - size, center.y - size, 0.0f, ColorTable[currentShape] });
			vertices.push_back({ center.x + size, center.y - size, 0.0f, ColorTable[currentShape] });
			vertices.push_back({ center.x, center.y + size, 0.0f, ColorTable[currentShape] });
			break;
		case RECTANGLE:
			// 좌상, 좌하, 우하, 우상
			vertices.push_back({ center.x - size, center.y + size, 0.0f, ColorTable[currentShape] });
			vertices.push_back({ center.x - size, center.y - size, 0.0f, ColorTable[currentShape] });
			vertices.push_back({ center.x + size, center.y - size, 0.0f, ColorTable[currentShape] });
			vertices.push_back({ center.x + size, center.y + size, 0.0f, ColorTable[currentShape] });
			break;
		case PENTAGON:
			// 좌하, 우하, 중앙, 좌상, 우상
			vertices.push_back({ center.x - size * 0.65f, center.y - size, 0.0f, ColorTable[currentShape] });
			vertices.push_back({ center.x + size * 0.65f, center.y - size, 0.0f, ColorTable[currentShape] });
			vertices.push_back({ center.x, center.y + size * 1.25f, 0.0f, ColorTable[currentShape] });
			vertices.push_back({ center.x - size, center.y + size * 0.4f, 0.0f, ColorTable[currentShape] });
			vertices.push_back({ center.x + size, center.y + size * 0.4f, 0.0f, ColorTable[currentShape] });
			break;
		}
	}

	Shape getShape() {	return currentShape;	}
	Vertex getCenter() { return center; }
};

class Renderer {
	std::vector<GLuint> VAOs, VBOs, EBOs;

public:
	Renderer() {
		VAOs.resize(15);
		VBOs.resize(15);
		EBOs.resize(15);
	}

	// VAO, VBO, EBO 초기화 및 데이터 연결
	void begin(std::vector<ShapeObject>& shapeList) {
		for (int i = 0; i < shapeList.size(); i++) {
			glGenVertexArrays(1, &VAOs[i]);
			glGenBuffers(1, &VBOs[i]);
			glGenBuffers(1, &EBOs[i]);

			glBindVertexArray(VAOs[i]);
			glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
			glBufferData(GL_ARRAY_BUFFER, 5 * sizeof(ColoredVertex), shapeList[i].vertices.data(), GL_DYNAMIC_DRAW);

			if (shapeList[i].currentShape == ShapeObject::RECTANGLE || shapeList[i].currentShape == ShapeObject::PENTAGON) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, 9 * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);
			}

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (void*)(sizeof(Vertex)));
			glEnableVertexAttribArray(1);
		}
	}

	void updatePos(ShapeObject& shape, int index, bool updateEBO = false) {
		glBindVertexArray(VAOs[index]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[index]);
		glBufferData(GL_ARRAY_BUFFER, shape.vertices.size() * sizeof(ColoredVertex), shape.vertices.data(), GL_DYNAMIC_DRAW);

		if (updateEBO) {
			if (shape.getShape() == ShapeObject::RECTANGLE || shape.getShape() == ShapeObject::PENTAGON) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[index]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, 9 * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);
			}
			else {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
		}
	}

	void draw(std::vector<ShapeObject>& shapeList) {
		for (int i = 0; i < shapeList.size(); i++) {
			glBindVertexArray(VAOs[i]);
			switch (shapeList[i].currentShape) {
			case ShapeObject::DOT:
				glPointSize(10.0f);
				glDrawArrays(GL_POINTS, 0, 1);
				break;
			case ShapeObject::LINE:
				glLineWidth(2.0f);
				glDrawArrays(GL_LINE_STRIP, 0, 2);
				break;
			case ShapeObject::TRIANGLE:
				glDrawArrays(GL_TRIANGLES, 0, 3);
				break;
			case ShapeObject::RECTANGLE:
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				break;
			case ShapeObject::PENTAGON:
				glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
				break;
			}
		}
	}

	void deleteData(int index) {
		glDeleteVertexArrays(1, &VAOs[index]);
		glDeleteBuffers(1, &VBOs[index]);
		glDeleteBuffers(1, &EBOs[index]);

		VAOs.erase(VAOs.begin() + index);
		VBOs.erase(VBOs.begin() + index);
		EBOs.erase(EBOs.begin() + index);
	}
};

Vertex bgColor = { 0.1f, 0.1f, 0.1f };
Renderer renderer;
std::vector<ShapeObject> shapeList;
bool dragging = false;
int dragIndex = -1;

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
	for (int i = 0; i < 15; i++) {
		Vertex center = { rand() / static_cast<float>(RAND_MAX) * 2.0f - 1.0f,
							rand() / static_cast<float>(RAND_MAX) * 2.0f - 1.0f,
							rand() / static_cast<float>(RAND_MAX) * 2.0f - 1.0f };

		ShapeObject::Shape shape = static_cast<ShapeObject::Shape>(i % 5);

		shapeList.push_back({ shape, center, shapeSizeOffset });
	}
	renderer.begin(shapeList);


	//--- 세이더 읽어와서 세이더 프로그램 만들기: 사용자 정의함수 호출
	make_vertexShaders(vertexShader, "vertex.glsl"); //--- 버텍스 세이더 만들기
	make_fragmentShaders(fragmentShader, "fragment.glsl"); //--- 프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram(vertexShader, fragmentShader);

	//--- 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();
}

//--- 출력 콜백 함수
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	renderer.draw(shapeList);

	glutSwapBuffers(); // 화면에 출력하기
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			if (!dragging) {
				GLfloat xGL, yGL;
				mPosToGL(winWidth, winHeight, x, y, xGL, yGL);
				for (int i = shapeList.size() - 1; i >= 0; i--) {
					if (!CircleCollider(shapeList[i].getCenter(), shapeSizeOffset * 1.5f, xGL, yGL)) continue;
					if (shapeList[i].clickCheck(xGL, yGL)) {
						dragging = true;
						dragIndex = i;
						break;
					}
				}
			}
		}
		else if (state == GLUT_UP) {
			if (dragIndex == -1) break;
			Vertex dragCenter = shapeList[dragIndex].getCenter();
			int combined = dragIndex, absorbed = -1;
			
			for (int i = shapeList.size() - 1; i >= 0; i--) {
				if (i == dragIndex || !CircleCollider(shapeList[i].getCenter(), shapeSizeOffset * 3.0f, dragCenter.x, dragCenter.y)) continue;
				combined = std::min(i, dragIndex);
				absorbed = std::max(i, dragIndex);
				break;
			}


			if (absorbed != -1) {
				// 충돌 시 모양 변경 후 흡수된 도형 삭제
				shapeList[combined].absorb(shapeList[absorbed]);
				renderer.deleteData(absorbed);
				shapeList.erase(shapeList.begin() + absorbed);
				renderer.updatePos(shapeList[combined], combined, true);
			}

			dragging = false;
			dragIndex = -1;
		}
		break;
	}
}

GLvoid MouseMotion(int x, int y) {
	if (dragging && dragIndex != -1) {
		GLfloat xGL, yGL;
		mPosToGL(winWidth, winHeight, x, y, xGL, yGL);

		if (xGL < -1.0f) xGL = -1.0f;
		else if (xGL > 1.0f) xGL = 1.0f;
		if (yGL < -1.0f) yGL = -1.0f;
		else if (yGL > 1.0f) yGL = 1.0f;

		shapeList[dragIndex].dragTo(xGL, yGL);
		renderer.updatePos(shapeList[dragIndex], dragIndex);
		glutPostRedisplay();
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