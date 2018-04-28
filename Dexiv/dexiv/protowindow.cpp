#include "protowindow.h"
#include "ui_protowindow.h"
//#include "exiv2/exiv2.hpp"
#include <fstream>

/*
 * Konstruktor
 */
ProtoWindow::ProtoWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ProtoWindow)
{
	ui->setupUi(this);
    //delete ui->mainToolBar;

	ui->backPushButton->setDisabled(true);
	ui->forwardPushButton->setDisabled(true);
	ui->showPushButton->setDisabled(true);
	ui->rotateLeftPushButton->setDisabled(true);
	ui->rotateRightPushButton->setDisabled(true);

	this->imageLabel = new MyImageLabel;
	this->imageLabel->setBackgroundRole(QPalette::Base);
	//this->imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	this->imageLabel->setScaledContents(true);

	ui->scrollArea->setBackgroundRole(QPalette::Dark);
	ui->scrollArea->setWidgetResizable(false);
	ui->scrollArea->setWidget(imageLabel);

	this->currentIndex = -1;
	this->scaleFactor = 1.0;
	this->curDir = QDir::current();
	this->isInverted = false;
	this->rotateChanged = 0;
	this->metaDataChanged = false;
	this->isAutoNormalSize = true;

	this->showMaximized();

	createActions();
	createMenus();

	setWindowTitle(trUtf8("EXIV Viewer"));
}

/*
 * Destruktor
 */
ProtoWindow::~ProtoWindow()
{
	delete ui;
}

/*
 * Kopiert die Argumente vom Starten des Programms in die Klasse.
 */
void ProtoWindow::setArgs(int _argc, char** _argv)
{
	this->argc = _argc;
	this->argv = _argv;
}

/*
 * trägt alle Dateinamen von jpeg-Dateien des aktuellen Ordners in einen Vektor ein.
 */
void ProtoWindow::setDirVec()
{
	// Filter erstellen, der nur jp(e)g-, png- und bmp-Dateien berücksichtigt.
	QStringList filters;
	filters << "*.jpg" << "*.jpeg" << "*.JPG" << "*.JPEG" << "*.png" << "*.PNG" << ".bmp" << ".BMP";

	// Liste aller Dateien mit im Filter angegebenen Dateinamen erstellen
	QFileInfoList _infoList = this->curDir.entryInfoList(filters);

	// Vektor erstellen, in den alle Bilddateien mit ihrem absoluten Pfad eingetragen werden.
	for(int in = 0; in < _infoList.length(); in++) {
		this->dirVec.push_back(_infoList.at(in).fileName());
	}
}

/*
 * wurde das Programm durch Doppelklick auf eine jp(e)g-Datei geöffnet, muss gleich dieses
 * Bild angezeigt werden.
 *
 * wird z.Zt. nicht weiter entwickelt
 */
void ProtoWindow::setFirstImage()
{
	QString q = QString::fromStdString(this->argv[1]);
	setCurrentIndex(q);

	// Bild anzeigen

	QString fileName = (this->curDir.filePath(this->dirVec[this->currentIndex]));
	// Dateiname sollte nicht leer sein...
	if (showNextImage(fileName)) {

		ui->backPushButton->setEnabled(true);
		ui->forwardPushButton->setEnabled(true);
		ui->showPushButton->setEnabled(true);
	}
}

void ProtoWindow::onNewFolder(QDir newDir)
{
	/* Altes löschen: */
	this->dirVec.clear();
	// TODO auch in imageLabel

	/* Neu aufbauen: */
	this->curDir = newDir;
	setDirVec();

	/* Meta-Daten lesen */
	QFile inputFile(this->curDir.absoluteFilePath("dexiv.txt"));
	if (inputFile.open(QIODevice::ReadOnly)) {
		QTextStream in(&inputFile);

		while (!in.atEnd()) {
			QString line = in.readLine();
			this->imageLabel->processLine(line, this->curDir);
		}
		inputFile.close();
	}
}

void ProtoWindow::onNewImage()
{
	// Label leeren (d.h. Bild und PersonSquares löschen)
	this->imageLabel->clear();
	this->imageLabel->clearImageInfoVec();

	this->scaleFactor = 1.0;
	this->resetImageChanged();
	this->metaDataChanged = false;
}

