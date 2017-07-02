#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <QString>
#include "personSquare.h"

class PersonSquare;

class ImageInfo {

public:
	ImageInfo(QString filename);

	// getter:
	QString getFileName() const;
	bool getLoaded() const;
	QVector<PersonSquare*>* getPersonSquares();

	// setter:
	void setFileName(const QString fileName);
	void setLoaded(const bool loaded);

	// 'personSquares'-Methoden
	// TODO: in cpp anpassen: Index, an dem der PersonSquare liegt, zurückgeben.
	int addPersonSquare(PersonSquare *personSquare);

private:

	// Daten
	/**
	 * @brief fileName voller Dateipfad (?) (oder nur Dateiname?)
	 */
	QString fileName;

	/**
	 * @brief personSquares Liste aller PersonSquares.
	 */
	QVector<PersonSquare*> personSquares;

	/**
	 * @brief changed Gibt an, ob sich der Inhalt dieses Objekts geändert hat.
	 */
	bool changed;

	/**
	 * @brief loaded Gibt an, ob die Daten für dieses Bild geladen wurden.
	 */
	bool loaded;
};

#endif // IMAGEINFO_H
