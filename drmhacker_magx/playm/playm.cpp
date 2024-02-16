// C
#include <cstdlib>
#include <cstdio>

// Qt
#include <qapplication.h>
#include <qobject.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qstring.h>

// EZX, MotoMAGX
#include <ezxsound.h>
#include <MP_PlayerEngine.h>

#if defined(EZX_E2) || defined(EZX_E6) || defined(EZX_A1200)
#define AM_NORMAL_DEV_INTERFACE ZAudioNormalDev
#define AM_VIRTUAL_DEV_BASE_CLASS ZAudioVirtualDev
#else
#define NORMAL_DEVICE 0xC000
// SOUNDM_AUDIOLVL_NORMALOUT_2  = 0x02 ????
#endif

class MiniPlayer : public QObject {
	Q_OBJECT

	AM_NORMAL_DEV_INTERFACE *audioOut;
	MP_PlayerEngine *playerEngine;

	bool firstSeek;
public:
	MiniPlayer(const QString &aPathToMediaFile) : firstSeek(false) {
		init(aPathToMediaFile);
		QObject::connect(playerEngine, SIGNAL(playFinished()), this, SLOT(finished()));
		QObject::connect(
			playerEngine, SIGNAL(positionChanged(unsigned int, unsigned int)),
			this, SLOT(position(unsigned int, unsigned int))
		);
	}
	void play(void) {
		// Second argument is check correctness of audio device interface.
		playerEngine->play();
//		playerEngine->play(false);
//		playerEngine->play(true);
	}
private slots:
	void finished(void) {
		fprintf(stderr, "===> Playing finished!\n");
		deinit();
	}
	void position(unsigned int newTime, unsigned int totalLenght) {
		fprintf(stderr, "====> newTime: %d, totalLenght: %d!\n", newTime, totalLenght);
		if (newTime > 10000 && !firstSeek) {
			fprintf(
				stderr, "=====> Can seek: %d, Progress: %d\n",
				playerEngine->canSeek(), playerEngine->getPlaybackPosition()
			);
			playerEngine->pause();
			playerEngine->setPositionInMS(20000); // In millis.
//			playerEngine->setPosition(80); // In percents.
			playerEngine->play();
			firstSeek = true;
		}
	}
private:
	void init(const QString &aPathToMediaFile) {
		// EZX and MotoMAGX initialization of audio device interface.
#if defined(EZX_E2) || defined(EZX_E6) || defined(EZX_A1200)
		audioOut = new AM_NORMAL_DEV_INTERFACE((SOUNDM_AUDIO_NORMALDEV_TYPE_ENUM_T) NORMAL_DEVICE, 8000, 1, 2);
#else
		audioOut = new AM_NORMAL_DEV_INTERFACE();
#endif
		// RingtoneEngine is fast, but no seek functionality there.
//		playerEngine = MP_CreateRingtoneEngine((AM_VIRTUAL_DEV_BASE_CLASS *) audioOut, NULL);
		playerEngine = MP_CreatePlayerEngine((AM_VIRTUAL_DEV_BASE_CLASS *) audioOut, NULL);

		QFileInfo audioFileInfo(aPathToMediaFile);
		QString filePath = QFile::encodeName(audioFileInfo.absFilePath());
		fprintf(stderr, "===> Path: '%s'.\n", filePath.data());

		// Second argument is obsolete.
		// bool open(const TCHAR* filename, bool headonly = false);
		playerEngine->open(filePath);
//		playerEngine->open(aPathIn, false);
//		playerEngine->open(aPathIn, true);

		// Set Volume of Media Player.
		playerEngine->setVolume(5);
	}
	void deinit(void) {
		// Close and deinit player.
		playerEngine->close();
		delete playerEngine;
		playerEngine = NULL;

		// Close audio output interface.
		audioOut->close();
		delete audioOut;
		audioOut = NULL;

		qApp->exit();
	}
};

int main(int argc, char *argv[]) {
	if (argc < 2)
		return 1;

	QApplication app(argc, argv);
	MiniPlayer miniPlayer(argv[1]);
	miniPlayer.play();
	return app.exec();
}

#include "playm.moc"
