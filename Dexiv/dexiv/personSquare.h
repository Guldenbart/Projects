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

/**
 * @brief The PersonSquare class is used to store information about a person.
 *
 * A PersonSquare is supposed to be used to tag a person in a picture.
 */
class PersonSquare : public QWidget
{
	Q_OBJECT

public:
	PersonSquare(QWidget* parent = 0, MyImageLabel* l = 0, int _x = 0, int _y = 0, QString _text = "");
	int getX();
	int getY();
	QString getName();
	void setX(int _x);
	void setY(int _y);
	QString packData();

private:
	//Zeiger auf ImageLabel
	MyImageLabel* l;

	/**
	 * @brief x X coordinate of the PersonSquare
	 */
	int x;

	/**
	 * @brief y Y coordinate of the PersonSquare
	 */
	int y;

	/**
	 * @brief description of the PersonSquare
	 */
	QString name;
};

#endif // PERSONSQUARE_H
