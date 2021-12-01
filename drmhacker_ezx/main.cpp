#include "drmfile.h"

#include <cstdio>

#include <qapplication.h>
#include <qdatastream.h>
#include <qfile.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		printf(
			"EZX DRM hacker by Borman, v1.0\n"
			"Usage: drmhacker <in-file> <out-file>\n"
		);
		return 1;
	}

	QApplication app(argc, argv, FALSE);

	J2ME_JAID_DRMFile drmFile(argv[1]);
	QFile plainFile(argv[2]);
	if (!drmFile.open(IO_ReadOnly)) {
		printf("Error opening drmFile for IO_ReadOnly: %s.\n", argv[1]);
		return 1;
	}
	if (!plainFile.open(IO_WriteOnly)) {
		printf("Error opening plainFile for IO_WriteOnly: %s.\n", argv[2]);
		return 1;
	}

	QDataStream plainFileStream(&plainFile);
	const uint bufsize = 4096;
	char buf[bufsize];
	while (!drmFile.atEnd()) {
		int bsize = drmFile.readBlock(buf, bufsize);
		if (bsize < 0)
			break;
		plainFileStream.writeRawBytes(buf, bsize);
	}

	drmFile.close();
	plainFile.close();
	return 0;
}
