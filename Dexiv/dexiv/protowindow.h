#ifndef PROTOWINDOW_H
#define PROTOWINDOW_H

// TODO: prüfen, ob man das paintEvent dieser Klasse ganz entfernen kann
// KOMMENTIEREN!!!!!
// TODO: prüfen, ob setCurrent nicht auch ohne Parameter geht, da filename ja als Attribut abgespeichert wird
// TODO: Suchfunktion in (untergeordneten) Ordner(n) nach Personen?
// TODO: Kann man auch mit STRG+Mausrad auch zoomen?
// TODO: unnötige Zwischenräume bei der GUI wegmachen
// TODO: Abhängigkeiten von std-Bibliothek entfernen

#include <QtGui/QMainWindow>
#include <QtGui>
//#include <vector>

#include "personSquare.h"
#include "myImageLabel.h"

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

protected:
	void paintEvent(QPaintEvent* e);

private:
	/*struct Node {
		int _x;
		int _y;
		QString _text;
		Node * next;
	};*/

	struct Array {
		QString fileName;
		QString content;
		int offset;
		bool changed;
	};

	//Daten
	Ui::ProtoWindow *ui;

	int argc;
	char** argv;
	MyImageLabel* imageLabel; //Label, das das Bild enthaelt

	QDir curDir; //aktuelles Verzeichnis
	bool vectorSet; //true, wenn die jp(e)g-Dateien-Liste schon erstellt wurde
	QVector<QString> dirVec; //Vektor mit allen jp(e)g-Dateien des Ordners
	int current; // myDir-Index des momentan geöffneten Bildes
	QImage currentImage;
	QString filename;
	double scaleFactor;
	Array* stringArray;

	//Funktionen
	void setCurrent(QString filename);
	void createActions();
	void createMenus();
	void updateActions();
	void scaleImage(double factor);
	void adjustScrollBar(QScrollBar *scrollBar, double factor);
	double fittingSize();
	bool showNextImage(QString filename);
	void createStringList();
	QString parseContentFromFile(QString content, int offset);
	bool findFile(QString content, int& fileNumber);

	//Aktionen
	QPrinter printer;

	QAction *openAct;
	QAction *printAct;
	QAction *exitAct;
	QAction *zoomInAct;
	QAction *zoomOutAct;
	QAction *normalSizeAct;
	QAction *fitToWindowAct;
	QAction *aboutAct;
	QAction *aboutQtAct;

	//Menues
	QMenu *fileMenu;
	QMenu *viewMenu;
	QMenu *helpMenu;



private slots:
	void on_backPushButton_clicked();
	void on_forwardPushButton_clicked();
	void on_showPushButton_clicked();

	void open();
	void print();
	void zoomIn();
	void zoomOut();
	void normalSize();
	void fitToWindow();
	void about();
};

/*Position x: 11
Position y: 33
global x: 11
global y: 53*/
#endif // PROTOWINDOW_H
