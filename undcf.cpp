/*
 * History:
 * 30-Aug-2021: Clean version.
 * 14-Jun-2021: Draft version.
 *
 * Flow:
 * DRM_IsDRMFile() => DRM_StartRightsMeter() => DRM_CreateConsumptionFilePath() => DRM_StopRightsMeter()
 *
 * Some Information:
 * https://github.com/rajdeokumarsingh/Notes/blob/master/computer.science/android/multimedia/drm/drm.pekall.2.2/concept.drm.txt
 * https://github.com/rajdeokumarsingh/Notes/blob/master/computer.science/android/multimedia/drm/drm.pekall.2.2/flow/flow.play.cpp
 *
 * Patch for MotoMAGX Toolchain & SDK:
 * cp squashfs-root/usr/lib/ezx/lib/libdrmfwudaclient.so* /opt/toolchains/motomagx/arm-eabi/lib/ezx-zn5/lib/
 * rm squashfs-root/usr/lib/libxml2.so.2.6
 * cp squashfs-root/usr/lib/libxml2.so* /opt/toolchains/motomagx/arm-eabi/lib/ezx-zn5/lib/
 */

// Qt
#include <qdatastream.h>
#include <qfile.h>
#include <qglobal.h>

// DRM
extern "C" {
	extern char *DRM_IsDRMFile(char *aFile);
	extern int DRM_StartRightsMeter(int *session, char *aFile, int arg1, int arg2);
	extern char *DRM_CreateConsumptionFilePath(int session, int arg1, char *aFile);
	extern int DRM_StopRightsMeter(int session);

	#define	DRM_START_RIGHTS_METER_OK 0x7D2
}

static int usage(void) {
	qDebug(
		"Information:\n"
		"\thttps://forum.motofan.ru/index.php?showtopic=1262\n" // TODO: Fix this link to actual!
		"Source code:\n"
		"\thttps://github.com/EXL/undcf\n"
		"Usage:\n"
		"\tundcf <in-file-path> <out-file-path>\n"
		"Example:\n"
		"\tundcf image.gif.dcf image.gif\n"
		"\tundcf image.drm.gif image.gif\n"
	);
	return 1;
}

static void CopyFileAux(QFile &aFileFrom, QFile &aFileTo) {
	const int lBufferSize = 4096;
	char lBuffer[lBufferSize] = { '\0' };
	int lBlockSizeAccumulator = 0;
	QDataStream lDataStreamOut(&aFileTo);
	while (!aFileFrom.atEnd()) {
		const int lBlockSize = aFileFrom.readBlock(lBuffer, lBufferSize);
		lBlockSizeAccumulator += lBlockSize;
		if (lBlockSize < 0)
			break;
		lDataStreamOut.writeRawBytes(lBuffer, lBlockSize);
		qDebug(QString("=> Written size: %1, buffer size: %2").arg(lBlockSizeAccumulator).arg(lBlockSize));
	}
}

static int CopyFile(const QString &aPathIn, const QString& aPathOut) {
	QFile lFileInput(aPathIn);
	QFile lFileOutput(aPathOut);
	if (!lFileInput.open(IO_ReadOnly)) {
		qDebug(QString("Error: Cannot open input file '%1' to read!").arg(aPathIn));
		return 0;
	}
	if (!lFileOutput.open(IO_WriteOnly)) {
		qDebug(QString("Error: Cannot open file '%1' to write!").arg(aPathOut));
		lFileInput.close();
		return 0;
	}
	CopyFileAux(lFileInput, lFileOutput);
	lFileInput.close();
	lFileOutput.close();
	return 1;
}

int main(int argc, char *argv[]) {
	qDebug("|MotoMAGX OMA DRM Hacker| by EXL, v1.0, 30-Aug-2021\n\n");
	if (argc < 3)
		return usage();

//	if (DRM_IsDRMFile(argv[1])) { // It looks like this is not being used.
		int lDrmSession = 0;
		const int lResult = DRM_StartRightsMeter(&lDrmSession, argv[1], 0, 1);
		qDebug(QString("Info: DRM_StartRightsMeter return is '%1'.").arg(lResult));
		if (lResult == DRM_START_RIGHTS_METER_OK) {
			const char *lConsumptionPath = DRM_CreateConsumptionFilePath(lDrmSession, 0, argv[1]);
			if (lConsumptionPath) {
				qDebug(QString("Info: Virtual file path for consumption is: '%1'").arg(QString(lConsumptionPath)));
				if (CopyFile(lConsumptionPath, argv[2]))
					qDebug(QString("Info: Uncrypted file '%1' was created.").arg(argv[2]));
			} else {
				qDebug("Error: Looks like DRM_CreateConsumptionFilePath() returned NULL.");
				return 1;
			}
			DRM_StopRightsMeter(lDrmSession);
		} else {
			qDebug("Error: Looks like DRM_StartRightsMeter() function failed.");
			return 1;
		}
//	} else {
//		qDebug(QString("Error: File '%1' is not a DRM file!").arg(argv[1]));
//		return 1;
//	}

	return 0;
}
