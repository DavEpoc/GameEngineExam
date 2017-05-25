#pragma once
//Struct that holds the RIFF data of the Wave file.
//The RIFF data is the meta data information that holds,
//the ID, size and format of the wave file
struct RIFF_Header {
	char chunkID[4];
	long chunkSize;//size not including chunkSize or chunkID
	char format[4];
};


//Struct to hold fmt subchunk data for WAVE files.
struct WAVE_Format {
	char subChunkID[4];
	long subChunkSize;
	short audioFormat;
	short numChannels;
	long sampleRate;
	long byteRate;
	short blockAlign;
	short bitsPerSample;
};

//Struct to hold the data of the wave file
struct WAVE_Data {
	char subChunkID[4]; //should contain the word data
	long subChunk2Size; //Stores the size of the data block
};


//Load wave file function. No need for ALUT with this
bool loadWavFile(const std::string filename, ALenum* format, unsigned char*& data, ALsizei* size, ALsizei* frequency) 
{
	//Local Declarations
	FILE* soundFile = NULL;
	WAVE_Format wave_format;
	RIFF_Header riff_header;
	WAVE_Data wave_data;
	//unsigned char* data;

	try {
		//soundFile = fopen(filename.c_str(), "rb");
		fopen_s(&soundFile, filename.c_str(), "rb");
		if (!soundFile)
			throw (filename);

		// Read in the first chunk into the struct
		fread(&riff_header, sizeof(RIFF_Header), 1, soundFile);

		//check for RIFF and WAVE tag in memeory
		if ((riff_header.chunkID[0] != 'R' ||
			riff_header.chunkID[1] != 'I' ||
			riff_header.chunkID[2] != 'F' ||
			riff_header.chunkID[3] != 'F') ||
			(riff_header.format[0] != 'W' ||
				riff_header.format[1] != 'A' ||
				riff_header.format[2] != 'V' ||
				riff_header.format[3] != 'E'))
			throw ("Invalid RIFF or WAVE Header");

		//Read in the 2nd chunk for the wave info
		fread(&wave_format, sizeof(WAVE_Format), 1, soundFile);
		//check for fmt tag in memory
		if (wave_format.subChunkID[0] != 'f' ||
			wave_format.subChunkID[1] != 'm' ||
			wave_format.subChunkID[2] != 't' ||
			wave_format.subChunkID[3] != ' ')
			throw ("Invalid Wave Format");

		//check for extra parameters;
		if (wave_format.subChunkSize > 16)
			fseek(soundFile, sizeof(short), SEEK_CUR);

		//Read in the the last byte of data before the sound file
		fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);
		//check for data tag in memory
		if (wave_data.subChunkID[0] != 'd' ||
			wave_data.subChunkID[1] != 'a' ||
			wave_data.subChunkID[2] != 't' ||
			wave_data.subChunkID[3] != 'a')
			throw ("Invalid data header");

		//Allocate memory for data
		data = new unsigned char[wave_data.subChunk2Size];

		// Read in the sound data into the soundData variable
		if (!fread(data, wave_data.subChunk2Size, 1, soundFile))
			throw ("error loading WAVE data into struct!");

		//Now we set the variables that we passed in with the
		//data from the structs
		*size = wave_data.subChunk2Size;
		*frequency = wave_format.sampleRate;
		//The format is worked out by looking at the number of
		//channels and the bits per sample.
		if (wave_format.numChannels == 1) {
			if (wave_format.bitsPerSample == 8)
				*format = AL_FORMAT_MONO8;
			else if (wave_format.bitsPerSample == 16)
				*format = AL_FORMAT_MONO16;
		}
		else if (wave_format.numChannels == 2) {
			if (wave_format.bitsPerSample == 8)
				*format = AL_FORMAT_STEREO8;
			else if (wave_format.bitsPerSample == 16)
				*format = AL_FORMAT_STEREO16;
		}

		cout << "dataIN: " << (data == NULL) << endl;
		/*
		//create our openAL buffer and check for success
		alGenBuffers(1, buffer);
		alGetError();
		//now we put our data into the openAL buffer and
		//check for success
		alBufferData(*buffer, *format, (void*)data,
			*size, *frequency);
		alGetError();
		*/

		//clean up and return true if successful
		fclose(soundFile);
		return true;
	}
	catch (std::string error) {
		//our catch statement for if we throw a string
		std::cerr << error << " : trying to load "
			<< filename << std::endl;
		//clean up memory if wave loading fails
		if (soundFile != NULL)
			fclose(soundFile);
		//return false to indicate the failure to load wave
		return false;
	}
}

/*
class WavData {
public:
	short format_tag;
	short* data;
	unsigned long size;
	unsigned long sample_rate;

	WavData() {
		data = NULL;
		size = 0;
	}
};

void loadWaveFile(char *fname, WavData *ret) {

	FILE* fp;
	fopen_s(&fp, fname, "rb");
	if (fp) {
		char id[5];
		unsigned long size;
		short format_tag, channels, block_align, bits_per_sample;
		unsigned long format_length, sample_rate, avg_bytes_sec, data_size;

		fread(id, sizeof(char), 4, fp);
		id[4] = '\0';

		if (!strcmp(id, "RIFF")) {
			fread(&size, sizeof(unsigned long), 1, fp);
			fread(id, sizeof(char), 4, fp);
			id[4] = '\0';

			if (!strcmp(id, "WAVE")) {
				fread(id, sizeof(char), 4, fp);
				fread(&format_length, sizeof(unsigned long), 1, fp);
				fread(&format_tag, sizeof(short), 1, fp);                 ret->format_tag = format_tag;
				fread(&channels, sizeof(short), 1, fp);                   //should check format checking num of channels and bit per sample
				fread(&sample_rate, sizeof(unsigned long), 1, fp);        ret->sample_rate = sample_rate;
				fread(&avg_bytes_sec, sizeof(unsigned long), 1, fp);
				fread(&block_align, sizeof(short), 1, fp);
				fread(&bits_per_sample, sizeof(short), 1, fp);
				fread(id, sizeof(char), 4, fp);
				fread(&data_size, sizeof(unsigned long), 1, fp);

				ret->size = data_size / sizeof(short);
				ret->data = (short*)malloc(data_size);
				fread(ret->data, sizeof(short), ret->size, fp);
			}
			else {
				cout << "Error: RIFF file but not a wave file\n";
			}
		}
		else {
			cout << "Error: not a RIFF file\n";
		}
	}
	fclose(fp);
}
*/

//http://www.dunsanyinteractive.com/blogs/oliver/?p=72
//cout << "uLong:4 " << sizeof(unsigned long) << " short:2 " << sizeof(short) << " int:4 " << sizeof(int) << " char:1 " << sizeof(char) << endl;
