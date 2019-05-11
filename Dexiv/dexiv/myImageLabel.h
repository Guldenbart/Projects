#ifndef MYIMAGELABEL_H
#define MYIMAGELABEL_H

#include <QtGui>
#include "protowindow.h"
#include "imageinfo.h"
#include "personSquare.h"

class PersonSquare;
class ImageInfo;

class MyImageLabel : public QLabel
{
public:
	MyImageLabel(QLabel* parent = 0);
	~MyImageLabel(); //<-TODO
	bool eventFilter(QObject *watched, QEvent *event);
	QSize sizeHint() const;

	//getter
	//QImage getMyImage();
	double getZoomFactor() const;

	//setter
	//void setMyImage(QImage i);
	void setZoomFactor(double factor);

	// PersonSquare-Methoden
	void addSquare(PersonSquare* personSquare);
	void addSquare(PersonSquare* personSquare, QString filename);
	void removeSquare(int index); //<-TODO
	void clearImageInfoVec();
	void moveRect();
	void processLine(QString line, QDir currentDir);

	// ImageInfo-Methoden
	void initImageInfoMap(QVector<QString>* vector);
	QString getMetaDataString(QString description); //<-TODO überarbeiten
	void setCurrentImageInfo(QString filename); //<-TODO überarbeiten
	bool hasInvalidCharacters();
	bool hasMetaDataChanged(QString currentDescription);
	QMap<QString, ImageInfo>::iterator* getCurrentImageInfoVec();

protected:
	void paintEvent(QPaintEvent* e);
	void resizeEvent(QResizeEvent *event);

private:

	// PersonSquare-Methoden
	/**
	 * @brief addToImageInfo Übergibt dem passenden ImageInfo den String mit PersonSquare-Informationen.
	 * @param line Der String mit PersonSquare-Informationen
	 */
	void addToImageInfo(QString filename, QString line);


	//Daten
	//QImage myImage;
	double zoomFactor;

	/**
	 * @brief imageInfoMap Für jedes Bild im aktuellen Verzeichnis einen Eintrag.
	 *
	 * key: Dateiname
	 * value: ImageInfo* mit Meta-Daten dieser Datei
	 */
	QMap<QString, ImageInfo> imageInfoMap; //<-TODO richtig anlegen und löschen, alle Funktionen zur Manipulation

	/**
	 * @brief curImageInfo Iterator auf den aktuellen Eintrag von 'imageInfoMap'
	 */
	QMap<QString, ImageInfo>::iterator curImageInfo; //<-TODO muss immer richtig gesetzt werden

};

#endif // MYIMAGELABEL_H
