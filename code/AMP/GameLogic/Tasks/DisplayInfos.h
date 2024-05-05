#pragma once
#include "TaskController.h"
#include "Components/Image.h"


class DisplayInfos : public amp::Task
{
public:
	amp::Image * images = nullptr;
	DisplayInfos(amp::Image* img) {
		this->images = img;
	};
	virtual ~DisplayInfos() {};


	//INPUT
	TEXT DisplayInfosReset = TEXT("DisplayInfosReset");		//No Params: RESET has always 0 params
	TEXT startDisplayWaitForPlayers = TEXT("startDisplayWaitForPlayers");	//No Params
	TEXT endDisplay = TEXT("endDisplay");					//No Params

	const glm::vec2 P_location = glm::vec2(0.5f, 0.5f);
	const float P_SizeX = 1.0f;
	const float P_SizeY = 0.5f;

	virtual void Init() override;

	void StartWaitForPlayers(std::string ip_other);

	void StopDisplayWaitForPlayers();

	void EndDisplay();

	virtual void Reset() override;
private:
	bool isDisplayingWaitForPlayers = false;
	amp::ImageInfo waitForPlayersImage; 

};
