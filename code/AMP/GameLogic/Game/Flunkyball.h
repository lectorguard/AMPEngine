#pragma once

#include "AMP_Engine.h"
#include "android/asset_manager.h"
#include "string"
#include "Text.h"
#include "memory"
#include "Scenes/SingleplayerScene.h"
#include "Scenes/Menu.h"
#include "Scenes/ClientScene.h"
#include "Scenes/ServerScene.h"
#include "Static/Camera.h"


class Flunkyball : public amp::AMP_Engine
{
public:
	std::unique_ptr<SingleplayerScene> singleplayerScene = nullptr;
	std::unique_ptr<Menu> menuScene = nullptr;
	std::unique_ptr<ClientScene> clientScene = nullptr;
	std::unique_ptr<ServerScene> serverScene = nullptr;

	Flunkyball(ANativeActivity* activity):AMP_Engine(activity) {
		singleplayerScene = std::make_unique<SingleplayerScene>(this);
		menuScene = std::make_unique<Menu>(this);
		clientScene = std::make_unique<ClientScene>(this);
		serverScene = std::make_unique<ServerScene>(this);
	};
	
	//INPUT
	TEXT onlineGameServer = TEXT("onlineGameServer");
	TEXT onlineGameClient = TEXT("onlineGameClient");
	TEXT tutorialGame = TEXT("tutorialGame");
	TEXT fromTutorialToMenu = TEXT("FromTutorialToMenu");
	TEXT fromClientGameToMenu = TEXT("fromClientGameToMenu");
	TEXT fromServerGameToMenu = TEXT("fromServerGameToMenu");
	TEXT fromMenuToMenu = TEXT("fromMenuToMenu");
	//OUTPUT
	TEXT startSendData = TEXT("startSendData"); //One Param: std::string ip
	TEXT startReceiveData = TEXT("startReceiveData"); //One Param: std::string ip
	
protected:
	void update() override;


	void BeforeSystemShutdown() override;


	void AfterWindowStartup() override;

	void SubLevelTransitions();

	void StartOnlineGameServer(std::string ip_Other);
	void StartOnlineGameClient(std::string ip_Other);
	void StartTutorialGame();

	void FromTutorialToMenu();
	void FromClientToMenu();
	void FromServerToMenu();
	void FromMenuToMenu();
};

