#include "AMP_Engine.h"
#include "Event/EventSystem.h"
#include "SoundPool.h"
#include <dlfcn.h>
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <vector>

#define MILLION 1000000


amp::SoundPool::~SoundPool()
{
	Shutdown();
}

void amp::SoundPool::Shutdown()
{
	for (int i = 0; i < bufferQueues.size(); i++) {
		delete bufferQueues.at(i);
		bufferQueues.at(i) = nullptr;
	}
	bufferQueues.clear();
	
	// destroy output mix object
	if (outputMixObject != NULL) {
		(*outputMixObject)->Destroy(outputMixObject);
		outputMixObject = NULL;
	}

	// destroy engine object, and invalidate all associated interfaces
	if (engineObject != NULL) {
		(*engineObject)->Destroy(engineObject);
		engineObject = NULL;
		engineEngine = NULL;
	}
}

void amp::SoundPool::clear()
{
	//end current players
	for (int i = 0; i < bufferQueues.size(); ++i) {
		clearBuffer(i);
	}
}

void amp::SoundPool::clearBuffer(int streamID)
{
	if (streamID < 0 && streamID >= bufferQueues.size())return;
	SLresult result;
	BufferQueue* bq = bufferQueues.at(streamID);
	if (bq->playing) {
		bq->playing = false;
		bq->looped = false;
		bq->currVolume = 1.0f;
		bq->currSampleID = -1;
		bq->currDuration = -1;
		bq->startPosition = -1;
		result = (**bq->queue)->Clear(*bq->queue);
		sles_check(result);
		result = (**bq->volume)->SetVolumeLevel(*bq->volume, MIN_VOL_MILLIBEL);
		sles_check(result);
	}
}

void amp::SoundPool::unloopAndClear(int sampleId)
{
	for (int i = 0; i < bufferQueues.size(); ++i) {
		BufferQueue* bq = bufferQueues.at(i);
		if (bq->playing && bq->looped && bq->currSampleID == sampleId)clearBuffer(i);
	}
}

void amp::SoundPool::RemoveAllandClear(std::vector<int> sampleIDs)
{
	for (int i = 0; i < bufferQueues.size(); ++i) {
		BufferQueue* bq = bufferQueues.at(i);
		bool contains = false;
		for (auto& sid : sampleIDs)if(bq->currSampleID == sid)contains = true;
		if (bq->playing && contains)clearBuffer(i);
	}
}

amp::SoundPool::SoundPool() : engineObject(NULL), outputMixObject(NULL)
{
}

void amp::SoundPool::Init()
{
	//Just one init when app starts
	if (!isFirst)return;
	isFirst = false;
	createEngine();
	createBufferQueueAudioPlayer();
	minVolume = SL_MILLIBEL_MIN;
	if (minVolume < MIN_VOL_MILLIBEL)minVolume = MIN_VOL_MILLIBEL;
	maxVolume = 0;
}

void amp::SoundPool::createEngine()
{
	SLresult result;

	const SLInterfaceID engine_ids[] = { SL_IID_ENGINE };
	const SLboolean engine_req[] = { SL_BOOLEAN_TRUE };

	result = slCreateEngine(&engineObject, 0, NULL, 0, engine_ids, engine_req);
	sles_check(result);

	result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
	sles_check(result);

	result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
	sles_check(result);

	const SLInterfaceID ids[1] = { SL_IID_NULL };
	const SLboolean req[1] = { SL_BOOLEAN_FALSE };

	result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
	sles_check(result);

	result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
	sles_check(result);
}

