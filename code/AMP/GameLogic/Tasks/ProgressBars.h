#pragma once
#include "TaskController.h"
#include "Components/ProgressBar.h"

enum Player
{
	One, Two
};


class ProgressBars : public amp::Task
{
public:
	//Player One is always the player, playing on the device
	amp::ProgressBar* playerOne = nullptr;
	amp::ProgressBar* playerTwo = nullptr;
	ProgressBars(amp::ProgressBar* first, amp::ProgressBar* second) {
		playerOne = first;
		playerTwo = second;
	};
	virtual ~ProgressBars() {};


	//INPUT
	TEXT progressBarsReset = TEXT("progressBarsReset");			//No Params: RESET has always 0 params
	TEXT updatePlayerProgress = TEXT("updatePlayerProgress");	//One Param: Player player
	TEXT setPlayerProgress = TEXT("setPlayerProgress");			//Two Param: Player player, float progress
	
	//OUTPUT
	TEXT onWinningGame = TEXT("onWinningGame");					//One Param: Player player
	TEXT onChangingProgress = TEXT("onChangingProgress");		//Two Params: Player player, float currentProgress


	const float drinkPerIcon = 0.008f;

	virtual void Init() override;

	virtual void Start() override;

	void UpdatePlayerProgress(Player player);

	void SetPlayerProgress(Player player, float progress);

	virtual void Reset() override;

private:
	void updateProgess(amp::ProgressBar* bar, Player player);

	void CheckEvents(float currProgress, Player player, amp::ProgressBar* bar);

	bool isProgress = false;

};