//#include <QApplication>
#include <qapplication.h>
#include <iostream>
#include "protowindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ProtoWindow w;

	QString str = "$bli$bla$blo";
	std::cout << str.section('$',0,0).toStdString();
	std::cout << str.section('$',0,1).toStdString();
	std::cout << str.section('$',0,2).toStdString();
	std::cout << str.section('$',1,0).toStdString();
	std::cout << str.section('$',1,1).toStdString();

	w.setArgs(argc, argv);
	if(argc > 1)
	{
		w.setDirVec();
		w.setFirstImage();
	}
	w.show();
	return a.exec();
}
