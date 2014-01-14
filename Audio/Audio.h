#pragma once

#include <stdio.h>
#include <assert.h>
#include <conio.h>

#include "Graphics.h"

#include "../../api/inc/fmod.hpp"
#include "../../api/inc/fmod_dsp.h"
#include "../../api/inc/fmod_errors.h"

#define SPECTRUM	2048

class GraphicsHandle;

class AudioHandle{

	private:
		
		GraphicsHandle			*localGraphics;
		FMOD_SYSTEM				*localsystem;
		FMOD_SOUND				*sound;
		FMOD_CREATESOUNDEXINFO	exinfo;
		FMOD_CHANNEL			*channel;
		FILE					*MicStream, *PCMStream;
		BOOL					dump, playOnce, dumped;
		char					key;
		int						num_driver, count, driver, recorddriver, dumpStreamed, *loopcounts;
		float					WAVData[SPECTRUM];
		unsigned int			recordpos;
		long int				lines;
		
		void ERRCHECK(FMOD_RESULT result);
		void SaveToWav(FMOD_SOUND *sound);
		~AudioHandle();

	public:
		
		AudioHandle();
		void dumpData();
		void audioFunc();
		void initAudio(AudioHandle *addr);
				
};

void getAudioFunc();