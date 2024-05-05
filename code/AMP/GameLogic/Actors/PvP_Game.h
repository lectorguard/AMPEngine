#pragma once

#include "Actor.h"
#include "AMP_Engine.h"
#include "Event/EventSystem.h"
#include "GameUtility.h"
#include "TaskController.h"


class PvP_Game : public amp::Actor
{
public:
	amp::TaskController * controller = nullptr;
	PvP_Game() {
		controller = CreateTaskController();
	};
	PvP_Game(const PvP_Game&) = delete;
	PvP_Game(PvP_Game&&) = delete;
	virtual ~PvP_Game() {};

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

	void EndAttackNetwork();

	void EndDefendPlayer();

	void EndDefendNetwork();

	void OnWinningGame(Player player);

	void BeforeSystemShutdown() override;

	void GoBackToMenu();

	void StartGoBackToMenuLoadingScreen();
private:
	bool hasGameEnded = false;
	float menuDelayTime = 4.0f;
};

