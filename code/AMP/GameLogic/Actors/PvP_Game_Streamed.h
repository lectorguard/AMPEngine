#pragma once

#include "Actor.h"
#include "AMP_Engine.h"
#include "Event/EventSystem.h"
#include "GameUtility.h"
#include "TaskController.h"


class PvP_Game_Streamed : public amp::Actor
{
public:
	amp::TaskController * controller = nullptr;
	PvP_Game_Streamed() {
		controller = CreateTaskController();
	};
	PvP_Game_Streamed(const PvP_Game_Streamed&) = delete;
	PvP_Game_Streamed(PvP_Game_Streamed&&) = delete;
	virtual ~PvP_Game_Streamed() {};

	amp::Actor* streamedAttacker = nullptr;
	amp::Actor* streamedDefender = nullptr;


	virtual void AfterWindowStartup() override;

	void OnPostInit();

	void OnPostPostInit();

	void StartRound(GameStagesOffline stage);

	void EndRound(GameStagesOffline stage);

	void EndStreamedAttack();

	void EndStreamedDefender();

	void OnGameEnd(Player p);

	void BeforeSystemShutdown() override;

	void GoBackToMenu();

	void StartGoBackToMenuLoadingScreen();
private:
	bool hasGameEnded = false;
	float menuDelayTime = 4.0f;
};
