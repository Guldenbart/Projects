#include "imageinfo.h"

/**
 * @brief ImageInfo::ImageInfo basic constructor of class 'ImageInfo'.
 *
 * Initializes 'changed' and 'loaded' as false
 */
ImageInfo::ImageInfo(QString filename) : fileName(filename), changed(false), loaded(false)
{
}


/* ************************************************************************************************
 * GETTER
 * ************************************************************************************************
 */

QString ImageInfo::getFileName() const
{
	return this->fileName;
}


bool ImageInfo::hasChanged() const
{
	return this->changed;
}


QString ImageInfo::getDescription() const
{
	return this->description;
}


bool ImageInfo::isLoaded() const
{
	return this->loaded;
}


QVector<PersonSquare*> ImageInfo::getPersonSquares()
{
	return this->personSquares;
}


/* ************************************************************************************************
 * SETTER
 * ************************************************************************************************
 */

void ImageInfo::setFileName(const QString fileName)
{
	this->fileName = fileName;
}


void ImageInfo::setDescription(const QString description)
{
	this->description = description;
}


void ImageInfo::setChanged(const bool changed)
{
	this->changed = changed;
}


void ImageInfo::setLoaded(const bool loaded)
{
	this->loaded = loaded;
}


/* ************************************************************************************************
 * 'PERSONSQUARES'-METHODEN
 * ************************************************************************************************
 */
int ImageInfo::addPersonSquare(PersonSquare* personSquare)
{
	this->personSquares.push_back(personSquare);
	return this->personSquares.size() - 1;
	this->changed = true;
}


QString ImageInfo::getPersonSquaresString() const
{
	QString personSquaresString = "";
	for (int i = 0; i < this->personSquares.size(); i++) {
		personSquaresString += this->personSquares[i]->getX();
		personSquaresString += "%";
		personSquaresString += this->personSquares[i]->getY();
		personSquaresString += "%";
		personSquaresString += this->personSquares[i]->getName();
		personSquaresString += "$";
	}

	return personSquaresString;
}
