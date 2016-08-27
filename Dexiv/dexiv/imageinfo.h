#ifndef IMAGEINFO_H
#define IMAGEINFO_H

struct ImageInfo {
	QString fileName;
	QString content;
	int offset;
	bool changed;
	bool loaded;
};

#endif // IMAGEINFO_H
