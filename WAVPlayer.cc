#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>

#include "Display.h"
#include "WAVFile.h"
#include "WAVPlayer.h"

#ifdef __EMSCRIPTEN__

WAVPlayer::WAVPlayer(WAVFile *theFile): theFile{theFile}, numChannels{theFile->numChannels}, 
				sampleRate{theFile->sampleRate}, blockAlign{theFile->blockAlign}, 
				bytesPerSample{theFile->bytesPerSample}, 
				currentData{0, theFile->sampleRate} {
	display = nullptr;
}

#else

WAVPlayer::WAVPlayer(WAVFile *theFile): theFile{theFile}, numChannels{theFile->numChannels}, 
				sampleRate{theFile->sampleRate}, blockAlign{theFile->blockAlign}, 
				bytesPerSample{theFile->bytesPerSample}, 
	currentData{ 0, theFile->numChannels, &theFile->normData } {
	display = nullptr;
}

#endif

int WAVPlayer::getSampleNumber() {
	return currentData.currentDataIndex / (blockAlign / bytesPerSample);
}

#ifdef __EMSCRIPTEN__

EM_BOOL WAVPlayer::iteration(double time, void* data) {
	dataStruct *theData = static_cast<dataStruct*>(data);
	double currentTime = EM_ASM_DOUBLE({
			return sound.seek();
	});
	double latency = EM_ASM_DOUBLE({
		if (typeof Howler.ctx != = 'undefined') {
			if (typeof Howler.ctx.outputLatency != = 'undefined') {
				return Howler.ctx.outputLatency;
			} else if (typeof Howler.ctx.baseLatency != = 'undefined') {
				return Howler.ctx.baseLatency;
			}
		}
		return 0.0;
	});
	theData->currentDataIndex = currentTime * sampleRate;
	display->setLatency(latency);
	display->update(getSampleNumber());
	int is_finished = EM_ASM_INT({
			return finished;
		});
	if (finished) {
		return EM_FALSE;
	} else {
		return EM_TRUE;
	}
}

void WAVPlayer::play() {
	emscripten_request_animation_frame_loop(iteration, &currentData);
}

#else

int WAVPlayer::patestCallback(const void *inputBuffer, void *outputBuffer,
				unsigned long framesPerBuffer,
				const PaStreamCallbackTimeInfo* timeInfo,
				PaStreamCallbackFlags statusFlags,
				void *data) {
	float *out = (float*) outputBuffer;
	dataStruct *theData = static_cast<dataStruct*>(data);
	(void) inputBuffer;
	try {
		for (unsigned int i = 0; i < framesPerBuffer; ++i) {
			for (int j = 0; j < theData->numChannels; ++j) {
				float toOutput = (theData->data)->at(theData->currentDataIndex++);
				*out++ = toOutput;
			}
		}
	} catch (...) {
		// return paAbort;
		return paComplete;
	}
	return paContinue;
}

void WAVPlayer::play() {

	PaStream *stream;
	PaError err;
	err = Pa_Initialize();
	if (err != paNoError) throw "Portaudio failed to initialize.";
	
	err = Pa_OpenDefaultStream(&stream,
					0,
					numChannels,
					paFloat32,
					sampleRate,
					paFramesPerBufferUnspecified,
					patestCallback,
					&currentData);
	if (err != paNoError) {
		//printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
		throw "Portaudio failed to open stream.";
	}
	/*
	std::ofstream devices{"logPortAudioDevices.txt"};
	int deviceId = Pa_GetDefaultOutputDevice();
	devices << "tried to play with sample rate: " << sampleRate << std::endl;
	devices << "default deviceId: " << deviceId << std::endl;
	char deviceName[20] = "";
	int numDevices = Pa_GetDeviceCount();
	for (int i = 0; i < numDevices; ++i) {
		std::strncpy(deviceName, Pa_GetDeviceInfo(i)->name, 19);
		deviceName[19] = '\0';
		devices << deviceName << std::endl;
		devices << "  hostApi: " << Pa_GetDeviceInfo(i)->hostApi << std::endl;
		devices << "  maxInputChannels: " << Pa_GetDeviceInfo(i)->maxInputChannels << std::endl;
		devices << "  maxOutputChannels: " << Pa_GetDeviceInfo(i)->maxOutputChannels << std::endl;
		devices << "  defaultLowInputLatency: " << Pa_GetDeviceInfo(i)->defaultLowInputLatency << std::endl;
		devices << "  defaultLowOutputLatency: " << Pa_GetDeviceInfo(i)->defaultLowOutputLatency << std::endl;
		devices << "  defaultHighInputLatency: " << Pa_GetDeviceInfo(i)->defaultHighInputLatency << std::endl;
		devices << "  defaultHighOutputLatency: " << Pa_GetDeviceInfo(i)->defaultHighOutputLatency << std::endl;
		devices << "  defaultSampleRate: " << std::to_string(Pa_GetDeviceInfo(i)->defaultSampleRate) << std::endl;
	}
	devices.close();
	*/
	display->setLatency(Pa_GetStreamInfo(stream)->outputLatency);
	
	err = Pa_StartStream(stream);
	if (err != paNoError) throw "Portaudio failed to start stream.";
	//std::cout << "Playing" << std::endl;

	
	while(Pa_IsStreamActive(stream) > 0 && !display->getQuit()) {
		//Pa_Sleep(10);
		display->setLatency(Pa_GetStreamInfo(stream)->outputLatency);
		display->update(getSampleNumber());
	}
	//Pa_Sleep(5000);
	//std::cout << "Finished" << std::endl;

	/*
	err = Pa_StopStream(stream);
	if (err != paNoError) throw "Portaudio failed to stop stream.";
	*/
	
	err = Pa_AbortStream(stream);
	if (err != paNoError) throw "Portaudio failed to abort stream.";
	
	err = Pa_CloseStream(stream);
	if( err != paNoError ) throw "Portaudio failed to close stream.";

	err = Pa_Terminate();
	if (err != paNoError) throw "Portaudio failed to terminate.";
	
}

#endif
