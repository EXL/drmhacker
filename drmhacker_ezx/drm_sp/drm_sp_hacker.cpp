/*
 * About:
 *  The "drm_sp_hacker" utility for decrypting OMA DRM files on Motorola EZX platform through DRM SP API.
 *
 * Author:
 *   EXL
 *
 * License:
 *   MIT
 *
 * History:
 *   01-Dec-2021: First draft version.
 */

// Qt
#include <qfile.h>
#include <qstring.h>
#include <qglobal.h>
#include <qdatastream.h>

// DRM
enum DRM_SP_RIGHTS_TYPE_S {
	DRM_SP_REQUEST_DISPLAY,
	DRM_SP_REQUEST_PLAY,
	DRM_SP_REQUEST_EXECUTE,
	DRM_SP_REQUEST_PRINT,
	DRM_SP_REQUEST_EXPORT
};

enum DRM_SP_BOOL_S {
	DRM_FALSE,
	DRM_TRUE
};

extern int DRM_SP_ValidateRights(const QString &aFileName, DRM_SP_RIGHTS_TYPE_S aAction, DRM_SP_BOOL_S aUnknownBool);

#define DRM_SP_REQUEST_DISPLAY 0x00  // Probably
#define DRM_SP_REQUEST_PLAY    0x01  // Probably
#define DRM_SP_REQUEST_EXECUTE 0x02  // Probably
#define DRM_SP_REQUEST_PRINT   0x03  // Probably
#define DRM_SP_REQUEST_EXPORT  0x04  // Probably

#define DRM_SP_SUCCESS        0x00
#define DRM_SP_ACTION_ENCRYPT 0x01

extern "C" {
	extern void DRM_SP_Register(const char *aFileName, int aReserved);
	extern void DRM_SP_UnRegister(const char *aFileName);
	extern int DRM_SP_SetClibDefaultAction(int aAction);
}

static int Usage(void) {
	qDebug(
		"Source code:\n"
		"\thttps://github.com/EXL/drmhacker\n"
		"Usage:\n"
		"\tdrm_sp_hacker <in-file-path> <out-file-path>\n"
		"Examples:\n"
		"\t./drm_sp_hacker file.drm.jar file.jar\n"
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

static int ModeDrmSpApiForDecrypt(const char *aPathIn, const char *aPathOut) {
	qDebug("Info: using DRM SP API decryption mode.");
	int lResult = -1;
#if !defined(EZX_E680) && !defined(EZX_E680I)
	DRM_SP_SetClibDefaultAction(DRM_SP_ACTION_ENCRYPT);
#endif
	DRM_SP_Register(aPathIn, false);
	QString lMediaFileName(aPathIn);
	for (int i = DRM_SP_REQUEST_DISPLAY; i <= DRM_SP_REQUEST_EXPORT; ++i) {
		lResult = DRM_SP_ValidateRights(lMediaFileName, (DRM_SP_RIGHTS_TYPE_S) i, (DRM_SP_BOOL_S) false);
		qDebug(QString("Info: DRM_SP_ValidateRights try '%1', action '0x%2', return is '0x%3'.")
			.arg(QString().setNum(i + 1)).arg(QString().setNum(i, 16)).arg(QString().setNum(lResult, 16)));
		if (lResult == DRM_SP_SUCCESS)
			break;
	}
	if (lResult == DRM_SP_SUCCESS)
		CopyFile(aPathIn, aPathOut);
	else {
		qDebug("Error: DRM SP API method not working.");
		lResult = 1;
	}
	return lResult;
}

int main(int argc, char *argv[]) {
	qDebug("|MotoEZX OMA DRM Hacker| by EXL, v1.0, 01-Dec-2021\n\n");
	if (argc != 3)
		return Usage();
	return ModeDrmSpApiForDecrypt(argv[1], argv[2]);
}
