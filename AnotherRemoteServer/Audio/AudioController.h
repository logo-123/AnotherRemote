#ifndef AUDIO_CONTROLLER
#define AUDIO_CONTROLLER

class AudioController
{
public:
	virtual ~AudioController() {}

	virtual bool IncreaseVolume() = 0;
	virtual bool DecreaseVolume() = 0;
	virtual bool ToggleMute() = 0;
	virtual inline bool isErrored() const = 0;
    virtual inline unsigned int errCode() const = 0;
};

#endif //AUDIO_CONTROLLER