/*
 * findet heraus, welchen Index das als geoeffnete Bild hat.
 * TODO in ImageLabel setzen, welches Bild das aktuelle ist
 */
void ProtoWindow::setCurrentIndex(QString filename)
{
	// '\' durch '/' ersetzen:
	for (int i = 0; i < filename.size(); i++) {
		if(filename[i] == '\\')
			filename[i] = '/';
	}

	// Durch den Vektor mit allen Dateinamen gehen und mit 'filename' abgleichen.
	for (int i = 0; i < this->dirVec.size(); i++) {
		if (curDir.filePath(this->dirVec[i]) == filename) {
			this->currentIndex = i;
			return;
		}
	}
	 // Falls der Dateiname nicht gefunden wurde... (was wahrscheinlich nur durch einen
	 // Fehler an einer anderen Stelle im Programm passieren kann)
	this->currentIndex = 0;
}

/*
 * Bild der Fenstergöße anpassen; bzw nach einem Zoomvorgang die Bildgröße
 * entsprechend verändern
 */
void ProtoWindow::scaleImage(double factor)
{
	//Q_ASSERT(imageLabel->pixmap());
	this->scaleFactor *= factor;
	this->imageLabel->setZoomFactor(scaleFactor);

	//QImage newImage = imageLabel->pixmap()->toImage();
	//const QSize newSize(static_cast<int>(static_cast<double>(newImage.width()) * factor), (static_cast<int>(static_cast<double>(newImage.height()) * factor)));
	//newImage = newImage.scaled(newSize);
	//imageLabel->setMyImage(newImage);
	imageLabel->resize(this->imageLabel->pixmap()->size() * this->scaleFactor);

	adjustScrollBar(ui->scrollArea->horizontalScrollBar(), factor);
	adjustScrollBar(ui->scrollArea->verticalScrollBar(), factor);
}

/*
 * Bereits vorgegebene Funktion, die die Größe der Scrollbars der Fenstergröße anpasst
 */
void ProtoWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
	scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

/*
 * Funktion, die genau den Faktor zurückgibt, der benötigt wird, um das Bild
 * im unveränderten Seitenverhältnis an die ScrollArea-Größe anzupassen
 */
double ProtoWindow::fittingSize(QString from)
{
	qDebug() << "########################################";
	qDebug() << "from " << from;

	if (!this->isAutoNormalSize) {
		// keine Anpassung gewünscht
		return 1.0;
	}

	const QPixmap* pixmap = this->imageLabel->pixmap();
	if (pixmap == 0 || this->currentIndex == -1) {
		qDebug() << "RESULT: 1.0 (no Image)";
		return 1.0;
	}

	QSize labelSize = this->imageLabel->size();
	QSize areaSize = ui->scrollArea->size();

	double labelSize_h = static_cast<double>(labelSize.height());
	double labelSize_w = static_cast<double>(labelSize.width());
	double areaSize_h = static_cast<double>(areaSize.height());
	double areaSize_w = static_cast<double>(areaSize.width());

	qDebug() << "label h: " << labelSize_h;
	qDebug() << "label w: " << labelSize_w;
	qDebug() << "area h: " << areaSize_h;
	qDebug() << "area w: " << areaSize_w;

	if (labelSize_h < areaSize_h && labelSize_w < areaSize_w) {
		double imageSize_h = pixmap->height();
		double imageSize_w = pixmap->width();

		if (imageSize_h <= areaSize_h && imageSize_w <= areaSize_w) {
			return 1/this->scaleFactor;
		} else if (imageSize_h > areaSize_h && imageSize_w <= areaSize_w) {
			return areaSize_h/labelSize_h;
		} else if (imageSize_h <= areaSize_h && imageSize_w > areaSize_w) {
			return areaSize_w/labelSize_w;
		} else if (imageSize_h > areaSize_h && imageSize_w > areaSize_w){
			if (((imageSize_h/imageSize_w)/(areaSize_h/areaSize_w)) > 1) {
				return (areaSize_h / labelSize_h)-0.001;
			} else {
				return (areaSize_w / labelSize_w)-0.001;
			}
		} else {
			// Bild ist sowieso kleiner als ScrollArea
			qDebug() << "RESULT: 1.0 (Bild ist sowieso kleiner als ScrollArea)";
			return 1.0;
		}
	}

	if (((labelSize_h/labelSize_w)/(areaSize_h/areaSize_w)) > 1) {
		// Höhe ist entscheidender Faktor
		qDebug() << "RESULT: " << (areaSize_h / labelSize_h)-0.001 << "(Höhe ist entscheidender Faktor - label)";
		return (areaSize_h / labelSize_h)-0.001;
	} else {
		// Weite ist entscheidender Faktor
		qDebug() << "RESULT: " << (areaSize_w / labelSize_w)-0.001 << "(Weite ist entscheidender Faktor - label)";
		return (areaSize_w / labelSize_w)-0.001;
	}
}


