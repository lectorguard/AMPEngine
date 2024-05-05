#pragma once
#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"
#include "SoundHelper.h"
#include "../AssetLoader/Attributes.h"
#include "vector"
#include "Text.h"
#include "stdlib.h"

#define NDKSP_MIN_VOLUME 0.0f
#define NDKSP_MAX_VOLUME 1.0f
#define MIN_VOL_MILLIBEL -5000

namespace amp
{
	class AMP_Engine;
	
// 	/// This is a amp SoundPool helper class
// 	/// 
// 	/// Each entity of this class contains one playable track (song, soundeffekt, ...)
// 	class ResourceBuffer {
// 	public:
// 		~ResourceBuffer(){
// 			free(buf);
// 			buf = nullptr;
// 		};
// 		char* buf = nullptr;
// 		int size = -1;
// 		int durationInMs = -1;
// 	};
	
	/// This is a amp SoundPool helper class
	/// 
	/// Each entity of this class is a player, which can be used to play a track
	/// @see ResourceBuffer
	class BufferQueue {
	public:
		virtual ~BufferQueue() {
			(**player)->Destroy(*player);
			player = NULL;
			playerPlay = NULL;
			queue = NULL;
			volume = NULL;
		};
		SLBufferQueueItf* queue = nullptr;
		SLObjectItf* player = nullptr;
		SLPlayItf* playerPlay = nullptr;
		SLVolumeItf* volume = nullptr;
		bool playing = false;
		bool looped = false;
		int currSampleID = -1;
		float currVolume = 1.0f;
		int currDuration = -1;
		int startPosition = -1;
	};
	
	/// This is the AMP sound pool
	/// 
	/// The sound pool can be used to load and to play sounds
	/// You can call the sound pool from everywhere, as long you have an engine reference
	/// All tracks become deleted after unloading a scene
	/// The players of the sound pool stays as long alive as the amp engine exists
	/// Just the file format .wav is allowed to play sounds
	/// You can load and play a track very easily
	/// @attention Format: .wav, stereo, 44100khz (samplinRate), 16 bit (sampleFormat) 
	/// ### Play sound example
	/// ```cpp
	/// ExampleActor::AfterWindowStartup(){
	///		//Store the index inside your actor as int
	///		soundIndex = engine->sound.load(TEXT("Sound/myAwesomeSound.wav"));
	/// }
	///
	/// ExampleActor::OnPlaySound(){
	///		engine->sound.play(soundIndex, 1.0f);
	/// }
	/// ```
	class SoundPool {
	public:
		SoundPool();
		virtual ~SoundPool();

		///Formula: bit-rate = samplingRate * sampleFormat * 2(channels stereo)
		///Duration: buf len / (sampling rate * 2(channels))
		const SLuint32 samplingRate = SL_SAMPLINGRATE_44_1;
		const SLuint32 sampleFormat = SL_PCMSAMPLEFORMAT_FIXED_16;
		const int maxStreams = 13;
		const int channels = 2;
		const SLmillisecond positionUpdate = 5u;

		/// Resets all players and deletes all tracks
		void clear();

		/// Resets certain player
		///
		/// @param streamID Is returned when calling play()
		/// @attention The from play() returned value is just valid as long the track is playing btw. played the first time (when looped)
		void clearBuffer(int streamID);

		/// Un-loops a track and resets its player (stops track from playing)
		///
		/// @param sampleId This is the id returned from load()
		/// @attention Removes all looped tracks and player with this sampleId 
		/// @attention So when a track is played multiple times looped on several players, all according players are stopped and the tracks become un-looped
		void unloopAndClear(int sampleId);

		/// Resets all player which are playing one of the listed sample ids
		/// 
		/// @param sampleIDs The sampleIds you want to stop playing
		void RemoveAllandClear(std::vector<int> sampleIDs);		
	
		/// Plays a track
		/// @param sampleId The from load() returned id of your track
		/// @param volume The volume can be between 0.0f to 1.0f from quiet to loud 
		/// @param isLooped If true, the track becomes looped. So track restarts from beginning when it is finished 
		virtual int play(int sampleId, float volume, bool isLooped = false);

		/// Loads a track by its directory path
		///
		/// @return The sampleID of the track, the id stays the same until your scene is unloaded
		/// @note When loading the same track several times, they will all have the same sampleID
		virtual int load(TEXT wavPath);

		/// Loads and interprets the track from a file path as a new SoundAttributes object 
		/// 
		/// @param wavPath The file path beginning after the assets folder from your wav file
		/// @return The SoundAttributes object, the pointer must be deleted by you when it is not used anymore (otherwise memory leak)
		SoundAttributes* readWAV(TEXT& wavPath);
	private:
		//volume between 0 and 1 
		//0: No Sound
		//1: max Sound
		SLmillibel CalculateVolumeInMillibel(float volume);
		void EnqueueSound(int sampleId, float volume, BufferQueue* avail, bool onLoop = false);
		virtual void createEngine();
		static void staticBqPlayerCallback(SLBufferQueueItf bq, void* context);
		virtual void bqPlayerCallback(SLBufferQueueItf bq, void* context);
		virtual void createBufferQueueAudioPlayer();
		void Shutdown();
		void Init();
	
		AMP_Engine* amp_engine = nullptr;
	
		// device specific min and max volumes
		SLmillibel minVolume;
		SLmillibel maxVolume;
	
		// engine interfaces
		SLObjectItf engineObject;
		SLEngineItf engineEngine;
	
		// output mix interfaces
		SLObjectItf outputMixObject;
	
		//BufferQueue queues[];
	
		// vector for BufferQueues (one for each channel)
		std::vector<BufferQueue*> bufferQueues;

		void SetEngine(AMP_Engine* engine);

		void FadeOutPlayers();

		friend class AMP_Engine;

		//Pointers do not become deleted when device goes standby or the app is not fully closed
		//When the user resumes to the app, the sound system does not need to be fully initialized again
		bool isFirst = true;

		SoundAttributes* first; 
	};
}
