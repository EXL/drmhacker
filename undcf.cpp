// Qt
#include <qapplication.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qglobal.h>

void usage(void) {
	qDebug(
		"|MotoMAGX OMA DRM Hacker| by EXL, v1.0, 14-Jun-2021\n\n"
		"Information:\n"
		"\thttps://forum.motofan.ru/index.php?showtopic=1262\n"
		"Source code:\n"
		"\thttps://github.com/EXL/undcf\n"
		"Usage:\n"
		"\tundcf <in-file-path> <out-file-path>\n"
		"Example:\n"
		"\tundcf picture.dcf picture.png\n"
	);
}

void copy(QFile &fileFrom, QFile &fileTo) {
	QDataStream streamOut(&fileTo);
	const int buffer_size = 4096;
	int block_size_accumulator = 0;
	char buffer[buffer_size];
	while (!fileFrom.atEnd()) {
		const int block_size = fileFrom.readBlock(buffer, buffer_size);
		block_size_accumulator += block_size;
		if (block_size < 0)
			break;
		streamOut.writeRawBytes(buffer, block_size);
		qDebug(QString("Written size: %1, buffer size: %2").arg(block_size_accumulator).arg(block_size));
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		usage();
		return 1;
	}

	QApplication app(argc, argv); // TODO: Is it necessary?

	QString pathIn(argv[1]);
	QString pathOut(argv[2]);
	QFile fileIn(pathIn);
	QFile fileOut(pathOut);
	if (!fileIn.open(IO_ReadOnly)) {
		qDebug(QString("Error: Cannot open file '%1' to read!").arg(pathIn));
		return 2;
	}
	if (!fileOut.open(IO_WriteOnly)) {
		qDebug(QString("Error: Cannot open file '%1' to write!").arg(pathOut));
		fileIn.close();
		return 3;
	}

	copy(fileIn, fileOut);

	fileIn.close();
	fileOut.close();

	// IsDrm?
	// StartDrmShit?
	// ObtainPath?
	// CopyFromPathToNormalFile?
	// StopDrmShit?

	return 0;
}