bool ProtoWindow::imageChanged()
{
	return this->isInverted || this->rotateChanged % 360 != 0;
}


void ProtoWindow::resetImageChanged()
{
	this->isInverted = false;
	this->rotateChanged = 0;
}


/**
 * Gemeinsame Funktion von open, on_backPushButton_clicked und on_forwardPushButton_clicked.
 *
 * sorgt dafür, dass Bild 'fileName' angezeigt wird.
 *
 * @return true, wenn das Bild geladen werden konnte, ansonsten false.
 */
bool ProtoWindow::showNextImage(QString fileName)
{
	// TODO: sollte eig nicht mehr vorkommen, weil in open() schon abgefangen; und in den anderen Funktionen sollte fileName nie leer werden können
	if (fileName.isEmpty()) {
		return false;
	}

	onNewImage();

	QImage image(fileName);
	if (image.isNull()) {
		 QMessageBox::information(this, trUtf8("EXIV Viewer"), trUtf8("%1 konnte nicht geladen werden.").arg(fileName));
		 return false;
	}

	//this->imageLabel->setMyImage(image);
	this->imageLabel->setPixmap(QPixmap::fromImage(image));

	scaleImage(fittingSize("showNextImage"));

	this->imageLabel->setCurrentImageInfo(fileName);

	return true;
}


/*
 * Überprüft, ob die Datei, die als String übergeben wird, eine Bilddatei
 * im dirVec ist. falls ja, wird true zurückgeliefert und der Index im dirVec
 * unter fileNumber gespeichert. Falls nicht, wird false zurückgeliefert.
 *//*
bool ProtoWindow::findFile(QString content, int& fileNumber)
{
	for (int i = 0; i < this->dirVec.size(); i++) {
		if (content == this->dirVec[i]) {
			fileNumber = i;
			return true;
		}
	}
	return false;
}
*/


void ProtoWindow::cleanUp()
{
	//delete stringArray;

}

/* #################################################################################################
 * ## Menü- und Button-Funktionen
 * #################################################################################################
 */

/*
 * Menüs erstellen
 */
void ProtoWindow::createMenus()
{
	fileMenu = new QMenu(trUtf8("&Datei"), this);
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveAsAct);
	//fileMenu->addAction(printAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	viewMenu = new QMenu(trUtf8("&Ansicht"), this);
	viewMenu->addAction(zoomInAct);
	viewMenu->addAction(zoomOutAct);
	viewMenu->addAction(normalSizeAct);
	viewMenu->addAction(autoNormalSizeAct);
	viewMenu->addAction(actualSizeAct);
	viewMenu->addSeparator();
	viewMenu->addAction(invertImageAct);
	viewMenu->addAction(rotateLeftAct);
	viewMenu->addAction(rotateRightAct);

	helpMenu = new QMenu(trUtf8("&Hilfe"), this);
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);

	ui->menuBar->addMenu(fileMenu);
	ui->menuBar->addMenu(viewMenu);
	ui->menuBar->addMenu(helpMenu);
}

/*
 * Menüpunkte erstellen
 */
