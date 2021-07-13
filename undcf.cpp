/*
 * Flow:
 * DRM_IsDRMFile() => DRM_StartRightsMeter() => DRM_CreateConsumptionFilePath() => DRM_StopRightsMeter()
 *
 * https://github.com/rajdeokumarsingh/Notes/blob/master/computer.science/android/multimedia/drm/drm.pekall.2.2/concept.drm.txt
 * https://github.com/rajdeokumarsingh/Notes/blob/master/computer.science/android/multimedia/drm/drm.pekall.2.2/flow/flow.play.cpp
 *
 *
 */

// Qt
#include <qapplication.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qglobal.h>

// DRM
extern QString *DRM_IsDRMFile(QString *aFile);
extern int DRM_StartRightsMeter(int *session, QString *aFile, int arg1, int arg2);
extern QString *DRM_CreateConsumptionFilePath(int session, int arg1, QString *aFile)
extern int DRM_StopRightsMeter(int session);

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

void copy_file(QFile &fileFrom, QFile &fileTo) {
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

void copy(const QString &pathIn, const QString pathOut) {
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

	copy_file(fileIn, fileOut);

	fileIn.close();
	fileOut.close();
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		usage();
		return 1;
	}

	QApplication app(argc, argv); // TODO: Is it necessary?

	QString pathIn(argv[1]);
	QString pathOut(argv[2]);

	if (DRM_IsDRMFile(pathIn)) {
		int drm_session = 0;
		if (DRM_StartRightsMeter(&drm_session, &pathIn, 0, 1) == 0x7D2) {
			QString pathVirtual = DRM_CreateConsumptionFilePath(drm_session, 0, &pathIn);
			if (pathVirtual) {
				qDebug(QString("Info: Virtual path for consumption is: '%1").arg(pathVirtual));
				copy(pathVirtual, pathOut);
			} else {
				qDebug("Error: Looks like DRM_CreateConsumptionFilePath() returned NULL.");
				return 2;
			}
		} else {
			qDebug("Error: Looks like DRM_StartRightsMeter() function failed.");
			return 3;
		}
	} else {
		qDebug(QString("Error: '%1' is not DRM file!").arg(pathIn));
		return 4;
	}

	return 0;
}