void amp::SoundPool::createBufferQueueAudioPlayer()
{
	SLresult result;

	SLDataLocator_AndroidSimpleBufferQueue loc_buf_q = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,2 };
	SLDataFormat_PCM format_pcm = { SL_DATAFORMAT_PCM,2,samplingRate, sampleFormat, sampleFormat, SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, SL_BYTEORDER_LITTLEENDIAN };

	SLDataSource audioSrc = { &loc_buf_q, &format_pcm };

	const SLInterfaceID player_ids[] = { SL_IID_BUFFERQUEUE, SL_IID_PLAY, SL_IID_VOLUME };
	const SLboolean player_req[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

	for (int i = 0; i < maxStreams; ++i)
	{
		BufferQueue* bf = new BufferQueue();
		bf->playing = false;
		bf->queue = new SLBufferQueueItf();
		bf->player = new SLObjectItf();
		bf->playerPlay = new SLPlayItf();
		bf->volume = new SLVolumeItf();

		//configure audio sink
		SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, outputMixObject };
		SLDataSink audioSnk = { &loc_outmix, NULL };

		//Create Simple Buffer Queue
		result = (*engineEngine)->CreateAudioPlayer(engineEngine, &*bf->player, &audioSrc, &audioSnk, 3, player_ids, player_req);
		sles_check(result);

		// realize the player
		result = (**bf->player)->Realize(*bf->player, SL_BOOLEAN_FALSE);
		sles_check(result);

		//get the play interface
		result = (**bf->player)->GetInterface(*bf->player, SL_IID_PLAY, &*bf->playerPlay);
		sles_check(result);

		//get the buffer queue interface
		result = (**bf->player)->GetInterface(*bf->player, SL_IID_BUFFERQUEUE, &*bf->queue);
		sles_check(result);

		result = (**bf->playerPlay)->SetPositionUpdatePeriod(*bf->playerPlay, positionUpdate);
		sles_check(result);


		//register Callbacks
		result = (**bf->queue)->RegisterCallback(*bf->queue, SoundPool::staticBqPlayerCallback, this);
		sles_check(result);

		result = (**bf->player)->GetInterface(*bf->player, SL_IID_VOLUME, &*bf->volume);
		sles_check(result);

		if (i == 0) {
			result = (**bf->volume)->GetMaxVolumeLevel(*bf->volume, &maxVolume);
			sles_check(result);
		}

		result = (**bf->playerPlay)->SetPlayState(*bf->playerPlay, SL_PLAYSTATE_PLAYING);
		sles_check(result);

		bufferQueues.push_back(bf);
	}
}

void amp::SoundPool::SetEngine(AMP_Engine* engine)
{
	amp_engine = engine;
}

void amp::SoundPool::FadeOutPlayers()
{
	SLresult result;
	SLmillisecond position;
	for (BufferQueue* bq : bufferQueues) {
		if(!bq->playing)continue;

		result = (**bq->playerPlay)->GetPosition(*bq->playerPlay, &position);
		sles_check(result);

		float progress = (position - bq->startPosition) / (float)bq->currDuration;
 
		SLmillibel currVolume;
		if (progress > 0.8) {
			
			result = (**bq->volume)->GetVolumeLevel(*bq->volume, &currVolume);
			sles_check(result);

			float a = (1.0f - (progress - 0.8f) / 0.2f);
			SLmillibel newVolumeLevel = CalculateVolumeInMillibel(a * bq->currVolume);

			result = (**bq->volume)->SetVolumeLevel(*bq->volume, newVolumeLevel);
			sles_check(result);
		}
	}
}

int amp::SoundPool::play(int sampleId, float volume, bool isLooped /*= false*/)
{
	int streamId = 0;

	BufferQueue* avail = nullptr;
	int i = 0;
	for (; i < bufferQueues.size(); i++) {
		if (!bufferQueues.at(i)->playing) {
			avail = bufferQueues.at(i);
			avail->playing = true;
			avail->looped = isLooped;
			avail->currSampleID = sampleId;
			avail->currVolume = volume;
			streamId = i;
			break;
		}
	}

	if (avail)EnqueueSound(sampleId, volume, avail);
	//else : Sound already playing or does not exist
	return streamId;
}

