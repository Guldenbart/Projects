#include "myImageLabel.h"
#include <QTextItem>
#include <QDebug>

MyImageLabel::MyImageLabel(QLabel* parent)
		: QLabel(parent), zoomFactor(1.0)
{
	setAttribute(Qt::WA_StaticContents);
}


bool MyImageLabel::eventFilter(QObject *watched, QEvent *event)
{
	// This function repeatedly call for those QObjects
		// which have installed eventFilter (Step 2)

	qDebug() << "eventFilter aufgerufen";

	//if (watched->objectName().contains("personSquare")) {
	if (qobject_cast<PersonSquare*>(watched) != NULL) {
		if (event->type() == QEvent::Enter) {
			qDebug() << "Objekt erkannt";
			// Whatever you want to do when mouse goes over targetPushButton
			((PersonSquare*)watched)->setHover(true);
			this->update();
		}
		if (event->type() == QEvent::Leave) {
			((PersonSquare*)watched)->setHover(false);
			this->update();
		}
		return true;
	}else {
		// pass the event on to the parent class
		return QWidget::eventFilter(watched, event);
	}
}



void MyImageLabel::setMyImage(QImage _image)
{
	this->myImage = _image;
	//resizeImage(&this->myImage, this->myImage.size().expandedTo(size()));
	update();
}


void MyImageLabel::setZoomFactor(double factor)
{
	this->zoomFactor = factor;
}


QImage MyImageLabel::MyImage()
{
	return this->myImage;
}


double MyImageLabel::ZoomFactor()
{
	return this->zoomFactor;
}


void MyImageLabel::addSquare(PersonSquare* personSquare)
{
	personSquare->installEventFilter(this);
	this->squareVec.push_back(personSquare);
	personSquare->setIndex(this->squareVec.size());

	this->update();
}


void MyImageLabel::removeSquare(int index)
{
	this->squareVec.remove(index);
	this->update();
}

/*
 * löscht alle PersonSquares aus dem Vektor
 */
void MyImageLabel::clearSquares()
{
	this->squareVec.clear();
}

void MyImageLabel::moveRect()
{
	update();
}

void MyImageLabel::paintEvent(QPaintEvent* event)
{
	//qDebug() << "paintEvent() called";

	double fontCorrection = 1.0;
	if(zoomFactor > 1.0)
		fontCorrection = 0.8;
	else if(zoomFactor < 1.0)
		fontCorrection = 1.2;

	QLabel::paintEvent(event);
	QPainter painter(this);
	if (!painter.isActive()) {
		painter.begin(this);
	}

	//painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
	painter.setFont(QFont("Times", static_cast<int>(20.0 * (zoomFactor * fontCorrection)), 2));

	for(int i = 0; i < this->squareVec.size(); i++)
	{
		int _x = static_cast<int>(static_cast<double>((this->squareVec[i]->getX())-30)*zoomFactor);
		int _y = static_cast<int>(static_cast<double>((this->squareVec[i]->getY())-30)*zoomFactor);
		int _w = static_cast<int>(60.0 * zoomFactor);
		int _h = static_cast<int>(60.0 * zoomFactor);
		if (this->squareVec[i]->getHover()) {
			painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
		} else {
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		}
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
