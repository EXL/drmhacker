/*
 * About:
 *   The "drmhacker_magx" utility for decrypting OMA DRM files on Motorola MotoMAGX platform.
 *
 * Author:
 *   EXL
 *
 * License:
 *   MIT
 *
 * History:
 *   11-Feb-2022: ~~~~~
 *   27-Jan-2022: Added no register mode for DRM SP uncrypto method.
 *   26-Jan-2022: Dropped qDebug() in favor to fprintf() because new MotoMAGX devices cut logs in Qt library.
 *   21-Sep-2021: Added loop for DRM_SP_ValidateRights() function.
 *   20-Sep-2021: Added some minor fixes, polish and refactoring code.
 *   14-Sep-2021: Added Media Player and Ringtone engines with finder file function.
 *   13-Sep-2021: Added loop for DRM_StartRightsMeter() function.
 *   12-Sep-2021: Added DRM_SP_* functions.
 *   30-Aug-2021: Added DRM_* functions.
 *   14-Jun-2021: First draft version.
 *
 * Flow:
 *   1. DRM_IsDRMFile() => DRM_StartRightsMeter() => DRM_CreateConsumptionFilePath() => DRM_StopRightsMeter()
 *   2. DRM_SP_SetClibDefaultAction() => DRM_SP_Register() => QFile::open() => fopen() => open()
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

// EZX
#include <ezxsound.h>
#include <MP_PlayerEngine.h>

// DRM
extern "C" {
	#define	DRM_ACTION_ALLOWED     0x7D2 // 2002, success return code.
	#define	DRM_SOME_ERROR         0x7D0 // 2000, some error return code.

	#define DRM_REQUEST_PLAY       0x00  // Probably.
	#define DRM_REQUEST_DISPLAY    0x01  // Probably.
	#define DRM_REQUEST_EXECUTE    0x02  // Probably.
	#define DRM_REQUEST_PRINT      0x03  // Probably.
	#define DRM_REQUEST_EXPORT     0x04  // Probably.

	#define DRM_SP_REQUEST_DISPLAY 0x00  // Probably.
	#define DRM_SP_REQUEST_PLAY    0x01  // Probably.
	#define DRM_SP_REQUEST_EXECUTE 0x02  // Probably.
	#define DRM_SP_REQUEST_PRINT   0x03  // Probably.
	#define DRM_SP_REQUEST_EXPORT  0x04  // Probably.

	#define DRM_SP_SUCCESS        0x00
	#define DRM_SP_ACTION_ENCRYPT 0x01

	extern int DRM_IsDRMFile(const char *aPathToFile);
	extern int DRM_StartRightsMeter(
		char **aSession,
		const char *aPathToFile,
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
	extern int DRM_SP_SetClibDefaultAction(int aAction);
}

// Defines
#ifndef NO_DEBUG
#define logd(fmt, ...)    do { fprintf(stderr, fmt, ## __VA_ARGS__); } while(0)
#else
#define logd(fmt, ...)    do { } while(0)
#endif
#define logi(fmt, ...)    do { fprintf(stdout, fmt, ## __VA_ARGS__); } while(0)
#define loge(fmt, ...)    do { fprintf(stderr, fmt, ## __VA_ARGS__); } while(0)

static int Usage(void) {
	loge(
		"Information:\n"
		"\thttps://forum.motofan.ru/index.php?showtopic=1742432\n"
		"Source code:\n"
		"\thttps://github.com/EXL/drmhacker\n"
		"Usage:\n"
		"\tdrmhacker_magx <method> <in-file-path> <out-file-path>\n"
		"Examples:\n"
		"\t./drmhacker_magx image.gif.dcf image.gif # Using Standard DRM API decryption mode (default).\n"
		"\t./drmhacker_magx image.drm.gif image.gif # Using Standard DRM API decryption mode (default).\n\n"
		"\t./drmhacker_magx drm sound.mp3.dcf sound.mp3 # Using Standard DRM API decryption mode.\n"
		"\t./drmhacker_magx drm_sp sound.mp3.dcf sound.mp3 # Using DRM SP API decryption mode.\n"
		"\t./drmhacker_magx drm_sp_reg sound.mp3.dcf sound.mp3 # Using DRM SP API decryption mode and register type.\n"
		"\t./drmhacker_magx player sound.mp3.dcf sound.mp3 # Using Media Player API decryption mode (not tested yet).\n"
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
		logi("=> Written size: '%d', buffer size: '%d'.\n", lBlockSizeAccumulator, lBlockSize);
	}
}

static int CopyFile(const QString &aPathIn, const QString& aPathOut) {
	QFile lFileInput(aPathIn);
	QFile lFileOutput(aPathOut);
	if (!lFileInput.open(IO_ReadOnly)) {
		loge("Error: Cannot open input file '%s' to read!\n", aPathIn.data());
		return 0;
	}
	if (!lFileOutput.open(IO_WriteOnly)) {
		loge("Error: Cannot open file '%s' to write!\n", aPathOut.data());
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

static int ModeMediaPlayerApiForDecrypt(const char *aPathIn, const char *aPathOut) {
	logi("Info: using Media Player API decryption mode.\n");

	loge("1\n");

	AM_NORMAL_DEV_INTERFACE *lAmNormalDevInterface = new AM_NORMAL_DEV_INTERFACE((SOUNDM_AUDIO_NORMALDEV_TYPE_ENUM_T) 2, 8000, 1, 2);

	QFileInfo f(aPathIn);

	MP_PlayerEngine *lMP_PlayerEngine = MP_CreateRingtoneEngine(lAmNormalDevInterface, NULL);

	loge("2\n");

//	MP_PlayerEngine *lMP_PlayerEngine = MP_CreatePlayerEngine(lAmNormalDevInterface, NULL);

//	lMP_PlayerEngine->open(aPathIn, false);
//	lMP_PlayerEngine->open(aPathIn, true);

	loge("2: %s\n", QFile::encodeName(f.absFilePath()).data());

	lMP_PlayerEngine->open(QFile::encodeName(f.absFilePath()));

	loge("3\n");

//	TODO: Why there is segmentation fault?

	lMP_PlayerEngine->play();

	loge("4\n");
//	lMP_PlayerEngine->play(false);
//	lMP_PlayerEngine->play(true);

	loge("5\n");

	CopyFile(FindFileInDirectoryByMask(aPathIn), aPathOut);

	loge("6\n");

	lMP_PlayerEngine->close();
	delete lMP_PlayerEngine;

	lAmNormalDevInterface->close();
	delete lAmNormalDevInterface;
	return 0;
}

static int ModeDrmSpApiForDecrypt(const char *aPathIn, const char *aPathOut, bool aRegister) {
	logi("Info: using DRM SP API decryption mode.\n");
	int lResult = -1;
	DRM_SP_SetClibDefaultAction(DRM_SP_ACTION_ENCRYPT);
	DRM_SP_Register(aPathIn, false);
	QString lMediaFileName(aPathIn);
	if (aRegister) {
		for (int i = DRM_SP_REQUEST_DISPLAY; i <= DRM_SP_REQUEST_EXPORT; ++i) {
			lResult = DRM_SP_ValidateRights(lMediaFileName, i, false);
			logi("Info: DRM_SP_ValidateRights try '%d', action '0x%08X', return is '0x%08X'.\n", i + 1, i, lResult);
			if (lResult == DRM_SP_SUCCESS)
				break;
		}
		if (lResult != DRM_SP_SUCCESS)
			loge("Error: DRM SP API method may not work correctly but copy the file anyway.\n");
	}
	CopyFile(aPathIn, aPathOut);
	return lResult;
}

static int ModeStandardDrmApiForDecrypt(const char *aPathIn, const char *aPathOut) {
	logi("Info: using Standard DRM API decryption mode.\n");
	int lResult = DRM_IsDRMFile(aPathIn);
	logi("Info: DRM_IsDRMFile returned '%d' (0x%08X).\n", lResult, lResult);
	char *lDrmSession = NULL;
	for (int i = DRM_REQUEST_PLAY; i <= DRM_REQUEST_EXPORT; ++i) {
		lResult = DRM_StartRightsMeter(&lDrmSession, aPathIn, NULL, i, NULL, false);
		logi("Info: DRM_StartRightsMeter try '%d', action '0x%08X', return is '0x%08X'.\n", i + 1, i, lResult);
		if (lResult == DRM_ACTION_ALLOWED)
			break;
		else {
			DRM_StopRightsMeter(lDrmSession);
			free(lDrmSession);
		}
	}
	if (lResult == DRM_ACTION_ALLOWED) {
		char *lConsumptionPath = DRM_CreateConsumptionFilePath(lDrmSession, false, aPathIn, 0);
		if (lConsumptionPath) {
			logi("Info: Virtual file path for consumption is: '%s'.\n", lConsumptionPath);
			if (CopyFile(lConsumptionPath, aPathOut))
				logi("Info: Uncrypted file '%s' was created.\n", aPathOut);
		} else {
			loge("Error: Looks like DRM_CreateConsumptionFilePath() returned NULL.\n");
			return 1;
		}
		free(lConsumptionPath);
		DRM_StopRightsMeter(lDrmSession);
	} else
		loge("Error: Looks like DRM_StartRightsMeter() function failed.\n");
	free(lDrmSession);
	lDrmSession = NULL;
	return 0;
}

int main(int argc, char *argv[]) {
	logi("|MotoMAGX OMA DRM Hacker| by EXL, v1.0, 27-Jan-2022\n\n");
	if (argc < 3 || argc > 4)
		return Usage();
	if (argc == 4) {
		if (strstr(argv[1], "player"))
			return ModeMediaPlayerApiForDecrypt(argv[2], argv[3]);
		else if (strstr(argv[1], "drm_sp_reg"))
			return ModeDrmSpApiForDecrypt(argv[2], argv[3], true);
		else if (strstr(argv[1], "drm_sp"))
			return ModeDrmSpApiForDecrypt(argv[2], argv[3], false);
		else
			return ModeStandardDrmApiForDecrypt(argv[2], argv[3]);
	}
	return ModeStandardDrmApiForDecrypt(argv[1], argv[2]);
}
