/*
 * About:
 *   This is source code of "drmhacker_magx" utility for decrypting OMA DRM files on Motorola MotoMAGX platform.
 *
 * Author:
 *   EXL
 *
 * License:
 *   MIT
 *
 * History:
 *   20-Sep-2021: Some minor fixes and polish code.
 *   14-Sep-2021: Added Media Player and Ringtone engines with finder file function.
 *   13-Sep-2021: Added loop for DRM_StartRightsMeter() function.
 *   12-Sep-2021: Added DRM_SP_* functions.
 *   30-Aug-2021: Added DRM_* functions.
 *   14-Jun-2021: First draft version.
 *
 * Flow:
 *   1. DRM_IsDRMFile() => DRM_StartRightsMeter() => DRM_CreateConsumptionFilePath() => DRM_StopRightsMeter()
 *   2. DRM_SP_Register() => fopen()
 *
 * Information:
 *   Before building copy actual phone libraries from /usr/lib and /usr/lib/ezx/lib directories to the MotoMAGX SDK!
 */

// C
#include <cstdlib>

// Qt
#include <qdatastream.h>
#include <qdir.h>
#include <qfile.h>
#include <qglobal.h>
#include <qobject.h>

// EZX
#include <ezxsound.h>

class Ezx_VideoDevice;

class MP_PlayerEngine : public QObject {
	Q_OBJECT
public:
	virtual bool open(const char *aFileName, bool aUnknownBool = false) = 0;
	virtual bool close() = 0;
	virtual bool play(bool aUnknownBool = true) = 0;
};

MP_PlayerEngine *MP_CreatePlayerEngine(AM_VIRTUAL_DEV_BASE_CLASS *, Ezx_VideoDevice *);
MP_PlayerEngine *MP_CreateRingtoneEngine(AM_VIRTUAL_DEV_BASE_CLASS *, Ezx_VideoDevice *);

// DRM
extern "C" {
	#define	DRM_ACTION_ALLOWED    0x7D2 // 2002, success return code
	#define	DRM_SOME_ERROR        0x7D0 // 2000, some error return code

	#define DRM_REQUEST_PLAY      0x00  // Probably
	#define DRM_REQUEST_PREVIEW   0x01  // Probably
	#define DRM_REQUEST_EXECUTE   0x02  // Probably
	#define DRM_REQUEST_PRINT     0x03  // Probably
	#define DRM_REQUEST_UNKNOWN_1 0x04  // ??
	#define DRM_REQUEST_UNKNOWN_2 0x05  // ??

	#define DRM_SP_SUCCESS        0x00
	#define DRM_SP_PLAY           0x01

	extern int DRM_IsDRMFile(char *aPathToFile);
	extern int DRM_StartRightsMeter(
		char **aSession,
		char *aPathToFile,
		char *aUnknownPtrOne,
		int aAction,
		char *aUnknownPtrTwo,
		int aUnknownBool
	);
	extern char *DRM_CreateConsumptionFilePath(
		char *aSession,
		int aUnknownBool,
		const char *aPathToFile,
		int aUnknownInt
	);
	extern int DRM_StopRightsMeter(char *aSession);

	extern void DRM_SP_Register(const char *aFileName, int aReserved);
	extern void DRM_SP_UnRegister(const char *aFileName);
	extern int DRM_SP_ValidateRights(const char *aFileName, int aAction, int aUnknownBool);
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
		"Alternative (not tested yet):\n"
		"\tdrmhacker_magx player sound.mp3.dcf sound.mp3\n"
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

static QString FindFileInDirectoryByMask(const QString &aMask) {
	QDir lDirectory("/mnt/drmfs/");
	const int lFilesCount = lDirectory.count();
	for (int i = 0; i < lFilesCount; ++i)
		if (lDirectory[i].contains(aMask))
			return lDirectory[i];
	return QString::null;
}

static bool OpenPlayerForDecrypt(const char *aPathIn, const char *aPathOut) {
	AM_NORMAL_DEV_INTERFACE *lAmNormalDevInterface = new AM_NORMAL_DEV_INTERFACE();
	MP_PlayerEngine *lMP_PlayerEngine = NULL;
	if (0) { // Use Player Enigne mode.
		lMP_PlayerEngine = MP_CreatePlayerEngine(lAmNormalDevInterface, NULL);
	} else { // Use Ringtone Engine mode.
		lMP_PlayerEngine = MP_CreateRingtoneEngine(lAmNormalDevInterface, NULL);
	}
	fprintf(stderr, "1\n");
	lMP_PlayerEngine->open(aPathIn);
//	lMP_PlayerEngine->open(aPathIn, false);
//	lMP_PlayerEngine->open(aPathIn, true);
	fprintf(stderr, "2\n");
	lMP_PlayerEngine->play();
	fprintf(stderr, "3\n");
	CopyFile(FindFileInDirectoryByMask(aPathIn), aPathOut);
	fprintf(stderr, "4\n");
//	lMP_PlayerEngine->play(false);
//	lMP_PlayerEngine->play(true);
	lMP_PlayerEngine->close();
	fprintf(stderr, "5\n");
	// delete lMP_PlayerEngine; // TODO:
	lAmNormalDevInterface->close();
	// delete lAmNormalDevInterface; // TODO:
	fprintf(stderr, "6\n");
}

int main(int argc, char *argv[]) {
	qDebug("|MotoMAGX OMA DRM Hacker| by EXL, v1.0, 20-Sep-2021\n\n");
	if (argc < 3 || argc > 4)
		return Usage();
	if (argc == 4) { // Use Media Player decryption mode.
		return OpenPlayerForDecrypt(argv[2], argv[3]);
	}
	int lResult = DRM_IsDRMFile(argv[1]);
	qDebug(QString("Info: DRM_IsDRMFile returned '0x%1'.").arg(QString().setNum(lResult, 16)));
	char *lDrmSession = NULL;
	for (int i = DRM_REQUEST_PLAY; i <= DRM_REQUEST_UNKNOWN_2; ++i) {
		lResult = DRM_StartRightsMeter(&lDrmSession, argv[1], NULL, i, NULL, false);
		qDebug(QString("Info: DRM_StartRightsMeter try '%1', action '0x%2', return is '0x%3'.")
			.arg(QString().setNum(i + 1)).arg(QString().setNum(i, 16)).arg(QString().setNum(lResult, 16)));
		if (lResult == DRM_ACTION_ALLOWED)
			break;
		else {
			DRM_StopRightsMeter(lDrmSession);
			free(lDrmSession);
		}
	}
	if (lResult == DRM_ACTION_ALLOWED) {
		char *lConsumptionPath = DRM_CreateConsumptionFilePath(lDrmSession, false, argv[1], 0);
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
		qDebug("Warning: Looks like DRM_StartRightsMeter() function failed.");
		qDebug("Warning: Trying another one method.");
		DRM_SP_Register(argv[1], false);
		QString lMediaFileName(argv[1]);
		if (DRM_SP_ValidateRights(lMediaFileName, DRM_SP_PLAY, false) == DRM_SP_SUCCESS)
			CopyFile(argv[1], argv[2]);
		else
			qDebug("Error: DRM_SP method not working.");
		DRM_SP_UnRegister(argv[1]);
		return 1;
	}
	free(lDrmSession);
	lDrmSession = NULL;
	return 0;
}
