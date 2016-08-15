#include "myImageLabel.h"
#include <QTextItem>

MyImageLabel::MyImageLabel(QLabel* parent)
		: QLabel(parent), zoomFactor(1.0), zooming(false)
{
	setAttribute(Qt::WA_StaticContents);
}

void MyImageLabel::setMyImage(QImage _image)
{
	this->myImage = _image;
	//resizeImage(&this->myImage, this->myImage.size().expandedTo(size()));
	update();
}

void MyImageLabel::setMyPoint(QPoint p)
{
	this->myPoint = p;
}

void MyImageLabel::setZoomFactor(double _z)
{
	this->zoomFactor = _z;
}

void MyImageLabel::setZooming(bool z)
{
	this->zooming = z;
}

QImage MyImageLabel::MyImage()
{
	return this->myImage;
}

QPoint MyImageLabel::MyPoint()
{
	return this->myPoint;
}

double MyImageLabel::ZoomFactor()
{
	return this->zoomFactor;
}

bool MyImageLabel::Zooming()
{
	return this->zooming;
}

void MyImageLabel::addSquare(PersonSquare* ps)
{
	this->squareVec.push_back(ps);
}

/*
 * löscht alle PersonSquares aus dem Vektor
 */
void MyImageLabel::clearVec()
{
	this->squareVec.clear();
}

void MyImageLabel::moveRect()
{
	this->zooming = true;
	update();
}

void MyImageLabel::paintEvent(QPaintEvent* event)
{
	double fontCorrection = 1.0;
	if(zoomFactor > 1.0)
		fontCorrection = 0.8;
	else if(zoomFactor < 1.0)
		fontCorrection = 1.2;
	QLabel::paintEvent(event);
	QPainter painter(this);
	painter.begin(this);
	painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
	painter.setFont(QFont("Times", static_cast<int>(20.0 * (zoomFactor * fontCorrection)), 2));

	for(int i = 0; i < this->squareVec.size(); i++)
	{
		int _x = static_cast<int>(static_cast<double>((this->squareVec[i]->getX())-30)*zoomFactor);
		int _y = static_cast<int>(static_cast<double>((this->squareVec[i]->getY())-30)*zoomFactor);
		int _w = static_cast<int>(60.0 * zoomFactor);
		int _h = static_cast<int>(60.0 * zoomFactor);
		painter.drawRect(_x, _y, _w, _h);
		const QString qs = this->squareVec[i]->getName();
		painter.drawText(_x-10, _y-10, qs);
	}
	painter.end();
}

void MyImageLabel::resizeEvent(QResizeEvent* event)
{
	QLabel::resizeEvent(event);
}
