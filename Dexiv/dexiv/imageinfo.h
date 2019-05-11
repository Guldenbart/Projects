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
	QString getDescription() const;
	bool hasChanged() const;
	bool isLoaded() const;
	QVector<PersonSquare*> getPersonSquares();

	// setter:
	void setFileName(const QString fileName);
	void setDescription(const QString description);
	void setChanged(const bool changed);
	void setLoaded(const bool loaded);

	// 'personSquares'-Methoden
	// TODO: in cpp anpassen: Index, an dem der PersonSquare liegt, zurückgeben.
	int addPersonSquare(PersonSquare *personSquare);
	void removePersonSquare(int index);
	QString getPersonSquaresString() const;

private:

	// Daten
	/**
	 * @brief fileName voller Dateipfad (?) (oder nur Dateiname?)
	 */
	QString fileName;

	/**
	 * @brief description Allgemeine Beschreibung des Bilds, die im Textfeld unter dem Bild
	 * angezeigt wird.
	 */
	QString description;

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
