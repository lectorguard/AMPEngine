#pragma once

#include "Actor.h"
#include "AMP_Engine.h"
#include "Event/EventSystem.h"
#include "GameUtility.h"
#include "TaskController.h"


class PvE_Game : public amp::Actor
{
public:
	amp::TaskController* controller = nullptr;
	PvE_Game() {
		controller = CreateTaskController();
	};
	PvE_Game(const PvE_Game&) = delete;
	PvE_Game(PvE_Game&&) = delete;
	virtual ~PvE_Game() {};

	amp::Actor* attackerPlayer = nullptr;
	amp::Actor* attackerNetwork = nullptr;
	amp::Actor* defenderPlayer = nullptr;
	amp::Actor* defenderNetwork = nullptr;




	virtual void AfterWindowStartup() override;

	void OnPostInit();

	void OnPostPostInit();

	void StartRound(GameStagesOffline stage);

	void EndRound(GameStagesOffline stage);

	void EndAttackPlayer();

	void EndAttackCPU();

	void EndDefendPlayer();

	void EndDefendCPU();

	GameStagesOffline CreateRandomStartingStage();

	void OnWinningGame(Player player);

	void GoBackToMenu();

	void StartGoBackToMenuLoadingScreen();

	virtual void BeforeSystemShutdown() override;
private:
	bool hasGameEnded = false;
	float menuDelayTime = 4.0f;
};
