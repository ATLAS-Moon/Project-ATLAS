#include "Audio.h"

AudioHandle *AudioHandleAddr;

void AudioHandle::dumpData(){
		
	if(playOnce){
 		ERRCHECK(FMOD_System_PlaySound(localsystem, FMOD_CHANNEL_REUSE, sound, 0, &channel));
		ERRCHECK(FMOD_Channel_SetMode(channel, FMOD_LOOP_NORMAL));
		playOnce = !playOnce;
	}

	FMOD_System_GetWaveData(localsystem, WAVData, SPECTRUM, 0);
	
	for(int i = 0; i < SPECTRUM; i++){
		lines++;
		fprintf(PCMStream, "%d. | %lf\n", lines, WAVData[i]);
	}
	
}

void AudioHandle::audioFunc(){
	
	static int   looping   = 0;
	int          recording = 0;
	int          playing   = 0;
	unsigned int playpos   = 0;
	unsigned int length;
	FMOD_DSP_FFT_WINDOW  window;

	memset(&window, 0, sizeof(FMOD_DSP_FFT_WINDOW));

	if (_kbhit()) // Returns non-zero if key is pressed
	{
	    key = _getch();	// Recives keystrokes

	    switch (key)
	    {
	        case 'r' :
	        case 'R' :
	        {
	            ERRCHECK(FMOD_System_RecordStart(localsystem, recorddriver, sound, true));
				Sleep(100);	// Creates buffer in playback to prevent bugs
	            break;
	        }
	        case 'p' :
	        case 'P' :
	        {
	            if (looping)
	            {
	                FMOD_Sound_SetMode(sound, FMOD_LOOP_NORMAL);
	            }
	            else
	            {
	                FMOD_Sound_SetMode(sound, FMOD_LOOP_OFF);
	            }

	            ERRCHECK(FMOD_System_PlaySound(localsystem, FMOD_CHANNEL_REUSE, sound, 0, &channel));
	            break;
	        }
	        case 'l' :
	        case 'L' :
	        {
	            looping = !looping;
	            break;
	        }
	        case 's' :
	        case 'S' :
	        {

	            if (channel)
	            {
	                FMOD_Channel_Stop(channel);
	                channel = 0;
	            }
	            break;
	        }
	        case 'w' :
	        case 'W' :
	        {
	            printf("Writing to record.wav ...                                                     \r");

	            SaveToWav(sound);
	            Sleep(500);
	            
	            break;
	        }
			case 'd':
			case 'D':

				dump = TRUE;
				
			break;
	    }
	}

	ERRCHECK(FMOD_Sound_GetLength(sound, &length, FMOD_TIMEUNIT_PCM));

	ERRCHECK(FMOD_System_IsRecording(localsystem, recorddriver, &recording));
	
	ERRCHECK(FMOD_System_GetRecordPosition(localsystem, recorddriver, &recordpos));

	if(recording){
		dumpData();
	}

	if (channel)
	{
	    ERRCHECK(FMOD_Channel_IsPlaying(channel, &playing));
		
	    ERRCHECK(FMOD_Channel_GetPosition(channel, &playpos, FMOD_TIMEUNIT_PCM));
	}

	printf("State: %-19s. Record pos = %6d : Play pos = %6d : Loop %-3s\r", recording ? playing ? "Recording / playing" : "Recording" : playing ? "Playing" : "Idle", recordpos, playpos, looping ? "On" : "Off");

	FMOD_System_Update(localsystem);

	Sleep(10);

	WAVClient(WAVData);

}

void AudioHandle::ERRCHECK(FMOD_RESULT result){

    if (result != FMOD_OK)
    {
		printf("\n\n=================================\nFMOD error! (%d) %s\n=================================\n\n", result, FMOD_ErrorString(result));
		MessageBoxA(NULL, "A Runtime Error Has Occured, Program Asserting!", "Error", MB_ICONERROR);
        //assert(0);
    }
}

void AudioHandle::SaveToWav(FMOD_SOUND *sound){

	   FILE *fp;
	   int             channels, bits;
	   float           rate;
	   void           *ptr1, *ptr2;
	   unsigned int    lenbytes, len1, len2;
	
	   if (!sound){return;}
	
	// Get formatting data
	
	   FMOD_Sound_GetFormat  (sound, 0, 0, &channels, &bits);
	   FMOD_Sound_GetDefaults(sound, &rate, 0, 0, 0);
	   FMOD_Sound_GetLength  (sound, &lenbytes, FMOD_TIMEUNIT_PCMBYTES);
	        
	//    WAV Structures
	
	typedef struct
	{
	 signed char id[4];
	 int 		size;
	} RiffChunk;
	
	struct
	{
	    RiffChunk       chunk;
	    unsigned short	wFormatTag;    /* format type  */
	    unsigned short	nChannels;    /* number of channels (i.e. mono, stereo...)  */
	    unsigned int	nSamplesPerSec;    /* sample rate  */
	    unsigned int	nAvgBytesPerSec;    /* for buffer estimation  */
	    unsigned short	nBlockAlign;    /* block size of data  */
	    unsigned short	wBitsPerSample;    /* number of bits per sample of mono data */
	} FmtChunk  = { {{'f','m','t',' '}, sizeof(FmtChunk) - sizeof(RiffChunk) }, 1, channels, (int)rate, (int)rate * channels * bits / 8, 1 * channels * bits / 8, bits };
	
	struct
	{
	    RiffChunk   chunk;
	} DataChunk = { {{'d','a','t','a'}, lenbytes } };
	
	struct
	{
	    RiffChunk   chunk;
	 signed char rifftype[4];
	} WavHeader = { {{'R','I','F','F'}, sizeof(FmtChunk) + sizeof(RiffChunk) + lenbytes }, {'W','A','V','E'} };
	
	fp = fopen("Mic.wav", "wb");
	
	//    Write out the WAV header.
	
	fwrite(&WavHeader, sizeof(WavHeader), 1, fp);
	fwrite(&FmtChunk, sizeof(FmtChunk), 1, fp);
	fwrite(&DataChunk, sizeof(DataChunk), 1, fp);
	
	//    Lock the sound to get access to the raw data.
	
	FMOD_Sound_Lock(sound, 0, lenbytes, &ptr1, &ptr2, &len1, &len2);
	
	//    Write it to disk.
	
	fwrite(ptr1, len1, 1, fp);
	
	//    Unlock the sound to allow FMOD to use it again.
	
	FMOD_Sound_Unlock(sound, ptr1, ptr2, len1, len2);
	
	fclose(fp);
	    
}

