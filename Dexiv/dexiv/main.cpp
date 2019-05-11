//#include <QApplication>
#include <qapplication.h>
#include <iostream>
#include "protowindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ProtoWindow w;

	w.setArgs(argc, argv);
	if(argc > 1)
	{
		w.setDirVec();
		w.setFirstImage();
	}
	w.show();
	return a.exec();
}
