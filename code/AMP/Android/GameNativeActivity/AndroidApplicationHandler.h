#pragma once

#include "AMP_Engine.h"
#include <malloc.h>


namespace Android
{
	/// This is an optional save state
	struct saved_state {
		float angle;
		int32_t x = -1;
		int32_t y = -1;
		bool isPressed = false;
		bool lastIsPressed = false;
	};
	
	/// This is the android application handler
	/// 
	/// This handler is the communication layer between android and amp
	/// All status updates for the engine are performed from here
	class AndroidApplicationHandler
	{
	public:
		AndroidApplicationHandler();

		AndroidApplicationHandler(const AndroidApplicationHandler&) = delete;
		AndroidApplicationHandler(AndroidApplicationHandler&&) = delete;

		/// This is the update handler for the amp engine
		/// 
		/// The update handler must be updated once per frame from the game to exchange information, inputs and outputs from android
		/// @param status This is the overall status of the android application, the status contains also inputs from android like touch inputs
		/// @return If the android wants to perform a shutdown of this application false is returned and the game loop breaks and the engine shuts down
		bool updateHandlerAndroid(amp::AndroidStatus* status);

		/// The android application handler have to be initialized before it can be used
		/// 
		/// This function sets callbacks and initializes the sensors for receiving touch callbacks from the android native application
		/// @param state The android native application pointer 
		void InitializeAndroidHandler(struct android_app* state);
	private:
		static void android_handle_cmd(struct android_app* app, int32_t cmd);

		static void Handle_Stop(AndroidApplicationHandler* apkHandler);

		static void Save(AndroidApplicationHandler* apkHandler);
		static void PrepareShutdownAPK(AndroidApplicationHandler* apkHandler);
		static void TermWindow(AndroidApplicationHandler* apkHandler);
		static void HandleLostFocus(AndroidApplicationHandler* apkHandler);
		static void InitWindow(AndroidApplicationHandler* apkHandler);
		static void PrepareInitAPK(AndroidApplicationHandler* apkHandler);
		static void RestartSensorEventQueue(AndroidApplicationHandler* apkHandler);
		static int32_t android_handle_input(struct android_app* app, AInputEvent* event);

		struct android_app* app = nullptr;
		ASensorManager* sensorManager = nullptr;
		const ASensor* accelerometerSensor = nullptr;
		ASensorEventQueue* sensorEventQueue = nullptr;
		amp::CommandsAndroid currentCommand = amp::NO_CMD;
		//updating actors and components
		int animating = 0;
		//when focus is lost a clean shutdown is executed followed by an init
		//When focus is lost no graphic memory is accessible for the apk (=> Shutdown)
		int ProcessShutdown = 0;
		struct saved_state saveState;
	};
}