void ProtoWindow::createActions()
{
	openAct = new QAction(trUtf8("Ö&ffnen..."), this);
	openAct->setShortcut(trUtf8("Ctrl+O"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	// TODO bei Speichern über STRG-S vermutlich nicht mehr nachfragen
	saveAct = new QAction(trUtf8("&Speichern"), this);
	saveAct->setShortcut(trUtf8("Ctrl+S"));
	saveAct->setEnabled(false);
	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	saveAsAct = new QAction(trUtf8("Speichern unter..."), this);
	saveAsAct->setEnabled(false);
	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	//printAct = new QAction(trUtf8("&Drucken..."), this);
	//printAct->setShortcut(trUtf8("Ctrl+P"));
	//printAct->setEnabled(false);
	//connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

	exitAct = new QAction(trUtf8("&Beenden"), this);
	exitAct->setShortcut(trUtf8("Ctrl+Q"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	zoomInAct = new QAction(trUtf8("ver&größern (25%)"), this);
	zoomInAct->setShortcut(trUtf8("Ctrl++"));
	zoomInAct->setEnabled(false);
	connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

	zoomOutAct = new QAction(trUtf8("ver&kleinern (25%)"), this);
	zoomOutAct->setShortcut(trUtf8("Ctrl+-"));
	zoomOutAct->setEnabled(false);
	connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

	normalSizeAct = new QAction(trUtf8("&Normale Größe"), this);
	normalSizeAct->setShortcut(trUtf8("Ctrl+N"));
	normalSizeAct->setEnabled(false);
	connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

	autoNormalSizeAct = new QAction(trUtf8("immer auf normale Größe einpassen"), this);
	autoNormalSizeAct->setCheckable(true);
	autoNormalSizeAct->setChecked(true);
	connect(autoNormalSizeAct, SIGNAL(triggered()), this, SLOT(autoNormalSize()));

	actualSizeAct = new QAction(trUtf8("&Tatsächliche Größe"), this);
	actualSizeAct->setShortcut(trUtf8("Ctrl+T"));
	actualSizeAct->setEnabled(false);
	connect(actualSizeAct, SIGNAL(triggered()), this, SLOT(actualSize()));

	invertImageAct = new QAction(trUtf8("Farben &invertieren"), this);
	invertImageAct->setEnabled(false);
	invertImageAct->setShortcut(trUtf8("I"));
	connect(invertImageAct, SIGNAL(triggered()), this, SLOT(invertImage()));

	rotateLeftAct = new QAction(trUtf8("90°-Drehung links"), this);
	rotateLeftAct->setEnabled(false);
	rotateLeftAct->setShortcut(trUtf8("Ctrl+Left"));
	connect(rotateLeftAct, SIGNAL(triggered()), this, SLOT(rotateLeft()));

	rotateRightAct = new QAction(trUtf8("90°-Drehung rechts"), this);
	rotateRightAct->setEnabled(false);
	rotateRightAct->setShortcut(trUtf8("Ctrl+Right"));
	connect(rotateRightAct, SIGNAL(triggered()), this, SLOT(rotateRight()));

	aboutAct = new QAction(trUtf8("&Über..."), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(trUtf8("Über &Qt"), this);
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void ProtoWindow::updateUI()
{
	bool imgNotNull = !this->imageLabel->pixmap()->isNull();
	saveAct->setEnabled(imgNotNull && (this->metaDataChanged || this->imageChanged()));
	saveAsAct->setEnabled(imgNotNull);

	zoomInAct->setEnabled(imgNotNull && this->scaleFactor<4.0);
	zoomOutAct->setEnabled(imgNotNull && this->scaleFactor>0.25);
	normalSizeAct->setEnabled(imgNotNull && fittingSize("updateUI")!=1.0);
	actualSizeAct->setEnabled(imgNotNull && this->scaleFactor!=1.0);

	invertImageAct->setEnabled(imgNotNull);
	rotateLeftAct->setEnabled(imgNotNull);
	rotateRightAct->setEnabled(imgNotNull);


	ui->backPushButton->setEnabled(imgNotNull && this->dirVec.size()>1);
	ui->forwardPushButton->setEnabled(imgNotNull && this->dirVec.size()>1);
	ui->showPushButton->setEnabled(imgNotNull);
	ui->rotateLeftPushButton->setEnabled(imgNotNull);
	ui->rotateRightPushButton->setEnabled(imgNotNull);

	QString windowTitle(trUtf8("EXIV Viewer"));
	if (imgNotNull) {
		QFileInfo info(this->dirVec.at(this->currentIndex));
		windowTitle.append(" - ");
		windowTitle.append(info.fileName());
	}
	setWindowTitle(windowTitle);
}

/*
 * Öffnet eine Bild-Datei und zeigt sie an (über Menü-Punkt)
 */
void ProtoWindow::open()
{
	// Öffnen-Dialog; Dateiname der zu öffnenden Datei wird in fileName gespeichert.
	const QString caption = trUtf8("Datei öffnen");
	const QString filter = trUtf8("Bilder (*.jpeg *.jpg *.JPG *.JPEG *.png)");
	QString fileName = QFileDialog::getOpenFileName(&(*(ui->centralWidget)), caption, QDir::homePath(), filter);

	if (fileName.isEmpty()) {
		// bedeutet, dass der Nutzer "Abbrechen" gedrückt hat
		return;
	}

	//wird gebraucht, um Pfad extrahieren zu können
	QFileInfo info(fileName);

	/*
	 * Die geöffnete Datei liegt in einem neuen Ordner, daher muss alles
	 * in Verbindung mit der Ordnerstruktur neu aufgebaut werden.
	 */
	if (info.absoluteDir() != this->curDir || this->dirVec.empty()) {
		onNewFolder(info.absoluteDir());
	}

	if (showNextImage(fileName)) {

		setCurrentIndex(info.absoluteFilePath());

		//scaleImage(fittingSize("on_backPushButton_clicked"));
		updateUI();
	}
}


void ProtoWindow::save()
{
	if (!this->imageChanged() && !this->metaDataChanged) {
		return;
	}

	QMessageBox speichernAbfrage(QMessageBox::Question, trUtf8(" Änderungen am Bild "), trUtf8("Speichern? "), QMessageBox::Yes|QMessageBox::No, this);

	speichernAbfrage.setButtonText(QMessageBox::Yes, trUtf8("Ja"));
	speichernAbfrage.setButtonText(QMessageBox::No, trUtf8("Nein"));
	speichernAbfrage.setDefaultButton(QMessageBox::No);

	if (speichernAbfrage.exec() == QMessageBox::Yes) {
		saveImage(this->curDir.absoluteFilePath(this->dirVec.at(this->currentIndex)));
		saveImageMetaData();
	}

}


void ProtoWindow::saveAs()
{
	const QString caption = trUtf8("Datei speichern");
	const QString filter = trUtf8("Bilder (*.jpeg *.jpg *.JPG *.JPEG *png)");
	QString fileName = QFileDialog::getSaveFileName(&(*(ui->centralWidget)), caption, this->curDir.absolutePath(), filter);

	if (fileName.isEmpty()) {
		// bedeutet, dass der Nutzer "Abbrechen" gedrückt hat
		return;
	}

	saveImage(fileName); // TODO gibt das Speichern unter einem anderen Dateinamen ein Problem bei den Metadaten?
}


void ProtoWindow::saveImage(QString fileName)
{
	if (!this->imageChanged()) {
		return;
	}

	bool saved = this->imageLabel->pixmap()->toImage().save(fileName);
	if (!saved) {
		QMessageBox msgBox;
		msgBox.setText("Das Bild" + fileName + "konnte nicht gespeichert werden!");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
		return;
	}

	this->resetImageChanged();
}


void ProtoWindow::saveImageMetaData()
{
	// Meta-Daten zu String serialisieren
	// in Datei schreiben

	//this->metaDataChanged = false;
}

void ProtoWindow::close()
{
	cleanUp();
	exit(0);
}

/*
void ProtoWindow::print()
{
	Q_ASSERT(imageLabel->pixmap());
	QPrintDialog dialog(&printer, this);
	if (dialog.exec())
	{
		QPainter painter(&printer);
		QRect rect = painter.viewport();
		QSize size = imageLabel->pixmap()->size();
		size.scale(rect.size(), Qt::KeepAspectRatio);
		painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
		painter.setWindow(imageLabel->pixmap()->rect());
		painter.drawPixmap(0, 0, *imageLabel->pixmap());
	}
}
*/

void ProtoWindow::zoomIn()
{
	scaleImage(1.25);

	this->imageLabel->update();
	updateUI();
}

void ProtoWindow::zoomOut()
{
	scaleImage(0.8);

	this->imageLabel->update();
	updateUI();
}

void ProtoWindow::normalSize()
{
	scaleImage(fittingSize("normalSize"));

	this->imageLabel->update();
	updateUI();
}


void ProtoWindow::autoNormalSize()
{
	this->isAutoNormalSize = autoNormalSizeAct->isChecked();
}

void ProtoWindow::actualSize()
{
	this->scaleFactor = 1.0;
	scaleImage(1.0);

	this->imageLabel->update();
	updateUI();
}

void ProtoWindow::invertImage()
{
	if (this->imageLabel->pixmap()->isNull()) {
		return;
	}

	QImage i = this->imageLabel->pixmap()->toImage();
	i.invertPixels();
	this->isInverted = !this->isInverted;
	this->imageLabel->setPixmap(QPixmap::fromImage(i)); // TODO auf setMyImage ändern

	updateUI();
}

void ProtoWindow::rotateLeft()
{
	rotate(270);
}

void ProtoWindow::rotateRight()
{
	rotate(90);
}

void ProtoWindow::rotate(int angle)
{
	if (this->imageLabel->pixmap()->isNull()) {
		return;
	}

	QTransform rotating;
	rotating.rotate(angle);
	QImage i = this->imageLabel->pixmap()->toImage().transformed(rotating);
	this->imageLabel->setPixmap(QPixmap::fromImage(i));
	this->imageLabel->resize(this->imageLabel->pixmap()->size());

	if (this->isAutoNormalSize) {
		this->scaleFactor = 1.0;
		scaleImage(fittingSize("rotate"));
	}

	this->rotateChanged = (this->rotateChanged + angle) % 360;

	this->imageLabel->update();

	updateUI();
}

void ProtoWindow::about()
{
	QMessageBox::about(this, trUtf8("About Image Viewer"),
			trUtf8("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
				"and QScrollArea to display an image. QLabel is typically used "
				"for displaying a text, but it can also display an image. "
				"QScrollArea provides a scrolling view around another widget. "
				"If the child widget exceeds the size of the frame, QScrollArea "
				"automatically provides scroll bars. </p><p>The example "
				"demonstrates how QLabel's ability to scale its contents "
				"(QLabel::scaledContents), and QScrollArea's ability to "
				"automatically resize its contents "
				"(QScrollArea::widgetResizable), can be used to implement "
				"zooming and scaling features. </p><p>In addition the example "
				"shows how to use QPainter to print an image.</p>"));
}

/*
 * Aktionen, die ausgeführt werden müssen, wenn der 'zurück'-Knopf gedrückt wurde.
 */
void ProtoWindow::on_backPushButton_clicked()
{
	// Daten des "alten" Bildes müssen gespeichert werden.
	save();
		/*
		 * Textdatei erstellen, falls nicht vorhanden.
		 *
		 * Daten in Textdatei schreiben. Dabei werden die alten Daten für dieses Bild überschrieben.
		 * Die Daten werden in der folgenden Reihenfolge gespeichert:
		 * "&"Dateiname(mit Endung)$bildbeschreibung"$"x-Koordinate"%"y-Koordinate"%"Text"$"nächstes Quadrat..."&"
		 *
		 * Bis auf Weiteres wird der gesamte Inhalt der Datei neu gespeichert.
		 */

	// current-Counter eins runterzaehlen, damit das naechste Bild angezeigt wird
	if (this->currentIndex-1 < 0) {
		this->currentIndex = this->dirVec.size()-1;
	} else {
		this->currentIndex = (this->currentIndex-1) % (this->dirVec.size());
	}
	QString fileName = this->dirVec[currentIndex];

	showNextImage((this->curDir.absoluteFilePath(fileName)));

	scaleImage(fittingSize("on_backPushButton_clicked"));
	updateUI();
}

/*
 * Aktionen, die ausgeführt werden müssen, wenn der 'vorwärts'-Knopf gedrückt wurde.
 */
void ProtoWindow::on_forwardPushButton_clicked()
{
	// Daten des "alten" Bildes muessen gespeichert werden.
	save();
		/*
		 * Textdatei erstellen, falls nicht vorhanden.
		 * s.o.
		 */

	// current-Counter eins hochzaehlen, damit das naechste Bild angezeigt wird
	this->currentIndex = (this->currentIndex+1) % (this->dirVec.size());
	QString fileName = this->dirVec[currentIndex];

	if (showNextImage(this->curDir.absoluteFilePath(fileName))) {
		scaleImage(fittingSize("on_backPushButton_clicked"));
		updateUI();
	}
}

void ProtoWindow::on_showPushButton_clicked()
// nur noch zum Testen und Debuggen; sollte zur Endversion entfernt werden.
{
	QString thisOne = this->dirVec[this->currentIndex];
	QString text = "Diese Datei: ";
	text.append(thisOne);

	text.append("\nabsoltueFilePath: ");
	QFileInfo info(this->dirVec[this->currentIndex]);
	text.append(info.absoluteFilePath());

	text.append("\n\nAlle Bilddateien im Ordner:\n");
	for(int i = 0; i < this->dirVec.size(); i++)
	{
		text.append(this->dirVec[i]);
		text.append('\n');
	}
	text.append("current Path: ");
	text.append(QDir::currentPath());

	QSize imageSize = this->imageLabel->pixmap()->size();
	QSize labelSize = this->imageLabel->size();
	QSize areaSize = ui->scrollArea->size();
	//std::ofstream fout("Groessen.txt");

	text.append("\nBild-Höhe: ");
	text.append(QString::number(imageSize.height()));
	text.append("\nBild-Weite: ");
	text.append(QString::number(imageSize.width()));
	text.append("\nLabel-Höhe: ");
	text.append(QString::number(labelSize.height()));
	text.append("\nLabel-Weite: ");
	text.append(QString::number(labelSize.width()));
	text.append("\nArea-Höhe: ");
	text.append(QString::number(areaSize.height()));
	text.append("\nArea-Weite: ");
	text.append(QString::number(areaSize.width()));
	//fout.close();
	//QString on = (ui->backPushButton->objectName());
	text.append("\nScale-factor: ");
	text.append(QString::number(this->scaleFactor));

	ui->toPutPlainTextEdit->setPlainText(text);

}


void ProtoWindow::on_rotateLeftPushButton_clicked()
{
	rotateLeft();
}


void ProtoWindow::on_rotateRightPushButton_clicked()
{
	rotateRight();
}

/*
 * Bei einem Doppelklick auf das Bild wird an dieser Stelle ein Quadrat erstellt, das beschriftet werden kann.
 * Dabei ist der Punkt, auf den geklickt wurde, der Mittelpunkt des Quadrats.
 */
void ProtoWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
	QPoint p = event->pos();

	// Berechnen, ob der Doppelklick innerhalb des Bildes gemacht wurde
	if(!(p.x() > 11 && p.x() < this->imageLabel->width()+11 && p.y() > 33 && p.y() < this->imageLabel->height()+33)){
		return;
	}

	//Koordinaten des Quadrats errechnen
	int _x = static_cast<int>(static_cast<double>(p.x()) * (1.0/scaleFactor))-11 + ui->scrollArea->horizontalScrollBar()->sliderPosition();
	int _y = static_cast<int>(static_cast<double>(p.y()) * (1.0/scaleFactor))-33 + ui->scrollArea->verticalScrollBar()->sliderPosition();

	bool ok;
	QString text = QInputDialog::getText(this, trUtf8("Name"), trUtf8("Name eingeben:"), QLineEdit::Normal, "", &ok);
	if (ok) {
		// neues PersonSquare erstellen und in den Vektor aller PersonSquares einfügen
		PersonSquare* ps = new PersonSquare(0, this->imageLabel, _x, _y, text);

		this->imageLabel->addSquare(ps);
		//this->imageLabel->update();
	}
}
