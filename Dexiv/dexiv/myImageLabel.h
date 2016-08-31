#ifndef MYIMAGELABEL_H
#define MYIMAGELABEL_H

#include <QtGui>
#include "protowindow.h"
#include "personSquare.h"

class PersonSquare;

class MyImageLabel : public QLabel
{
public:
	MyImageLabel(QLabel* parent = 0);
	bool eventFilter(QObject *watched, QEvent *event);
	//setter
	void setMyImage(QImage i);
	void setZoomFactor(double factor);

	//getter
	QImage MyImage();
	double ZoomFactor();

	void addSquare(PersonSquare* personSquare);
	void removeSquare(int index);
	void clearSquares();
	void moveRect();

protected:
	void paintEvent(QPaintEvent* e);
	void resizeEvent(QResizeEvent *event);

private:
	//Daten
	QImage myImage;
	double zoomFactor;
	QVector<PersonSquare*> squareVec;

};

#endif // MYIMAGELABEL_H
