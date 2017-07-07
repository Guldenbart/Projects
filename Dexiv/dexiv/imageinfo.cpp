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


bool ImageInfo::getChanged() const
{
	return this->changed;
}


bool ImageInfo::getLoaded() const
{
	return this->loaded;
}


QVector<PersonSquare*>* ImageInfo::getPersonSquares()
{
	return &(this->personSquares);
}


/* ************************************************************************************************
 * SETTER
 * ************************************************************************************************
 */

void ImageInfo::setFileName(const QString fileName)
{
	this->fileName = fileName;
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
}
