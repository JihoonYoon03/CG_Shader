#include "tools.h"

Vertex randColor() {
	Vertex color;
	color.x = rand() / static_cast<GLfloat>(RAND_MAX);
	color.y = rand() / static_cast<GLfloat>(RAND_MAX);
	color.z = rand() / static_cast<GLfloat>(RAND_MAX);
	return color;
}

rtPos randSquarePos(GLfloat offset) {
	rtPos pos;
	pos.x1 = rand() / static_cast<GLfloat>(RAND_MAX) * 2.0f - 1.0f;
	pos.y1 = rand() / static_cast<GLfloat>(RAND_MAX) * 2.0f - 1.0f;

	pos.x2 = pos.x1 + offset;
	pos.y2 = pos.y1 - offset;
	return pos;
}

rtPos randRectPos(GLfloat offset) {
	rtPos pos;
	pos.x1 = rand() / static_cast<GLfloat>(RAND_MAX) * 2.0f - 1.0f;
	pos.y1 = rand() / static_cast<GLfloat>(RAND_MAX) * 2.0f - 1.0f;

	pos.x2 = pos.x1 + (rand() / static_cast<GLfloat>(RAND_MAX) * 0.1f + offset);
	pos.y2 = pos.y1 - (rand() / static_cast<GLfloat>(RAND_MAX) * 0.1f + offset);
	return pos;
}

void mPosToGL(GLuint winWidth, GLuint winHeight, int mx, int my, GLfloat& xGL, GLfloat& yGL)
{
	xGL = (mx / (winWidth / 2.0f)) - 1.0f;
	yGL = 1.0f - (my / (winHeight / 2.0f));
}

bool isMouseIn(rtPos& pos, GLuint winWidth, GLuint winHeight, int mx, int my)
{
	GLfloat xGL, yGL;
	mPosToGL(winWidth, winHeight, mx, my, xGL, yGL);

	if (xGL > pos.x1 && xGL < pos.x2 && yGL < pos.y1 && yGL > pos.y2) return true;
	else return false;
}

bool CircleCollider(Vertex& center, GLfloat distCap, GLfloat xGL, GLfloat yGL) {
	GLfloat dist = sqrt((center.x - xGL) * (center.x - xGL) + (center.y - yGL) * (center.y - yGL));
	if (dist < distCap) {
		return true;
	}
	return false;
}

bool LineCollider(Vertex& p1, Vertex& p2, GLfloat distCap, GLfloat xGL, GLfloat yGL) {
	GLfloat m;
	m = (p2.y - p1.y) / (p2.x - p1.x);	// 기울기

	// y - y1 = m(x - x1)
	GLfloat xOnLine = (yGL - p1.y) / m + p1.x; // y좌표에 대응하는 x좌표
	GLfloat yOnLine = m * (xGL - p1.x) + p1.y; // x좌표에 대응하는 y좌표

	GLfloat minX = std::min(p1.x, p2.x) - distCap * 0.5f;
	GLfloat maxX = std::max(p1.x, p2.x) + distCap * 0.5f;
	GLfloat minY = std::min(p1.y, p2.y) - distCap * 0.5f;
	GLfloat maxY = std::max(p1.y, p2.y) + distCap * 0.5f;

	// 선분 범위 내에 있는지 확인
	if (xGL < minX || xGL > maxX || yGL < minY || yGL > maxY) {
		return false;
	}

	if (abs(xOnLine - xGL) < distCap && abs(yOnLine - yGL) < distCap) {
		return true;
	}

	return false;
}

bool RectCollider(Vertex& p1, Vertex& p2, GLfloat xGL, GLfloat yGL) {
	GLfloat minX = std::min(p1.x, p2.x);
	GLfloat maxX = std::max(p1.x, p2.x);
	GLfloat minY = std::min(p1.y, p2.y);
	GLfloat maxY = std::max(p1.y, p2.y);

	if (xGL > minX && xGL < maxX && yGL > minY && yGL < maxY) return true;
	else return false;
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fopen_s(&fptr, file, "rb");		// Open file for reading
	if (!fptr)						// Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END);		// Seek to the end of the file
	length = ftell(fptr);			// Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET);		// Go back to the beginning of the file
	fread(buf, length, 1, fptr);	// Read the contents of the file in to the buffer
	fclose(fptr);					// Close the file
	buf[length] = 0;				// Null terminator
	return buf;						// Return the buffer
}

void make_vertexShaders(GLuint& vertexShader, const std::string& shaderName)
{
	GLchar* vertexSource;
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
	vertexSource = filetobuf(shaderName.data());
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders(GLuint& fragmentShader, const std::string& shaderName)
{
	GLchar* fragmentSource;
	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentSource = filetobuf(shaderName.data()); // 프래그세이더 읽어오기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

GLuint make_shaderProgram(const GLuint& vertexShader, const GLuint& fragmentShader)
{
	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기
	glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기
	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID); //--- 만들어진 세이더 프로그램 사용하기
	//--- 여러 개의 세이더프로그램 만들 수 있고, 그 중 한개의 프로그램을 사용하려면
	//--- glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
	//--- 사용하기 직전에 호출할 수 있다.
	return shaderID;
}