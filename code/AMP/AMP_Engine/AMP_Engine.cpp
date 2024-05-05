#include "Static\Pixel.h"
#include "AMP_Engine.h"
#include <android\native_window.h>



amp::AMP_Engine::~AMP_Engine()
{
}

void amp::AMP_Engine::SetScreenDPI(ANativeActivity* activity)
{
	AConfiguration* config = AConfiguration_new();
	AConfiguration_fromAssetManager(config, activity->assetManager);
	int32_t density = AConfiguration_getDensity(config);
	int32_t dpX = AConfiguration_getScreenWidthDp(config);
	int32_t dpY = AConfiguration_getScreenHeightDp(config);
	AConfiguration_delete(config);
	config = nullptr;
	PIX::setDensity(density);
	PIX::setDIPX(dpX);
	PIX::setDIPY(dpY);
}

void amp::AMP_Engine::SetAssetManager(AAssetManager* assetManager)
{
	ampShader.setAssetManager(assetManager);
	this->assetManager = assetManager;
	assetLoader.setAssetManager(assetManager);
	animLoader.setAssetManager(assetManager);
}

void amp::AMP_Engine::run(std::function<bool(AndroidStatus*)> AndroidStatusInput)
{
	AfterSystemStartup();
	SystemStartupCallback();
	AndroidStatus status;
	isRuntime = true;
	while (AndroidStatusInput(&status))
	{
		startTime = TIME::currentTimeInNanos();
		if (status.ProcessShutdown) {
			ProcessShutdown(status);
		}
		if (status.commands == APP_CMD_INIT_WINDOW) {
			StartSystemsWhenNecessary(status);
		}
		if (status.commands == APP_CMD_LOST_FOCUS) {
			GameUpdate();
			animating = false;
		}
		ProcessTouchEvents(status);
		animating = status.animating;	
		if (animating && hasDisplay) {
			GameUpdate();
		}
		frameTime = TIME::NanosToSeconds(TIME::currentTimeInNanos() - startTime);
	}	
	Shutdown();
}

void amp::AMP_Engine::StartSystemsWhenNecessary(AndroidStatus& status)
{
	if (isFullStartUpNecessary) {
		isFullStartUpNecessary = false;
		ampContext.initDisplay(status.window);
		InitSystems();
	}
	animating = true;
}

void amp::AMP_Engine::ProcessShutdown(AndroidStatus& status)
{
	if (isFullStartUpNecessary)return;
	status.ProcessShutdown = 0;
	if (status.commands == APP_CMD_STOP) {
		PrepareShutdown();
		isFullStartUpNecessary = true;
	}
	animating = false;
}

void amp::AMP_Engine::Shutdown()
{
	BeforeSystemShutdownCallback();
	currentScene.clear(false);
	BeforeSystemShutdown();
	physics.destroyDynamicWorld();
	ampContext.termDisplay();
}

void amp::AMP_Engine::GameUpdate()
{
	physics.integrateDynamicPhysics();
	UpdateCallback();
	frameBuffer.clear(true);
	publishSceneActorComponentsUpdate();
	PostSceneUpdateCallback();
	publishUIActorComponentsUpdate();
	frameBuffer.drawFrameBufferContent();
	update();
	// performance measurement
	calculationTime = TIME::NanosToSeconds(TIME::currentTimeInNanos() - startTime);
	ampContext.SwapBuffers();
}

void amp::AMP_Engine::PublishTouchEvents(AndroidStatus& status)
{
	TouchX = status.TouchX;
	TouchY = status.TouchY;
	OnTouchCallback(status.TouchX, status.TouchY);
	OnTouch(status.TouchX, status.TouchY);
}

void amp::AMP_Engine::ProcessTouchEvents(AndroidStatus& status) {
	if (!animating)return;
	if (status.touchChanged) {
		if (status.touchEvent)StartTouch(status.TouchX, status.TouchY);
		else EndTouch(status.TouchX, status.TouchY);
	}
	else if (status.touchEvent) {
		PublishTouchEvents(status);
	}
}

void amp::AMP_Engine::StartTouch(int32_t x, int32_t y)
{
	TouchX = x;
	TouchY = y;
	events.publishEvent(startTouch, x, y);
}

void amp::AMP_Engine::EndTouch(int32_t x, int32_t y)
{
	TouchX = x;
	TouchY = y;
	events.publishEvent(endTouch, x, y);
}

void amp::AMP_Engine::InitSystems()
{
	this->hasDisplay = true;
	sound.SetEngine(this);
	sound.Init();
	//createPhysics
	physics.SetEngine(this);
	physics.createDynamicWorld();
	AfterWindowStartupCallback();
	AfterWindowStartup();
	frameBuffer.enable(this);
}

