#include "pch.h"
#include "ProgressBars.h"

void ProgressBars::Init()
{
	engine->events.subscribeEvent(progressBarsReset, &ProgressBars::Reset, this);
	engine->events.subscribeEvent(updatePlayerProgress, &ProgressBars::UpdatePlayerProgress, this);
	engine->events.subscribeEvent(setPlayerProgress, &ProgressBars::SetPlayerProgress, this);
}

void ProgressBars::Start()
{
	isProgress = true;
}

void ProgressBars::UpdatePlayerProgress(Player player)
{
	if (!playerOne || !playerTwo || !isProgress)return;
	switch (player)
	{
	case One:
		updateProgess(playerOne, player);
		break;
	case Two:
		updateProgess(playerTwo, player);
		break;
	}
}

void ProgressBars::SetPlayerProgress(Player player, float progress)
{
	if (player == One)CheckEvents(progress, Player::One, playerOne);
	else CheckEvents(progress, Player::Two, playerTwo);
}

void ProgressBars::Reset()
{
	isProgress = false;
}

void ProgressBars::updateProgess(amp::ProgressBar* bar, Player player)
{
	float currProgress = bar->getProgress();
	currProgress -= drinkPerIcon;
	CheckEvents(currProgress, player, bar);
}

void ProgressBars::CheckEvents(float currProgress, Player player, amp::ProgressBar* bar)
{
	if (currProgress <= 0.0f) {
		Reset();
		engine->events.publishEvent(onWinningGame, player);
	}
	else {
		bar->UpdateProgress(currProgress);
	}
	engine->events.publishEvent(onChangingProgress, player, currProgress);
}
