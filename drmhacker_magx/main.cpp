/*
 * History:
 * 12-Sep-2021: Refactor code.
 * 30-Aug-2021: Clean version.
 * 14-Jun-2021: Draft version.
 *
 * Flow:
 * 1. DRM_IsDRMFile() => DRM_StartRightsMeter() => DRM_CreateConsumptionFilePath() => DRM_StopRightsMeter()
 * 2. DRM_SP_Register() => fopen()
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
	#define	DRM_ACTION_ALLOWED  0x7D2 // 2002
	#define DRM_REQUEST_PREVIEW 0x00
	#define DRM_REQUEST_PLAY    0x01

	extern char *DRM_IsDRMFile(char *aPathToFile);
	extern int DRM_StartRightsMeter(
		char *aSession,
		char *aPathToFile,
		char *aUnknownPtrOne,
		int aAction,
		char *aUnknownPtrTwo,
		int aUnknownBool
	);
	/*
	extern int DRM_StartRightsMeter(
		char *aSession,
		char *aPathToFile,
		int *aUnknownPtrOne,
		int aAction,
		int *aUnknownPtrTwo,
		int aUnknownBool
	);
	*/
	extern char *DRM_CreateConsumptionFilePath(
		char *aSession,
		int aUnknownBool,
		const char *aPathToFile,
		int aUnknownInt
	);
	extern int DRM_StopRightsMeter(char *aSession);
}

static int Usage(void) {
	qDebug(
		"Information:\n"
		"\thttps://forum.motofan.ru/index.php?showtopic=1262\n" // TODO: Fix this link to actual!
		"Source code:\n"
		"\thttps://github.com/EXL/drmhacker\n"
		"Usage:\n"
		"\tdrmhacker_magx <in-file-path> <out-file-path>\n"
		"Examples:\n"
		"\tdrmhacker_magx image.gif.dcf image.gif\n"
		"\tdrmhacker_magx image.drm.gif image.gif\n"
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
		return Usage();

//	if (DRM_IsDRMFile(argv[1])) { // It looks like this is not being used.
		char *lDrmSession = NULL;
		const int lResult = DRM_StartRightsMeter(&lDrmSession, argv[1], NULL, DRM_REQUEST_PLAY, NULL, false);
		qDebug(QString("Info: DRM_StartRightsMeter return is '0x%1'.").arg(QString().setNum(lResult, 16)));
		if (lResult == DRM_ACTION_ALLOWED) {
			const char *lConsumptionPath = DRM_CreateConsumptionFilePath(lDrmSession, false, argv[1], 0);
			if (lConsumptionPath) {
				qDebug(QString("Info: Virtual file path for consumption is: '%1'").arg(QString(lConsumptionPath)));
				if (CopyFile(lConsumptionPath, argv[2]))
					qDebug(QString("Info: Uncrypted file '%1' was created.").arg(argv[2]));
			} else {
				qDebug("Error: Looks like DRM_CreateConsumptionFilePath() returned NULL.");
				return 1;
			}
			free(lConsumptionPath);
			DRM_StopRightsMeter(lDrmSession);
		} else {
			qDebug("Error: Looks like DRM_StartRightsMeter() function failed.");
			return 1;
		}
		free(lDrmSession);
		lDrmSession = NULL;
//	} else {
//		qDebug(QString("Error: File '%1' is not a DRM file!").arg(argv[1]));
//		return 1;
//	}

	return 0;
}
