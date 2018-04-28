#include "personSquare.h"

PersonSquare::PersonSquare(QWidget* parent, MyImageLabel* l, int xCoord, int yCoord, QString _text)
	:QWidget(parent), label(l), x(xCoord), y(yCoord), name(_text)
{
	this->hover = false;
}

/* ************************************************************************************************
 * GETTER
 * ************************************************************************************************
 */

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


bool PersonSquare::getHover()
{
	return this->hover;
}


/* ************************************************************************************************
 * SETTER
 * ************************************************************************************************
 */


void PersonSquare::setX(int x)
{
	this->x = x;
}


void PersonSquare::setY(int y)
{
	this->y = y;
}


void PersonSquare::setName(const QString text)
{
	this->name = text;
}


void PersonSquare::setIndex(int i)
{
	this->index = i;
}


void PersonSquare::setHover(bool h)
{
	this->hover = h;
}

