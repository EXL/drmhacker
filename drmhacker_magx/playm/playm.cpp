// C
#include <cstdlib>
#include <cstdio>

// Qt
#include <qapplication.h>
#include <qobject.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qstring.h>

// EZX
#include <ezxsound.h>
#include <MP_PlayerEngine.h>

#ifdef EZX_E6
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
			this, SLOT(info(unsigned int, unsigned int))
		);
	}
	void play(void) {
		fprintf(stderr, "=> 8. Play file with player/ringtone engine. Is there Segmentation Fault?\n");
		playerEngine->play();
		fprintf(stderr, "=> 9. What is first bool parameter?\n");
//		playerEngine->play(false);
//		playerEngine->play(true);
	}
private slots:
	void finished(void) {
		fprintf(stderr, "=> 11. Playing finished!\n");
		deinit();
	}
	void info(unsigned int newTime, unsigned int totalLenght) {
		fprintf(stderr, "====> newTime: %d, totalLenght: %d!\n", newTime, totalLenght);
		if (newTime > 10000 && !firstSeek) {
			fprintf(stderr, "=> 10. Can seek? %d, Seek to 95%\n", playerEngine->canSeek());
			playerEngine->pause();
			playerEngine->setPositionInMS(playerEngine->getPlaybackPosition() + 10000);
			playerEngine->play();
			firstSeek = true;
		}
	}
private:
	void init(const QString &aPathToMediaFile) {
		fprintf(stderr, "=> 1. Initialize audio interface. Why 8000 Hz? Why 1 (mono) channel? Check this later.\n");
		audioOut = new AM_NORMAL_DEV_INTERFACE((SOUNDM_AUDIO_NORMALDEV_TYPE_ENUM_T) NORMAL_DEVICE, 8000, 1, 2);

		fprintf(stderr, "=> 2. Initialize ringtone engine. Is NULL as second parameter correct?\n");
		playerEngine = MP_CreateRingtoneEngine((AM_VIRTUAL_DEV_BASE_CLASS *) audioOut, NULL);
		fprintf(stderr, "=> 3. What difference between ringtone engine and player engine?\n");
//		playerEngine = MP_CreatePlayerEngine((AM_VIRTUAL_DEV_BASE_CLASS *) audioOut, NULL);

		QFileInfo audioFileInfo(aPathToMediaFile);
		QString filePath = QFile::encodeName(audioFileInfo.absFilePath());
		fprintf(stderr, "=> 4. Get full path to audiofile! Path: '%s'.\n", filePath.data());

		fprintf(stderr, "=> 5. Open file with player/ringtone engine.\n");
		playerEngine->open(filePath);
		fprintf(stderr, "=> 6. What is second bool parameter?\n");
//		playerEngine->open(aPathIn, false);
//		playerEngine->open(aPathIn, true);

		fprintf(stderr, "=> 7. Set volume to 5!\n");
		playerEngine->setVolume(5);
	}
	void deinit(void) {
		fprintf(stderr, "=> 12. Close player now?\n");
		playerEngine->close();

		fprintf(stderr, "=> 13. Delete player/ringtone engine.\n");
		delete playerEngine;
		playerEngine = NULL;

		fprintf(stderr, "=> 14. Close audio interface.\n");
		audioOut->close();

		fprintf(stderr, "=> 15. Delete audio interface.\n");
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
