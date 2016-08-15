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
	//setter
	void setMyImage(QImage i);
	void setMyPoint(QPoint p);
	void setZoomFactor(double z);
	void setZooming(bool z);

	//getter
	QImage MyImage();
	QPoint MyPoint();
	double ZoomFactor();
	bool Zooming();

	void addSquare(PersonSquare* ps);
	void clearVec();
	void moveRect();

protected:
	void paintEvent(QPaintEvent* e);
	void resizeEvent(QResizeEvent *event);

private:
	//Daten
	QImage myImage;
	QPoint myPoint;
	double zoomFactor;
	bool zooming;
	QVector<PersonSquare*> squareVec;

};

#endif // MYIMAGELABEL_H
