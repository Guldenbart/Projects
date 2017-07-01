#ifndef PROTOWINDOW_H
#define PROTOWINDOW_H

// TODO: werden die Informationen zum Bild gesichert???
// TODO: in der Titelleiste des Fensters den Namen der angeziegten Datei anzeigen
// TODO: schauen, was alles 'const' werden kann
// KOMMENTIEREN!!!!!
// TODO: prüfen, ob setCurrent nicht auch ohne Parameter geht, da filename ja als Attribut abgespeichert wird
// TODO: Suchfunktion in (untergeordneten) Ordner(n) nach Personen?
// TODO: Kann man auch mit STRG+Mausrad auch zoomen?
// TODO: Optimierung: doch 'QImage* currentImage' verwenden
// TODO: QString bei 'fittingSize()' wegmachen
// TODO: unnötige Zwischenräume bei der GUI wegmachen
// TODO: Abhängigkeiten von std-Bibliothek entfernen
// TODO: beim Eingeben eines Textes für ein PersonSquare müssen die Sonderzeichen '$' und '§' abgelehnt werden.

#include <QMainWindow>
#include <QtGui>
#include <QScrollBar>
#include <QLabel>
//#include <QtPrintSupport/QPrinter>
//#include <QtPrintSupport/QPrintDialog>
#include <QMessageBox>
#include <QFileDialog>

#include "personSquare.h"
#include "myImageLabel.h"
#include "imageinfo.h"

class PersonSquare;
class MyImageLabel;

namespace Ui
{
    class ProtoWindow;
}

class ProtoWindow : public QMainWindow
{
    Q_OBJECT

public:
	ProtoWindow(QWidget *parent = 0);
	~ProtoWindow();
	void setArgs(int ac, char** av);
	void setDirVec();
	void setFirstImage();
	void mouseDoubleClickEvent(QMouseEvent* e);

private:

	//Daten
	Ui::ProtoWindow *ui;

	int argc;
	char** argv;
	MyImageLabel* imageLabel; // Label, das das Bild enthält

	QDir curDir; // Verzeichnis des aktuellen Bilds; wird vor allem gebraucht um zu überprüfen, ob bei open() der Ordner gewechselt wird
	QVector<QString> dirVec; // Vektor mit allen Bild-Dateien-Pfade des aktuellen Ordners
	int currentIndex; // myDir-Index des momentan geöffneten Bildes
	double scaleFactor;
	QVector<ImageInfo> imgInfoVec;
	bool imageChanged;
	bool metaDataChanged;
	bool isAutoNormalSize;

	//Funktionen
	void onNewFolder();
	void onNewImage();
	void setCurrentIndex(QString filename);
	void createActions();
	void createMenus();
	void updateUI();
	void saveImage(QString fileName);
	void scaleImage(double factor);
	void adjustScrollBar(QScrollBar *scrollBar, double factor);
	double fittingSize(QString from);
	bool showNextImage(QString filename);
	void parseLine(QString content);
	//bool findFile(QString content, int& fileNumber);
	void saveImageMetaData(); //<-TODO
	void cleanUp();

	//Aktionen
	//QPrinter printer;

	QAction *openAct;
	QAction *saveAct;
	QAction *saveAsAct;
	//QAction *printAct;
	QAction *exitAct;
	QAction *zoomInAct;
	QAction *zoomOutAct;
	QAction *normalSizeAct;
	QAction *autoNormalSizeAct;
	QAction *actualSizeAct;
	QAction *invertImageAct;
	QAction *rotateLeftAct;
	QAction *rotateRightAct;
	QAction *aboutAct;
	QAction *aboutQtAct;

	//Menüs
	QMenu *fileMenu;
	QMenu *viewMenu;
	QMenu *helpMenu;


private slots:
	void on_backPushButton_clicked();
	void on_forwardPushButton_clicked();
	void on_showPushButton_clicked();

	void open();
	void save();
	void saveAs();
	void close();
	//void print();
	void zoomIn();
	void zoomOut();
	void normalSize();
	void autoNormalSize();
	void actualSize();
	void invertImage();
	void rotateLeft();
	void rotateRight();
	void rotate(int angle);
	void about();
	void on_rotateLeftPushButton_clicked();
	void on_rotateRightPushButton_clicked();
};

/*Position x: 11
Position y: 33
global x: 11
global y: 53*/
#endif // PROTOWINDOW_H
