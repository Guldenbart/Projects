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
	PersonSquare(QWidget* parent = 0, MyImageLabel* l = 0, int xCoord = 0, int yCoord = 0, QString _text = "");

	// getter
	int getX();
	int getY();
	QString getName();
	bool getHover();

	// setter
	void setX(int _x);
	void setY(int _y);
	void setName(const QString text);
	void setIndex(int i);
	void setHover(bool h);

private:
	//Zeiger auf ImageLabel
	MyImageLabel* label;

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

	/**
	 * @brief index of this PersonSquare in the QVector
	 */
	/*
	 * Wahrscheinlich wird der Index gebraucht, wenn das PersonSquare direkt ein Event bekommt,
	 * und man Änderungen vornehmen muss => dann muss man wissen, an welcher Stelle des QVectors
	 * in der ImageInfo dieses PersonSquare liegt.
	 */
	int index;

	/**
	 * @brief hover Tells if the mouse cursor is hovering over this PersonSquare
	 */
	bool hover;

protected:

};

#endif // PERSONSQUARE_H
