#pragma once
#include "TaskController.h"
#include "Components/Image.h"

enum Game {
	Win, Loose
};

class WinLoose : public amp::Task
{
public:
	amp::Image* images = nullptr;
	WinLoose(amp::Image* img) {
		this->images = img;
	};
	virtual ~WinLoose() {};


	//INPUT
	TEXT winLooseReset = TEXT("WinLooseReset");		//No Params: RESET has always 0 params
	TEXT startDisplay = TEXT("startDisplay");		//One Param: Game game
	TEXT startLoading = TEXT("startLoadingScreen"); //No Params
	TEXT endDisplay = TEXT("endDisplay");			//No Params


	const glm::vec2 location = glm::vec2(600, 960);
	const int sizeX = 600;
	const int sizeY = 600;

	const glm::vec2 P_location = glm::vec2(0.5f, 0.5f);
	const int P_SizeUniformFromX = 1.0f;

	virtual void Init() override;

	void StartDisplay(Game game);

	void StartLoadingScreen();

	void EndDisplay();

	virtual void Reset() override;
private:
	bool isDisplaying = false;

	int winLoseSoundIndex[3] = { 0 };
};
