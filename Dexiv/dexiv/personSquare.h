#ifndef PERSONSQUARE_H
#define PERSONSQUARE_H

#include <QtGui>
#include <QWidget>
#include <QInputDialog>
#include <QLineEdit>
#include <QPaintEvent>
#include "protowindow.h"
#include "myImageLabel.h"

class MyImageLabel;

class PersonSquare : public QWidget
{
	Q_OBJECT

public:
	PersonSquare(QWidget* parent = 0, MyImageLabel* l = 0, int _x = 0, int _y = 0);
	int getX();
	int getY();
	QString getName();
	void setX(int _x);
	void setY(int _y);
	QString packData();

private:
	//Zeiger auf ImageLabel
	MyImageLabel* l;

	//Koordinaten des Mittelpunkts
	int x;
	int y;

	//Beschriftung
	QString name;
};

#endif // PERSONSQUARE_H
