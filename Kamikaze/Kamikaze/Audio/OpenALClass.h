#pragma once
class OpenALClass : public AudioWrapper
{
private:
	ALenum error;
	const ALCchar* devices;
	const ALCchar* defaultDeviceName;
	vector<string>* devicesList;

	//Streaming properties
	ALuint** streamingBuffersList;
	int currentSoundDataPosition[MAX_BUFFERS];
	unsigned char* singleBufferStreamingData;
	bool isStreaming;
	bool isLooping;

	struct sound
	{
	public:
		ALsizei size;
		ALsizei frequency;
		ALenum  format;
		unsigned char* data;

		sound(ALenum  _format, unsigned char* _data, ALsizei _size, ALsizei _frequency)
		{
			format = _format; data = _data; size = _size; frequency = _frequency;
		}
	};

	ALCdevice *ActiveDevice;

	ALCcontext *ContextsList[MAX_CONTEXTS];
	unsigned int activeContext;

	vector<sound>* soundsList;
	unsigned int activeSound;

	ALuint* BuffersList;
	unsigned int activeBuffer;

	ALuint* sourcesList[MAX_CONTEXTS];
	unsigned int activeSource;

public:
	OpenALClass() : activeContext(0), activeSound(0), activeBuffer(0), activeSource(0), isStreaming(false), isLooping(false)
	{
		devicesList = new vector<string>();
		BuffersList = new ALuint[MAX_BUFFERS]();
		sourcesList[activeContext] = new ALuint[MAX_SOURCES_PER_CONTEXT]();
		soundsList = new vector<sound>();

		//init on setting Streaming mode
		streamingBuffersList = nullptr;
		singleBufferStreamingData = nullptr;

		for (int i = 0; i < MAX_BUFFERS; ++i)
		{
			currentSoundDataPosition[i] = 0;
		}
	};

	~OpenALClass();

	void Init();

	void selectContext(unsigned int activecontext);

	void createSound(const char* pFile);
	void selectSoundFromList(unsigned int activesound);
	void selectLastCreatedSoundFromList();

	//selects active buffer
	void Channel(unsigned int activebuffer);

	void Loading(LOADINGTYPE loadtype);

	void linkSoundToBuffer();

	void selectSource(unsigned int activesurce);

	void linkBufferToSource();

	void Pause();
	void Play();
	void Stop();

	void setLooping(bool value);
};