void AudioHandle::initAudio(AudioHandle *addr){

	for(int i = 0; i < SPECTRUM; i++){
		WAVData[i] = 0;
	}

	AudioHandleAddr = addr;

	MicStream = fopen("Mic.wav", "wb");

	ERRCHECK(FMOD_System_Create(&localsystem));
	ERRCHECK(FMOD_System_Init(localsystem, 5, FMOD_INIT_NORMAL, NULL));
	
	ERRCHECK(FMOD_System_GetNumDrivers(localsystem, &num_driver));

	printf("---------------------------------------------------------\n");    
	printf("Choose a PLAYBACK driver\n");
	printf("---------------------------------------------------------\n");    
	for (count = 0; count < num_driver; count++)
	{
	    char name[256];

	    ERRCHECK(FMOD_System_GetDriverInfo(localsystem, count, name, 128, 0));

	    printf("%d : %s\n", count + 1, name);
	}

	printf("---------------------------------------------------------\n\n");
	printf("Press a corresponding number or ESC to quit\n\n>");

	do
	{
	    key = _getch();
	    if (key == 27)
	    {
	        return;
	    }
	    driver = key - '1';
	} while (driver < 0 || driver >= num_driver);
	
	ERRCHECK(FMOD_System_GetRecordNumDrivers(localsystem, &num_driver));

	printf("\n\n---------------------------------------------------------\n");    
	printf("Choose a RECORD driver\n");
	printf("---------------------------------------------------------\n");    
	for (count=0; count < num_driver; count++)
	{
	    char name[256];

	    ERRCHECK(FMOD_System_GetRecordDriverInfo(localsystem, count, name, 256, 0));

	    printf("%d : %s\n", count + 1, name);
	}
	printf("---------------------------------------------------------\n\n");
	printf("Press a corresponding number or ESC to quit\n\n>");

	recorddriver = 0;
	do
	{
	    key = _getch();
	    if (key == 27)
	    {
	        return;
	    }
	    recorddriver = key - '1';
	} while (recorddriver < 0 || recorddriver >= num_driver);

	/* !SEE TEARDOWN #1 FOR MORE INFORMATION! */

	printf("\n");

	// Init buffers

	memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));	// Callocing with a non pointer type

	exinfo.cbsize           = sizeof(FMOD_CREATESOUNDEXINFO);
	exinfo.numchannels      = 1;
	exinfo.format           = FMOD_SOUND_FORMAT_PCM16;											// WAV Data
	exinfo.defaultfrequency = 44100;															// WAV Resalution in b/sec
	exinfo.length           = exinfo.defaultfrequency * sizeof(short) * exinfo.numchannels * 5;	// 5 is number of seconds, soon to be overridden

	ERRCHECK(FMOD_System_CreateSound(localsystem, NULL, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exinfo, &sound));

	printf("\n\n===================================================================\n");
	printf("Recording example.  Copyright (c) Dioxy Labs 2013.\n");
	printf("===================================================================\n");
	printf("\n");
	printf("Press 'r' to record a 5 second segment of audio and write it to a wav file.\n");
	printf("Press 'p' to play the 5 second segment of audio.\n");
	printf("Press 'l' to turn looping on/off.\n");
	printf("Press 's' to stop recording and playback.\n");
	printf("Press 'w' to save the 5 second segment to a wav file.\n");
	printf("Press 'd' to dump PCM data\n");
	printf("Press 'Esc' to quit\n");
	printf("\n");
}

AudioHandle::AudioHandle(){

		dump			= FALSE;
		playOnce		= TRUE;
		channel			= NULL;
		num_driver		= 0;
		dumpStreamed	= 0;
		recordpos		= 0;
		lines			= 0;
	
		PCMStream = fopen("PCM.txt", "w");
}

AudioHandle::~AudioHandle(){
		printf("Audio Deconstructed");
		fclose(PCMStream);
}

void getAudioFunc(){

	AudioHandleAddr->audioFunc();
}