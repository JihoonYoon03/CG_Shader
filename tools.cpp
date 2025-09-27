#include "tools.h"

ColorRGB randColor() {
	ColorRGB color;
	color.r = rand() / static_cast<GLfloat>(RAND_MAX);
	color.g = rand() / static_cast<GLfloat>(RAND_MAX);
	color.b = rand() / static_cast<GLfloat>(RAND_MAX);
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

void mPosToGL(int mx, int my, GLfloat& xGL, GLfloat& yGL)
{
	xGL = (mx / 400.0f) - 1.0f;
	yGL = 1.0f - (my / 300.0f);
}

bool isMouseIn(rtPos& pos, int mx, int my)
{
	GLfloat xGL, yGL;
	mPosToGL(mx, my, xGL, yGL);

	if (xGL > pos.x1 && xGL < pos.x2 && yGL < pos.y1 && yGL > pos.y2) return true;
	else return false;
}