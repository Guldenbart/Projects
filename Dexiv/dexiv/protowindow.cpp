#include "protowindow.h"
#include "ui_protowindow.h"
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

	this->imageLabel = new MyImageLabel;
	this->imageLabel->setBackgroundRole(QPalette::Base);
	this->imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	this->imageLabel->setScaledContents(true);
	this->imageLabel->setZooming(false);

	ui->scrollArea->setBackgroundRole(QPalette::Dark);
	ui->scrollArea->setWidgetResizable(false);
	ui->scrollArea->setWidget(imageLabel);

	this->current = 0;
	this->scaleFactor = 1.0;
	this->curDir = QDir::current();
	this->imageChanged = false;
	this->timesRotated = 0;

	this->showMaximized();

	createActions();
	createMenus();

	setWindowTitle(tr("EXIV Viewer"));
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
	// Filter erstellen, der nur jp(e)g-Datein berücksichtigt.
	QStringList filters;
	filters << "*.jpg" << "*.jpeg" << "*.JPG" << "*.JPEG";

	// Liste aller Dateien mit im Filter angegebenen Dateinamen erstellen
	QFileInfoList _infoList = this->curDir.entryInfoList(filters);

	// Vektor erstellen, in den alle Bilddateien mit ihrem absoluten Pfad eingetragen werden.
	for(int in = 0; in < _infoList.length(); in++) {
		this->dirVec.push_back(_infoList.at(in).absoluteFilePath());
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
	setCurrent(q);

	// Bild anzeigen

	QString fileName = (this->curDir.filePath(this->dirVec[this->current]));
	// Dateiname sollte nicht leer sein...
	if (showNextImage(fileName)) {

		ui->backPushButton->setEnabled(true);
		ui->forwardPushButton->setEnabled(true);
		ui->showPushButton->setEnabled(true);
	}
}

void ProtoWindow::onNewFolder()
{
	onNewImage();
}

void ProtoWindow::onNewImage()
{
	// Label leeren (d.h. Bild und PersonSquares löschen)
	this->imageLabel->clear();
	this->imageLabel->clearSquares();

	this->scaleFactor = 1.0;
}

/*
 * findet heraus, welchen Index das als geoeffnete Bild hat.
 */
