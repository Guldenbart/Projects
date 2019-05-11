#include "myImageLabel.h"
#include <QTextItem>
#include <QDebug>

MyImageLabel::MyImageLabel(QLabel* parent)
		: QLabel(parent), zoomFactor(1.0)
{
	setAttribute(Qt::WA_StaticContents);
	//this->curImageInfo = this->imageInfoMap.end();
}


MyImageLabel::~MyImageLabel()
{
	for (auto it = this->imageInfoMap.begin(); it != this->imageInfoMap.end(); /* don't increment here */) {
		it = this->imageInfoMap.erase(it);
		// TODO hier schauen, ob Destruktor aufgerufen wird!
	}
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


QSize MyImageLabel::sizeHint() const
{
	QSize retVal = QLabel::sizeHint();
	return retVal;
}


/* ************************************************************************************************
 * GETTER
 * ************************************************************************************************
 */ /*
QImage MyImageLabel::getMyImage()
{
	return this->myImage;
}
*/

double MyImageLabel::getZoomFactor() const
{
	return this->zoomFactor;
}


/* ************************************************************************************************
 * SETTER
 * ************************************************************************************************
 */ /*
void MyImageLabel::setMyImage(QImage _image)
{
	this->myImage = _image;

	QPixmap pixmap = QPixmap::fromImage(this->myImage);
	this->setPixmap(pixmap);

	update();
}
*/

void MyImageLabel::setZoomFactor(double factor)
{
	this->zoomFactor = factor;
}


/* ************************************************************************************************
 * 'PERSONSQUARES'-METHODEN
 * ************************************************************************************************
 */


/**
 * @brief MyImageLabel::addSquare Fügt dem aktuellen Bild ein PersonSquare hinzu.
 * Wird von protowindow aufgerufen, wenn das betroffene Bild gerade geöffnet/bearbeitet wird.
 * @param personSquare PersonSquare, der eingefügt werden soll.
 */
void MyImageLabel::addSquare(PersonSquare* personSquare)
{
	addSquare(personSquare, this->curImageInfo.value()->getFileName());
}


/**
 * @brief MyImageLabel::addSquare Fügt dem aktuellen Bild ein PersonSquare hinzu.
 * Wird von parseLine aufgerufen, wenn während dem Laden die Struktur aufgebaut wird.
 * @param personSquare PersonSquare, der eingefügt werden soll.
 * @param filename
 */
void MyImageLabel::addSquare(PersonSquare *personSquare, QString filename)
{
	personSquare->installEventFilter(this);
	ImageInfo* imageInfo = this->imageInfoMap.find(filename).value();
	int index = imageInfo->addPersonSquare(personSquare);
	personSquare->setIndex(index);

	// am Schluss; damit der neue Square auch angezeigt wird
	this->update();

}


/**
 * @brief MyImageLabel::removeSquare Löscht ein PersonSquare des aktuellen Bildes.
 * @param index Index eines ImageInfos, unter dem der zu Löschende PersonSquare liegt.
 */
void MyImageLabel::removeSquare(int index)
{
	// TODO
}

/**
 * löscht alle PersonSquares aus dem Vektor
 * TODO prüfen, ob Dekonstruktoren aufgerufen werden. Wenn nicht, müssen alle Elemente einzeln abgebaut werden.
 * Evtl kann auf clear() überschrieben werden; dann kann die Funktionalität dieser Methode da rein
 */
void MyImageLabel::clearImageInfoVec()
{
	this->imageInfoMap.clear();
}


void MyImageLabel::moveRect()
{
	update();
}


/**
 * @brief MyImageLabel::processLine verarbeitet Meta-Daten einer Bild-Datei
 * Der Einfachheit halber wird vorerst angenommen, dass immer nur neue Informationen eingelesen werden.
 * @param line QString, der Meta-Daten enthält.
 * @param currentDir Ordner-Pfad, in dem wir uns gerade befinden.
 */
void MyImageLabel::processLine(QString line, QDir currentDir)
{
	// Als erstes müssen die Rand-Symbole überprüft werden.
	if (line[0] != '&' || line[line.size()-1] != '&') {
		qDebug() << "Fehler beim Parsen: Falsche Kodierung der gesamten Zeile! [" << line << "]";
		return;
	}
	line = line.remove(line.length()-1, 1).remove(0,1);

	// Als nächstes muss der Dateiname gefunden werden
	QString filename = line.section('$', 0, 0);
	if (!QFile::exists(currentDir.absoluteFilePath(filename))) {
		qDebug() << "Fehler beim Parsen: Genannte Datei " << filename << " nicht im aktuellen Ordner " << currentDir << " vorhanden!";
		return;
	}

	// passendes ImageInfo suchen
	QMap<QString, ImageInfo>::iterator it = this->imageInfoMap.find(filename);

	// schauen, ob Dateiname schon vorhanden...
	if (it == this->imageInfoMap.end()) {
		// ... wenn nicht, erstellen
		ImageInfo* imageInfo = new ImageInfo(filename);
		it = this->imageInfoMap.insert(filename, imageInfo);
	}

	ImageInfo* imageInfo = it.value();

	// Zeile zwischen dem ersten und zweiten '$': Bildbeschreibung
	QString description = line.section('$', 1, 1);

	// Zeile ab dem zweiten '$': PersonSquares
	QString payloadString = line.section('$', 2, -1);
	int dollarCount = payloadString.count('$');
	imageInfo->setDescription(description);

	for (int i = 0; i < dollarCount+1; i++) {
		QString metaDataString = payloadString.section('$', i, i);
		int xCoord = metaDataString.section('%', 0, 0).toInt();
		int yCoord = metaDataString.section('%', 1, 1).toInt();
		QString text = metaDataString.section('%', 2, 2);

		PersonSquare* personSquare = new PersonSquare(0, this, xCoord, yCoord, text);
		//personSquare->installEventFilter(this);

		imageInfo->addPersonSquare(personSquare);
	}
}


void MyImageLabel::initImageInfoMap(QVector<QString>* vector)
{
	for (auto it = vector->constBegin(); it != vector->constEnd(); it++) {
		this->imageInfoMap.insert(QString(it->data()), new ImageInfo(""));
	}
}


/**
 * @brief MyImageLabel::getMetaDataString Erstellt aus allen registrierten Meta-Daten
 *		den String, der gespeichert werden soll.
 * @return String, der gespeichert werden soll.
 */
QString MyImageLabel::getMetaDataString(QString description)
{
	this->curImageInfo.value()->setDescription(description);

	QString metaData = "&";
	for (auto it = this->imageInfoMap.constBegin(); it != this->imageInfoMap.constEnd(); it++) {
		metaData += it.value()->getFileName();
		metaData += "$";
		metaData += it.value()->getDescription();
		metaData += "$";
		metaData += it.value()->getPersonSquaresString();
	}
	metaData += "&";

	return metaData;
}


/**
 * @brief MyImageLabel::setCurrentImageInfo Setzt curImageInfo auf den entsprechenden Eintrag im imageInfoMap.
 * Diese Methode sollte immter aufgerufen werden, wenn das Bild gewechselt wird. Damit weiß man beim Einfügen
 * und Auslesen von Meta-Daten immer schon, bei welchem Bild man gerade ist.
 * @param filename Dateiname, nach dem gesucht wird.
 */
void MyImageLabel::setCurrentImageInfo(QString filename)
{
	// TODO überprüfen, ob das geht mit Zuweisung und Vergleich gleichzeitig.
	if ((this->curImageInfo = this->imageInfoMap.find(filename)) == this->imageInfoMap.end()) {

		// ImageInfo für Bild existiert noch nicht
		ImageInfo* imageInfo = new ImageInfo(filename);
		// TODO überprüfen, ob nach 'insert' der iterator richtig gesetzt wird. (oder auf den Eintrag davor/danach)
		this->curImageInfo = this->imageInfoMap.insert(filename, imageInfo);
	}
}


/**
 * @brief MyImageLabel::hasInvalidCharacters überprüft, ob nicht zugelassene Zeichen vorhanden sind.
 * In der Beschreibung des Bildes dürfen die Zeichen
 * '&', '$', '%'
 * (vorerst) nicht vorkommen, das sie als "Steuerzeichen" für die Serialisierung verwendet werden.
 * @return true, wenn unzulässige Zeichen vorhanden sind; ansonsten false.
 */
bool MyImageLabel::hasInvalidCharacters()
{
	if (this->curImageInfo.value()->getDescription().contains('&')) {
		return true;
	}

	if (this->curImageInfo.value()->getDescription().contains('$')) {
		return true;
	}

	if (this->curImageInfo.value()->getDescription().contains('%')) {
		return true;
	}

	return false;
}


/**
 * @brief MyImageLabel::hasMetaDataChanged Stellt fest, ob sich die Meta-Daten des aktuellen Bildes geändert haben
 * @param currentDescription aktuelle Beschreibung des Bildes,
 * @return
 */
bool MyImageLabel::hasMetaDataChanged(QString currentDescription) {
	return this->curImageInfo.value()->hasChanged() || currentDescription != this->curImageInfo.value()->getDescription();
}

QMap<QString, ImageInfo*>::iterator* MyImageLabel::getCurrentImageInfoVec() {
	return &this->curImageInfo;
}


void MyImageLabel::paintEvent(QPaintEvent* event)
{
	//qDebug() << "paintEvent() called";

	if (this->imageInfoMap.empty()) {
		return;
	}
	if (this->curImageInfo == this->imageInfoMap.end()) {
		return;
	}

	double fontCorrection = 1.0;
	if(this->zoomFactor > 1.0)
		fontCorrection = 0.8;
	else if(this->zoomFactor < 1.0)
		fontCorrection = 1.2;

	QLabel::paintEvent(event);
	QPainter painter(this);
	if (!painter.isActive()) {
		painter.begin(this);
	}

	if (this->curImageInfo == this->imageInfoMap.end()) {
		// keine Metadaten für das aktuelle Bild vorhanden
		painter.end();
		return;
	}

	//painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
	painter.setFont(QFont("Times", static_cast<int>(20.0 * (this->zoomFactor * fontCorrection)), 2));

	QVector<PersonSquare*> personSquares = this->curImageInfo.value()->getPersonSquares();
	for (int i = 0; i < personSquares.size(); i++) {
		int _x = static_cast<int>(static_cast<double>((personSquares[i]->getX())-30)*this->zoomFactor);
		int _y = static_cast<int>(static_cast<double>((personSquares[i]->getY())-30)*this->zoomFactor);
		int _w = static_cast<int>(60.0 * this->zoomFactor);
		int _h = static_cast<int>(60.0 * this->zoomFactor);
		if (personSquares[i]->getHover()) {
			painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
		} else {
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		}
		painter.drawRect(_x, _y, _w, _h);
		const QString qs = personSquares[i]->getName();
		painter.drawText(_x-10, _y-10, qs);
	}
	painter.end();
}


// TODO kann weg, wenn nur Superklasse aufgerufen wird!
void MyImageLabel::resizeEvent(QResizeEvent* event)
{
	QLabel::resizeEvent(event);
}
