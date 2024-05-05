
#include "Flunkyball.h"

void Flunkyball::AfterWindowStartup()
{
	frameBuffer.setShaderPath("Shader/CustomFrameBufferShader.glsl");
	float aspect = ampContext.getWidth() / (float)ampContext.getHeight(); //Samsung tablet 1920/1200
	amp::Camera::setProjection(glm::perspective(glm::radians(60.0f), aspect, 0.1f, 1000.0f));
	physics.SetGravity(0, -99.81, 0);
	menuScene.get()->loadScene();
	SubLevelTransitions();

}

void Flunkyball::SubLevelTransitions()
{
	//Transitions
	events.subscribeEvent(onlineGameServer, &Flunkyball::StartOnlineGameServer, this);
	events.subscribeEvent(onlineGameClient, &Flunkyball::StartOnlineGameClient, this);
	events.subscribeEvent(tutorialGame, &Flunkyball::StartTutorialGame, this);
	events.subscribeEvent(fromTutorialToMenu, &Flunkyball::FromTutorialToMenu, this);
	events.subscribeEvent(fromClientGameToMenu, &Flunkyball::FromClientToMenu, this);
	events.subscribeEvent(fromServerGameToMenu, &Flunkyball::FromServerToMenu, this);
	events.subscribeEvent(fromMenuToMenu, &Flunkyball::FromMenuToMenu, this);
}

void Flunkyball::StartOnlineGameServer(std::string ip_Other)
{
	menuScene.get()->unloadScene();
	serverScene.get()->loadScene();
	events.publishEvent(startSendData, ip_Other);
	SubLevelTransitions();
}

void Flunkyball::StartOnlineGameClient(std::string ip_Other)
{
	menuScene.get()->unloadScene();
	clientScene.get()->loadScene();
	events.publishEvent(startReceiveData, ip_Other);
	SubLevelTransitions();
}

void Flunkyball::StartTutorialGame()
{
	menuScene.get()->unloadScene();
	singleplayerScene.get()->loadScene();
	SubLevelTransitions();
}

void Flunkyball::FromTutorialToMenu()
{
	singleplayerScene.get()->unloadScene();
	menuScene.get()->loadScene();
	SubLevelTransitions();
}

void Flunkyball::FromClientToMenu()
{
	clientScene.get()->unloadScene();
	menuScene.get()->loadScene();
	SubLevelTransitions();
}

void Flunkyball::FromServerToMenu()
{
	serverScene.get()->unloadScene();
	menuScene.get()->loadScene();
	SubLevelTransitions();
}

void Flunkyball::FromMenuToMenu()
{
	menuScene.get()->unloadScene();
	menuScene.get()->loadScene();
	SubLevelTransitions();
}

void Flunkyball::update()
{

}

void Flunkyball::BeforeSystemShutdown()
{

}


