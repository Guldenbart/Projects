#include "personSquare.h"

PersonSquare::PersonSquare(QWidget* parent, MyImageLabel* _l, int _x, int _y, QString _text)
	:QWidget(parent), l(_l), x(_x), y(_y), name(_text)
{
}

int PersonSquare::getX()
{
	return this->x;
}

int PersonSquare::getY()
{
	return this->y;
}

QString PersonSquare::getName()
{
	return this->name;
}

void PersonSquare::setX(int x)
{
	this->x = x;
}

void PersonSquare::setY(int y)
{
	this->y = y;
}

QString PersonSquare::packData()
{
	return "";
}
