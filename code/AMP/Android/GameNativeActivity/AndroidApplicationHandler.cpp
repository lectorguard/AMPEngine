
#include "AndroidApplicationHandler.h"
#include "EGL/egl.h"
#include "assert.h"


Android::AndroidApplicationHandler::AndroidApplicationHandler()
{
}

void Android::AndroidApplicationHandler::android_handle_cmd(struct android_app* app, int32_t cmd) {
	AndroidApplicationHandler* apkHandler = (AndroidApplicationHandler*)app->userData;
	//native activity callbacks
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		Save(apkHandler);
		break;
	case APP_CMD_INIT_WINDOW:
		if (apkHandler->app->window != NULL) {
			InitWindow(apkHandler);
		}
		break;
	case APP_CMD_TERM_WINDOW:
		if (apkHandler->app->window != NULL) {
			TermWindow(apkHandler);
		}
		break;
	case APP_CMD_GAINED_FOCUS:
		//Monitor acceleration sensor
		if (apkHandler->accelerometerSensor != NULL) {
			RestartSensorEventQueue(apkHandler);
		}
		if (apkHandler->app->window != NULL && apkHandler->ProcessShutdown) {
			InitWindow(apkHandler);
		}
		break;
	case APP_CMD_LOST_FOCUS: {
		HandleLostFocus(apkHandler);
		break;
		}
	case APP_CMD_PAUSE: {
		Handle_Stop(apkHandler);
		break;
		}	
	}	
}

void Android::AndroidApplicationHandler::Handle_Stop(AndroidApplicationHandler* apkHandler)
{
	apkHandler->currentCommand = amp::APP_CMD_STOP;
	PrepareShutdownAPK(apkHandler);
}

int32_t Android::AndroidApplicationHandler::android_handle_input(struct android_app* app, AInputEvent* event) {
	AndroidApplicationHandler* apkHandler = (AndroidApplicationHandler*)app->userData;
	
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		apkHandler->saveState.x = AMotionEvent_getX(event, 0);
		apkHandler->saveState.y = AMotionEvent_getY(event, 0);
		int32_t action = AMotionEvent_getAction(event);
		auto& state = apkHandler->saveState;
		if (action == AMOTION_EVENT_ACTION_UP)state.isPressed = false;
		else if(action == AMOTION_EVENT_ACTION_DOWN)state.isPressed = true;
		return 1;
	}
	else return 0;
}

bool Android::AndroidApplicationHandler::updateHandlerAndroid(amp::AndroidStatus* status) {	
	int ident;
	int events;
	struct android_poll_source* source = nullptr;
	// events and android native activity communication is handled in this while loop
	while ((ident = ALooper_pollAll(animating ? 0 : -1, NULL, &events,
		(void**)&source)) >= 0) {
		// Event processing
		if (source != NULL) {
			source->process(app, source);
		}
		//Process sensor data
		if (ident == LOOPER_ID_USER) {
			if (accelerometerSensor != NULL) {
				ASensorEvent event;
				while (ASensorEventQueue_getEvents(sensorEventQueue,
					&event, 1) > 0) {
				}
			}
		}
		//Termination check
		if (app->destroyRequested != 0) {
			status->commands = amp::CommandsAndroid::APP_CMD_TERM_WINDOW;
			ProcessShutdown = 0;
			return false;
		}
		//Focus is lost, while loop should not be left (otherwise app can't get back focus) 
		//Just called once until gained focus event set the apk up again
		if (ProcessShutdown) {
			status->ProcessShutdown = 1;
			status->commands = currentCommand;
			currentCommand = amp::CommandsAndroid::NO_CMD;
			return true;
		}
	}
	status->animating = animating;
	if (currentCommand == APP_CMD_INIT_WINDOW) {
		status->window = app->window;
	}
	status->commands = currentCommand;

	status->touchChanged = saveState.isPressed != saveState.lastIsPressed;
	status->touchEvent = saveState.isPressed;
	status->TouchX = saveState.x;
	status->TouchY = saveState.y;
	saveState.lastIsPressed = saveState.isPressed;

	currentCommand = amp::CommandsAndroid::NO_CMD;
	return true;
}

void Android::AndroidApplicationHandler::InitializeAndroidHandler(android_app* state)
{
	state->userData = this;
	state->onAppCmd = android_handle_cmd;
	state->onInputEvent = android_handle_input;
	app = state;
	// Waiting for acceleration sensor
	sensorManager = ASensorManager_getInstance();
	accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager,
		ASENSOR_TYPE_ACCELEROMETER);
	sensorEventQueue = ASensorManager_createEventQueue(sensorManager,
		app->looper, LOOPER_ID_USER, NULL, NULL);
	if (app->savedState != NULL) {
		//load save state
		this->saveState = *(struct saved_state*)app->savedState;
	}
	animating = 1;
}

void Android::AndroidApplicationHandler::Save(AndroidApplicationHandler* apkHandler)
{
	//Create Save State
	apkHandler->app->savedState = malloc(sizeof(struct saved_state));
	*((struct saved_state*)apkHandler->app->savedState) = apkHandler->saveState;
	apkHandler->app->savedStateSize = sizeof(struct saved_state);
	PrepareShutdownAPK(apkHandler);
}

void Android::AndroidApplicationHandler::PrepareShutdownAPK(AndroidApplicationHandler* apkHandler)
{
	//No animations anymore, graphic memory access is lost
	apkHandler->animating = 0;
	apkHandler->ProcessShutdown = 1;
}

void Android::AndroidApplicationHandler::TermWindow(AndroidApplicationHandler* apkHandler)
{
	apkHandler->currentCommand = amp::APP_CMD_TERM_WINDOW;
	PrepareShutdownAPK(apkHandler);
}

void Android::AndroidApplicationHandler::HandleLostFocus(AndroidApplicationHandler* apkHandler)
{
	// Stop monitoring acceleration sensor
	// Better battery performance when lost focus
	if (apkHandler->accelerometerSensor != NULL) {
		ASensorEventQueue_disableSensor(apkHandler->sensorEventQueue, apkHandler->accelerometerSensor);
	}
	if(apkHandler->currentCommand != amp::APP_CMD_STOP)apkHandler->currentCommand = amp::APP_CMD_LOST_FOCUS;
	PrepareShutdownAPK(apkHandler);
}

void Android::AndroidApplicationHandler::InitWindow(AndroidApplicationHandler* apkHandler)
{
	apkHandler->currentCommand = amp::APP_CMD_INIT_WINDOW;
	PrepareInitAPK(apkHandler);
}

void Android::AndroidApplicationHandler::PrepareInitAPK(AndroidApplicationHandler* apkHandler)
{
	apkHandler->animating = 1;
	apkHandler->ProcessShutdown = 0;
}

void Android::AndroidApplicationHandler::RestartSensorEventQueue(AndroidApplicationHandler* apkHandler)
{
	ASensorEventQueue_enableSensor(apkHandler->sensorEventQueue,
		apkHandler->accelerometerSensor);
	//60 fps sensor rate
	ASensorEventQueue_setEventRate(apkHandler->sensorEventQueue,
		apkHandler->accelerometerSensor, (1000L / 60) * 1000);
}
