#ifndef MP_PLAYER_ENGINE_H
#define MP_PLAYER_ENGINE_H

#include <qobject.h>

class Ezx_VideoDevice;
class StaticStreamInfo;
class DynamicStreamInfo;
class MediaClipInfo;
class MediaMetaInfo;

class MP_PlayerEngine : public QObject {
	Q_OBJECT

public:
	virtual bool getPCMData(
		const char *filename,
		char *buf,
		int &size,
		unsigned int beginMS,
		unsigned int endMS,
		StaticStreamInfo& sinfo
	) = 0;
	virtual bool getClipInfoQuickly(const char *filename, MediaClipInfo &sinfo) = 0;
	virtual bool getMetaInfoQuickly(const char *filename, MediaMetaInfo &metainfo) = 0;
	virtual bool getClipInfo(MediaClipInfo &clipInfo) const = 0;
	virtual bool getStaticInfo(StaticStreamInfo &sinfo) const = 0;
	virtual bool getDynamicInfo(DynamicStreamInfo &dinfo) const = 0;
	virtual bool open(const char *filename, bool headonly = false) = 0;
	virtual bool open(void *data, int size, char *ext) = 0;
	virtual bool fifoOpen(char *ext) = 0;
	virtual int fifoWrite(unsigned char *pBuf, int iSize, bool fileEnd) = 0;
	virtual int fifoGetEmptySize() = 0;
	virtual bool close() = 0;
	virtual bool play(bool bCheckAudioDevice = true) = 0;
	virtual bool pause() = 0;
	virtual bool stop() = 0;
	virtual unsigned char getStatus() const = 0;
	virtual bool setPosition(unsigned int percentage) = 0;
	virtual bool setPositionInMS(unsigned long millisecond) = 0;
	virtual unsigned int setVolume(unsigned int newVvolLevel) = 0;
	virtual unsigned int getVolume() const = 0;
	virtual bool setMute(bool state) = 0;
	virtual bool getMuteState() const = 0;
	virtual Ezx_VideoDevice *setVideoDevice(Ezx_VideoDevice *vDev) = 0;
	virtual Ezx_VideoDevice *getVideoDevice() = 0;
	virtual AM_VIRTUAL_DEV_BASE_CLASS *setAudioDevice(AM_VIRTUAL_DEV_BASE_CLASS *aDev) = 0;
	virtual AM_VIRTUAL_DEV_BASE_CLASS *getAudioDevice() = 0;
	virtual bool setProxyEnable(bool bEnable) = 0;
	virtual bool setProxyAddress(const char *strAddr) = 0;
	virtual bool setProxyPort(const unsigned int nPort) = 0;
	virtual unsigned int getPlaybackPosition() const = 0;
	virtual bool canSeek() const = 0;
	virtual unsigned short getErrorCode() const = 0;
	virtual QString getErrorDescription() const = 0;
	virtual void enableVideo(bool bEnable) = 0;
	virtual bool videoEnabled() const = 0;
	virtual bool getOpenedContentName(QString &fileName) const = 0;
	virtual bool isAudioDeviceAvailable() const = 0;
	virtual ~MP_PlayerEngine() { };
signals:
	virtual void positionChanged(unsigned int newTime, unsigned int totalLenght) = 0;
	virtual void volumeChanged(unsigned int vol_percentage) = 0;
	virtual void muteStateChanged(bool bNewState) = 0;
	virtual void playFinished() = 0;
	virtual void audioDeviceUnavailable() = 0;
	virtual void playStateChanged(unsigned char newState) = 0;
	virtual void onBuffering(unsigned int uFlags, unsigned short uPercent) = 0;
	virtual void sequenceTooBig() = 0;
	virtual void clipBeginSuccessful() = 0;
	virtual void errorOccurred(unsigned short errorCode) = 0;
	virtual void downloadProgress(unsigned int ulNewDurSoFar, unsigned int ulNewBytesSoFar) = 0;
	virtual void totalDurChanged(unsigned int ulNewTotalDur, unsigned int ulNewTotalSize) = 0;
	virtual void downloadComplete() = 0;
};

MP_PlayerEngine *MP_CreatePlayerEngine(AM_VIRTUAL_DEV_BASE_CLASS *, Ezx_VideoDevice *);
MP_PlayerEngine *MP_CreateRingtoneEngine(AM_VIRTUAL_DEV_BASE_CLASS *, Ezx_VideoDevice *);

#endif /* MP_PLAYER_ENGINE_H */
