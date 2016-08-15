#include "personSquare.h"

PersonSquare::PersonSquare(QWidget* parent, MyImageLabel* _l, int _x, int _y)
	:QWidget(parent), l(_l), x(_x), y(_y)
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Name"),
													tr("Name eingeben:"), QLineEdit::Normal,
													"", &ok);
	if (ok && !text.isEmpty())
		this->name = text;

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

void PersonSquare::setX(int _x)
{
	this->x = _x;
}

void PersonSquare::setY(int _y)
{
	this->y = _y;
}

QString PersonSquare::packData()
{
	return "";
}
