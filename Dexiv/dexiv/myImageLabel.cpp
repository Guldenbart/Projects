#include "myImageLabel.h"
#include <QTextItem>
#include <QDebug>

MyImageLabel::MyImageLabel(QLabel* parent)
		: QLabel(parent), zoomFactor(1.0)
{
	setAttribute(Qt::WA_StaticContents);
}


MyImageLabel::~MyImageLabel()
{
	/*
	for (auto e : this->imageInfoVec.keys()) {
		delete this->imageInfoVec.begin();
	}
	*/
	for (auto it = this->imageInfoVec.begin(); it != this->imageInfoVec.end(); /* don't increment here */) {
		it = this->imageInfoVec.erase(it);
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
 */
QImage MyImageLabel::getMyImage()
{
	return this->myImage;
}


double MyImageLabel::getZoomFactor()
{
	return this->zoomFactor;
}


/* ************************************************************************************************
 * SETTER
 * ************************************************************************************************
 */
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
	personSquare->installEventFilter(this);
	this->squareVec.push_back(personSquare);
	personSquare->setIndex(this->squareVec.size());

	this->update();

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
	ImageInfo* imageInfo = this->imageInfoVec.find(filename).value();
	int index = imageInfo->addPersonSquare(personSquare);

}


void MyImageLabel::removeSquare(int index)
{
	this->squareVec.remove(index);
	this->update();
}

/**
 * löscht alle PersonSquares aus dem Vektor
 * TODO prüfen, ob Dekonstruktoren aufgerufen werden. Wenn nicht, müssen alle Elemente einzeln abgebaut werden.
 * Evtl kann auf clear() überschrieben werden; dann kann die Funktionalität dieser Methode da rein
 */
void MyImageLabel::clearImageInfoVec()
{
	this->imageInfoVec.clear();
}


void MyImageLabel::moveRect()
{
	update();
}


/**
 * @brief MyImageLabel::processLine verarbeitet Meta-Daten einer Bild-Datei.
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
	QMap<QString, ImageInfo*>::iterator it = this->imageInfoVec.find(filename);

	// schauen, ob Dateiname schon vorhanden
	if (it == this->imageInfoVec.end()) {
		// wenn nicht, erstellen
		ImageInfo* imageInfo = new ImageInfo(filename);
		it = this->imageInfoVec.insert(filename, imageInfo);
	}

	ImageInfo* imageInfo = it.value();

	// (Zeile ab dem ersten '$')
	QString payloadString = line.section('$', 1, -1);
	int dollarCount = payloadString.count('$');

	for (int i=0; i<dollarCount+1; i++) {
		QString metaDataString = payloadString.section('$', i, i);
		int xCoord = metaDataString.section('%', 0, 0).toInt();
		int yCoord = metaDataString.section('%', 1, 1).toInt();
		QString text = metaDataString.section('%', 2, 2).toInt();

		PersonSquare* personSquare = new PersonSquare(0, this, xCoord, yCoord, text);
		//personSquare->installEventFilter(this);

		imageInfo->addPersonSquare(personSquare);
	}
}


/**
 * @brief MyImageLabel::setCurrentImageInfo Setzt curImageInfo auf den entsprechenden Eintrag im imageinfoVec.
 * @param filename Dateiname, nach dem gesucht wird.
 */
void MyImageLabel::setCurrentImageInfo(QString filename)
{
	// TODO überprüfen, ob das geht mit Zuweisung und Vergleich gleichzeitig.
	if (this->curImageInfo = this->imageInfoVec.find(filename) == this->imageInfoVec.end()) {

		// ImageInfo für Bild existiert noch nicht
		ImageInfo* imageInfo = new ImageInfo(filename);
		// TODO überprüfen, ob nach 'insert' der iterator richtig gesetzt wird.
		this->curImageInfo = this->imageInfoVec.insert(filename, imageInfo);
	}
}


void MyImageLabel::paintEvent(QPaintEvent* event)
{
	//qDebug() << "paintEvent() called";

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

	if (this->curImageInfo == this->imageInfoVec.end()) {
		// keine Metadaten für das aktuelle Bild vorhanden
		painter.end();
		return;
	}

	//painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
	painter.setFont(QFont("Times", static_cast<int>(20.0 * (this->zoomFactor * fontCorrection)), 2));

	QVector<PersonSquare*>* personSquares = this->curImageInfo.value()->getPersonSquares();
	for(int i = 0; i < personSquares->size(); i++)
	{
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