void amp::SoundPool::EnqueueSound(int sampleId, float volume, BufferQueue* avail, bool onLoop /*= false*/)
{
	assert(amp_engine);
	SoundAttributes* soundAttr = nullptr;
	if (auto result = amp_engine->currentScene.AssetAttribMap.tryFind(sampleId)) {
		soundAttr = (SoundAttributes*)*result;
	}
	else throw "You have to call load before play or you have not passed the sampleID to play, you received from load ";

	avail->currDuration = soundAttr->durationInMS;

	SLresult result;
	SLmillibel newVolume = CalculateVolumeInMillibel(volume);

	SLmillisecond position;
	result = (**avail->playerPlay)->GetPosition(*avail->playerPlay, &position);
	sles_check(result);
	avail->startPosition = position;

	//adjust volume for the buffer queue
	result = (**avail->volume)->SetVolumeLevel(*avail->volume, newVolume);
	sles_check(result);


	result = (**avail->queue)->Enqueue(*avail->queue, (void*)(short*)soundAttr->buf, soundAttr->size);
	sles_check(result);
}

SLmillibel amp::SoundPool::CalculateVolumeInMillibel(float volume)
{
	return ((minVolume - maxVolume) * (1.0f - volume)) + maxVolume;
}

// .wav files
int amp::SoundPool::load(TEXT wavPath)
{
	if (!amp_engine)throw "engine is not initialized";
	SoundAttributes* soundAttr = nullptr;
	if (auto result = amp_engine->currentScene.AssetAttribMap.tryFind(wavPath.getHash())) {
		soundAttr = (SoundAttributes*)*result;
	}
	else {
		soundAttr = readWAV(wavPath);
		soundAttr->index = wavPath.getHash();
		soundAttr->durationInMS = soundAttr->size / ((samplingRate / (float)MILLION) * (float)channels);
		amp_engine->currentScene.AssetAttribMap.add(wavPath.getHash(), soundAttr);
		return wavPath.getHash();
	}
	return soundAttr->index;
}

SoundAttributes* amp::SoundPool::readWAV(TEXT& wavPath)
{
	FILE* file = amp_engine->assetLoader.getRawFile(wavPath.getCharArray());
	fseek(file, 0, SEEK_END);
	int length = ftell(file);
	fseek(file, 0, SEEK_SET);

	int len = 0;
	char* buf = nullptr;
	int indx = 0;
	int headersLength = 0;
	const char* dataHdr = "data";
	int dataHdrLength = 4;
	int dataHdrIndx = 0;
	bool readingRaw = false;
	int countdown = 0;

	for (int i = 0; i < length; i++) {
		char c = getc(file);
		if (readingRaw) {
			if (countdown == 0) {
				buf[indx] = c;
				indx++;
			}
			else {
				countdown--;
			}
		}
		else {
			headersLength++;
			if (dataHdr[dataHdrIndx] == c) {
				if (dataHdrIndx == dataHdrLength - 1) {
					readingRaw = true;
					buf = (char*)malloc(length - headersLength - 4);
					countdown = 4;
				}
				else {
					dataHdrIndx++;
				}
			}
			else {
				dataHdrIndx = 0;
			}
		}
	}
	fclose(file);
	len = length - headersLength - 4;

	SoundAttributes* sa = new SoundAttributes();
	sa->buf = buf;
	sa->size = len;
	return sa;
}

void amp::SoundPool::staticBqPlayerCallback(SLBufferQueueItf bq, void* context)
{
	if (context) {
		SoundPool* currentPool = (SoundPool*)context;
		currentPool->bqPlayerCallback(bq, context);
	}
}

void amp::SoundPool::bqPlayerCallback(SLBufferQueueItf bq, void* context)
{
	BufferQueue* avail = NULL;
	for (auto& curr_bq : bufferQueues) {
		if (*curr_bq->queue == bq) {
			avail = curr_bq;
			if (avail->looped)play(avail->currSampleID, avail->currVolume, true);
			avail->playing = false;
			avail->looped = false;
			break;
		}
	}
	SLresult result;
	result = (**avail->volume)->SetVolumeLevel(*avail->volume, MIN_VOL_MILLIBEL);
	sles_check(result);

}