void amp::AMP_Engine::PrepareShutdown()
{
	BeforeSystemShutdownCallback();
	currentScene.clear(false);
	BeforeSystemShutdown();
	ampContext.termDisplay();
	hasDisplay = false;
}

double amp::AMP_Engine::getFrameTime()
{
	if (frameTime <= 0.0 || frameTime > 1.0)return 1.0;
	return frameTime;
}

double amp::AMP_Engine::getCalculationTime()
{
	if (!calculationTime)return 1.0;
	return calculationTime;
}

void amp::AMP_Engine::addTimerFunction(TimeEvent timeEvent)
{
	events.addTimerFunction(timeEvent);
}

void amp::AMP_Engine::removeTimerFunction(TimeEvent& timeEvent)
{
	events.removeTimerFunction(timeEvent);
}

amp::Actor* amp::AMP_Engine::getSceneActor(int index)
{
	assert(index >= 0 && index < currentScene.CurrentSceneActorArray.count() && "Invalid Index");
	auto* actor = currentScene.CurrentSceneActorArray[index].get();
	return static_cast<Actor*>(actor);
}


amp::Actor* amp::AMP_Engine::getSceneActorByName(TEXT name)
{
	for (int i = 0; i < getSceneCount(); ++i) {
		if (getSceneActor(i)->GetName() == name)return getSceneActor(i);
	}	throw "There is no Scene Actor with such a name";
}

int amp::AMP_Engine::getSceneCount()
{
	return currentScene.CurrentSceneActorArray.count();
}

amp::Actor* amp::AMP_Engine::getUIActor(int index)
{
	assert(index >= 0 && index < currentScene.CurrentSceneUIArray.count() && "Invalid Index");
	auto* actor = currentScene.CurrentSceneUIArray[index].get();
	return static_cast<Actor*>(actor);
}

amp::Actor* amp::AMP_Engine::getUIActorByName(TEXT name)
{
	for (int i = 0; i < getUICount(); ++i) {
		if (getUIActor(i)->GetName() == name)return getUIActor(i);
	}
	throw "There is no UI Actor with such a name";
}

int amp::AMP_Engine::getUICount()
{
	return currentScene.CurrentSceneUIArray.count();
}

void amp::AMP_Engine::SystemStartupCallback()
{
	publishActorEvent([](Actor* a) {a->AfterSystemStartup(); });
	publishUIEvent([](Actor* a) {a->AfterSystemStartup(); });
}


void amp::AMP_Engine::AfterWindowStartupCallback()
{
	publishActorEvent([](Actor* a) {a->AfterWindowStartup(); a->OnActivate(); });
	publishUIEvent([](Actor* a) {a->AfterWindowStartup(); a->OnActivate(); });
}

void amp::AMP_Engine::UpdateCallback()
{
	events.publishTimeEvent();
	publishActorEvent([](Actor* a) {a->UpdateActor(); });
	publishUIEvent([](Actor* a) {a->UpdateActor(); });
}

void amp::AMP_Engine::PostSceneUpdateCallback()
{
	sound.FadeOutPlayers();
	events.publishEvent(postSceneUpdate);
}

void amp::AMP_Engine::publishUIActorComponentsUpdate()
{
	publishUIEvent([](Actor* a) {a->ComponentUpdate(); });
}

void amp::AMP_Engine::publishSceneActorComponentsUpdate()
{
	publishActorEvent([](Actor* a) {a->ComponentUpdate(); });
}

void amp::AMP_Engine::OnTouchCallback(int32_t x, int32_t y)
{
	events.publishEvent(touchUpdate,x,y);
}


void amp::AMP_Engine::BeforeSystemShutdownCallback()
{
	events.publishEvent(beforeShutdown);
	publishActorEvent([](Actor* a) {a->ComponentsShutdown(); a->BeforeSystemShutdown();});
	publishUIEvent([](Actor* a) {a->ComponentsShutdown(); a->BeforeSystemShutdown(); });
	events.clear();
	sound.clear();
}

void amp::AMP_Engine::publishActorEvent(FuncHandle_t function)
{
	for (int k = 0; k < currentScene.CurrentSceneActorArray.count(); k++) {
		auto* ref = currentScene.CurrentSceneActorArray[k].get();
		function(ref);
	}
}

void amp::AMP_Engine::publishUIEvent(FuncHandle_t function)
{
	for (int k = 0; k < currentScene.CurrentSceneUIArray.count(); k++) {
		auto* ref = currentScene.CurrentSceneUIArray[k].get();
		function(ref);
	}
}