void ProtoWindow::setCurrent(QString filename)
{
	// '\' durch '/' ersetzen:
	for(int i = 0; i < filename.size(); i++)
	{
		if(filename[i] == '\\')
			filename[i] = '/';
	}

	// Durch den Vektor mit allen Dateinamen gehen und mit 'filename' abgleichen.
	for(int i = 0; i < this->dirVec.size(); i++)
	{
		if(curDir.filePath(this->dirVec[i]) == filename)
		{
			this->current = i;
			return;
		}
	}
	 // Falls der Dateiname nicht gefunden wurde... (was wahrscheinlich nur durch einen
	 // Fehler an einer anderen Stelle im Programm passieren kann)
	this->current = 0;
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

	QImage newImage = imageLabel->MyImage();
	const QSize newSize(static_cast<int>(static_cast<double>(newImage.width()) * factor), (static_cast<int>(static_cast<double>(newImage.height()) * factor)));
	newImage.scaled(newSize);
	imageLabel->setMyImage(newImage);
	imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

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
double ProtoWindow::fittingSize()
{
	if (this->currentImage.isNull()) {
		return 1.0;
	}

	QSize labelSize = this->imageLabel->size();
	QSize areaSize = ui->scrollArea->size();

	double labelSize_h = static_cast<double>(labelSize.height());
	double labelSize_w = static_cast<double>(labelSize.width());
	double areaSize_h = static_cast<double>(areaSize.height());
	double areaSize_w = static_cast<double>(areaSize.width());

	if (labelSize_h < areaSize_h && labelSize_w < areaSize_w) {
		//Bild ist sowieso kleiner als ScrollArea
		return 1.0;
	}

	if (((labelSize_h/labelSize_w)/(areaSize_h/areaSize_w)) > 1) {
		// Höhe ist entscheidender Faktor
		return (areaSize_h / labelSize_h)-0.001;
	} else {
		// Weite ist entscheidender Faktor
		return (areaSize_w / labelSize_w)-0.001;
	}
}

/*
 * Gemeinsame Funktion von open, on_backPushButton_clicked und on_forwardPushButton_clicked.
 *
 * sorgt dafür, dass Bild 'fileName' angezeigt wird.
 */
bool ProtoWindow::showNextImage(QString fileName)
{
	/*
	// Label leeren (d.h. Bild und PersonSquares löschen)
	this->imageLabel->clear();
	this->imageLabel->clearSquares();
	*/

	// TODO: sollte eig nicht mehr vorkommen, weil in open() schon abgefangen; und in den anderen Funktionen sollte fileName nie leer werden können
	if (fileName.isEmpty()) {
		return false;
	}

	onNewImage();

	QImage image(fileName);
	if (image.isNull()) {
		 QMessageBox::information(this, tr("EXIV Viewer"), tr("%1 konnte nicht geladen werden.").arg(fileName));
		 return false;
	}

	this->currentImage = image;
	this->imageLabel->setPixmap(QPixmap::fromImage(image));

	scaleImage(fittingSize());

	updateUI();
	return true;
}

/*
 * extrahiert die Informationen einer Zeile der Content-Datei.
 */
QString ProtoWindow::parseContentFromFile(QString content) {

	// Als erstes müssen die Rand-Symbole überprüft werden.
	if (content[0] != '&' || content[content.size()-1] != '%') {
		//Fehler: Falsche Kodierung in der Datei
	}
	int index = 0;
	// Als nächstes muss der Punkt des Dateinamens gefunden werden
	while (content[index] != '.') {
		if (content[index++] == '$') {
			//Fehler: ungültiger Dateiname, der nicht erkannt werden kann.
		}
		if (content[index] == '%') {
			//Fehler: kein Dateiname
		}
	}
	int fileNumber = -1;
	if (content[index+3] == 'g' || content[index+3] == 'G') {
		findFile(QString::fromStdString((content.toStdString()).substr(1,index+3)), fileNumber);
		if (fileNumber == -1) {
			//Fehler: ungültiger Dateiname
		}
	}
	else {

	}

	return "";
}

/*
 * Überprüft, ob die Datei, die als String übergeben wird, eine Bilddatei
 * im dirVec ist. falls ja, wird true zurückgeliefert und der Index im dirVec
 * unter fileNumber gespeichert. Falls nicht, wird false zurückgeliefert.
 */
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

void ProtoWindow::saveData()
{

}

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
	fileMenu = new QMenu(tr("&Datei"), this);
	fileMenu->addAction(openAct);
	//fileMenu->addAction(printAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	viewMenu = new QMenu(tr("&Ansicht"), this);
	viewMenu->addAction(zoomInAct);
	viewMenu->addAction(zoomOutAct);
	viewMenu->addAction(normalSizeAct);
	viewMenu->addAction(actualSizeAct);
	viewMenu->addSeparator();
	viewMenu->addAction(invertImageAct);
	viewMenu->addAction(rotateLeftAct);
	viewMenu->addAction(rotateRightAct);

	helpMenu = new QMenu(tr("&Hilfe"), this);
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
	openAct = new QAction(tr("Ö&ffnen..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	//printAct = new QAction(tr("&Drucken..."), this);
	//printAct->setShortcut(tr("Ctrl+P"));
	//printAct->setEnabled(false);
	//connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

	exitAct = new QAction(tr("&Beenden"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	zoomInAct = new QAction(tr("ver&größern (25%)"), this);
	zoomInAct->setShortcut(tr("Ctrl++"));
	zoomInAct->setEnabled(false);
	connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

	zoomOutAct = new QAction(tr("ver&kleinern (25%)"), this);
	zoomOutAct->setShortcut(tr("Ctrl+-"));
	zoomOutAct->setEnabled(false);
	connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

	normalSizeAct = new QAction(tr("&Normale Größe"), this);
	normalSizeAct->setShortcut(tr("Ctrl+N"));
	normalSizeAct->setEnabled(false);
	connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

	actualSizeAct = new QAction(tr("&Tatsächliche Größe"), this);
	actualSizeAct->setShortcut(tr("Ctrl+T"));
	actualSizeAct->setEnabled(false);
	connect(actualSizeAct, SIGNAL(triggered()), this, SLOT(actualSize()));

	invertImageAct = new QAction(tr("Farben &invertieren"), this);
	invertImageAct->setEnabled(false);
	invertImageAct->setShortcut(tr("I"));
	connect(invertImageAct, SIGNAL(triggered()), this, SLOT(invertImage()));

	rotateLeftAct = new QAction(tr("90°-Drehung links"), this);
	rotateLeftAct->setEnabled(false);
	rotateLeftAct->setShortcut(tr("Ctrl+Left"));
	connect(rotateLeftAct, SIGNAL(triggered()), this, SLOT(rotateLeft()));

	rotateRightAct = new QAction(tr("90°-Drehung rechts"), this);
	rotateRightAct->setEnabled(false);
	rotateRightAct->setShortcut(tr("Ctrl+Right"));
	connect(rotateRightAct, SIGNAL(triggered()), this, SLOT(rotateRight()));

	aboutAct = new QAction(tr("&Über..."), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAct = new QAction(tr("Über &Qt"), this);
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void ProtoWindow::updateUI()
{
	double fs = fittingSize();
	zoomInAct->setEnabled(this->scaleFactor<4.0 && !this->currentImage.isNull());
	zoomOutAct->setEnabled(this->scaleFactor>0.25 && !this->currentImage.isNull());
	normalSizeAct->setEnabled(fittingSize()!=1.0 && !this->currentImage.isNull());
	actualSizeAct->setEnabled(this->scaleFactor!=1.0 && !this->currentImage.isNull());

	invertImageAct->setEnabled(!this->currentImage.isNull());
	rotateLeftAct->setEnabled(!this->currentImage.isNull());
	rotateRightAct->setEnabled(!this->currentImage.isNull());
}

/*
 * Öffnet eine Bild-Datei und zeigt sie an (über Menü-Punkt)
 */
void ProtoWindow::open()
{
	// Öffnen-Dialog; Dateiname der zu öffnenden Datei wird in fileName gespeichert.
	const QString o = tr("Datei öffnen");
	QString fileName = QFileDialog::getOpenFileName(&(*(ui->centralWidget)), o, QDir::currentPath(), tr("Bilder (*.jpeg *.jpg *.JPG *.JPEG)"));

	if (fileName == "") {
		// bedeutet, dass der Nutzer "Abbrechen" gedrückt hat
		return;
	}

	onNewFolder();

	//wird gebraucht, um Pfad extrahieren zu können
	QFileInfo info(fileName);

	if (showNextImage(fileName)) {

		//Überprüfen, ob die Liste der Dateien neu erstellt werden muss
		if(info.absoluteDir() != this->curDir || this->dirVec.empty())
		{
			this->curDir = info.absoluteDir();
			setDirVec();
		}
		setCurrent(info.absoluteFilePath());

		ui->backPushButton->setEnabled(true);
		ui->forwardPushButton->setEnabled(true);
		ui->showPushButton->setEnabled(true);

		updateUI();
	}
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
	//this->imageLabel->setZooming(true);
	scaleImage(1.25);
	//this->imageLabel->setZooming(false);
	updateUI();
}

void ProtoWindow::zoomOut()
{
	//this->imageLabel->setZooming(true);
	scaleImage(0.8);
	//this->imageLabel->setZooming(false);
	updateUI();
}

void ProtoWindow::normalSize()
{
//	this->scaleFactor = fittingSize();
	scaleImage(fittingSize());

	//imageLabel->adjustSize();
	updateUI();
}

void ProtoWindow::actualSize()
{
	this->scaleFactor = 1.0;
	scaleImage(1.0);

	updateUI();
}

void ProtoWindow::invertImage()
{
	if (this->currentImage.isNull()) {
		return;
	}

	this->currentImage.invertPixels();
	this->imageChanged = true;
	this->imageLabel->update();
}

void ProtoWindow::rotateLeft()
{
	rotate(90);
	this->imageLabel->setMyImage(this->currentImage);
	this->imageLabel->repaint();
}

void ProtoWindow::rotateRight()
{
	rotate(270);
	this->imageLabel->setMyImage(this->currentImage);
	this->imageLabel->repaint();
}

void ProtoWindow::rotate(int angle)
{
	QTransform rotating;
	rotating.rotate(angle);
	this->currentImage = this->currentImage.transformed(rotating);

	//update();
}

void ProtoWindow::about()
{
	QMessageBox::about(this, tr("About Image Viewer"),
			tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
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

		/*
		 * Textdatei erstellen, falls nicht vorhanden.
		 *
		 * Daten in Textdatei schreiben. Dabei werden die alten Daten für dieses Bild überschrieben.
		 * Die Daten werden in der folgenden Reihenfolge gespeichert:
		 * "&"Dateiname"$"x-Koordinate,y-Koordinate,Text"$"nächstes Quadrat..."%"
		 *
		 * Bis auf Weiteres muss der gesamte Inhalt der Datei als String gespeichert
		 * werden, der Teil, der zum Bild gehört, "ausgeschnitten", die neuen, aktualisierten
		 * Informationen angehängt und dann alles wieder ge/überschrieben werden.
		 */

	// current-Counter eins runterzaehlen, damit das naechste Bild angezeigt wird
	if(this->current-1 < 0)
		this->current = this->dirVec.size()-1;
	else
		this->current = (this->current-1) % (this->dirVec.size());
	QString fileName = this->dirVec[current];

	showNextImage(fileName);
}

/*
 * Aktionen, die ausgeführt werden müssen, wenn der 'vorwärts'-Knopf gedrückt wurde.
 */
void ProtoWindow::on_forwardPushButton_clicked()
{
	// Daten des "alten" Bildes muessen gespeichert werden.

		/*
		 * Textdatei erstellen, falls nicht vorhanden.
		 *
		 * Daten in Textdatei schreiben. Dabei werden die alten Daten für dieses Bild überschrieben.
		 * Die Daten werden in der folgenden Reihenfolge gespeichert:
		 * "§"Dateiname"$"x-Koordinate,y-Koordinate,Text"$"nächstes Quadrat..."%"
		 *
		 * Bis auf Weiteres muss der gesamte Inhalt der Datei als String gespeichert
		 * werden, der Teil, der zum Bild gehört, "ausgeschnitten", die neuen, aktualisierten
		 * Informationen angehängt und dann alles wieder ge/überschrieben werden.
		 */

	// current-Counter eins hochzaehlen, damit das naechste Bild angezeigt wird
	this->current = (this->current+1) % (this->dirVec.size());
	QString fileName = this->dirVec[current];

	showNextImage(fileName);
}

void ProtoWindow::on_showPushButton_clicked()
// nur noch zum Testen und Debuggen; sollte zur Endversion entfernt werden.
{
	QString thisOne = this->dirVec[this->current];
	QString text = "Diese Datei: ";
	text.append(thisOne);

	text.append("\nabsoltueFilePath: ");
	QFileInfo info(this->dirVec[this->current]);
	text.append(info.absoluteFilePath());

	text.append("\n\nAlle Bilddateien im Ordner:\n");
	for(int i = 0; i < this->dirVec.size(); i++)
	{
		text.append(this->dirVec[i]);
		text.append('\n');
	}
	text.append("current Path: ");
	text.append(QDir::currentPath());

	QSize imageSize = this->currentImage.size();
	QSize labelSize = this->imageLabel->size();
	QSize areaSize = ui->scrollArea->size();
	std::ofstream fout("Groessen.txt");

	fout << "Bild-Höhe: ";
	fout << imageSize.height();
	fout << "\nBild-Weite: ";
	fout << imageSize.width();
	fout << "\nLabel-Höhe: ";
	fout << labelSize.height();
	fout << "\nLabel-Weite: ";
	fout << labelSize.width();
	fout << "\nArea-Höhe: ";
	fout << areaSize.height();
	fout << "\nArea-Weite: ";
	fout << areaSize.width();
	fout.close();

	ui->toPutPlainTextEdit->setPlainText(text);
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
	QString text = QInputDialog::getText(this, tr("Name"), tr("Name eingeben:"), QLineEdit::Normal, "", &ok);
	if (ok) {
		// neues PersonSquare erstellen und in den Vektor aller PersonSquares einfügen
		PersonSquare* ps = new PersonSquare(0, this->imageLabel, _x, _y, text);

		this->imageLabel->addSquare(ps);
		this->imageLabel->update();
	}
}

void ProtoWindow::paintEvent(QPaintEvent* event)
{
	/*std::ofstream fout("test.txt");
	fout << "paintEvent aufgerufen!";

	QPainter painter(ui->scrollArea);
	painter.begin(ui->scrollArea);
	painter.setPen(Qt::SolidLine);
	painter.setBrush(Qt::black);
	painter.drawRect(100, 100, 50, 50);
	fout << "Viereck gemalt!";
	fout.close();
	painter.drawImage(QPoint(11, 33), this->currentImage);*/
}
